from PIL import Image
import struct
import sys
import math
import os.path

def p2px(p, sz):
    return (int(p[0] * sz[0]), int(p[1] * sz[1]))

def px2p(px, sz):
    return (px[0] / sz[0], px[1] / sz[1])

def dot(ba, sz, p, thick=4):
    ft = math.floor(thick/2)
    ct = math.ceil(thick/2)
    for ix in range(p[0] - ft, p[0] + ct + 1):
        if 0 <= ix < sz[0]:
            for iy in range(p[1] - ft, p[1] + ct + 1):
                if 0 <= iy < sz[1]:
                    ba[(iy * sz[0]) + ix] = 0



def lineto(ba, sz, p1, p2, thick=4):
    # lazy, just do x and y loops

    runx = abs(p2[0] - p1[0])
    if runx > 0:
        dy = (p2[1] - p1[1]) / runx
        dx = 1 if p2[0] > p1[0] else -1
        for i in range(runx):
            x = p1[0] + (i * dx)
            y = int(p1[1] + (dy * i))
            dot(ba, sz, (x,y), thick)

    runy = abs(p2[1] - p1[1])
    if runy > 0:
        dx = (p2[0] - p1[0]) / runy
        dy = 1 if p2[1] > p1[1] else -1
        for i in range(runy):
            y = p1[1] + (i * dy)
            x = int(p1[0] + (dx * i))
            dot(ba, sz, (x,y), thick)


def draw_spiral_old(rot = 15, step = 0.4):
    pts = [(0.5, 0.5)]
    
    stepcount = int(2*math.pi*rot // step)

    maxrad = 0.5
    spi_const = maxrad / (stepcount * step)
    for i in range(stepcount):
        ang = step * i
        r = spi_const * ang
        y = math.sin(ang) * r
        x = math.cos(ang) * r

        y = y + 0.5
        x = x + 0.5
        pts.append((x,y))

    return pts

def get_val(img, px):
    return (256 - img.getpixel(px)) / 256

def draw_spiral_am(img, rot = 27, step = 0.001):
    # arch spiral: r = const*ang
    # const = maxradius / (2*pi*rot)
    # len = Integrate(sqrt((r)**2 + (dr/dang)**2), ang0, ang1)
    # len = Integrate(sqrt((const*ang)**2 + (Derive(const*ang, ang))**2), ang0, ang1)
    # len = Integrate(sqrt((const*ang)**2 + (const**2), ang0, ang1)
    # len = const Integrate(sqrt(ang**2 + 1), ang0, ang1)
    # len = const * 0.5 * ((sqrt(ang1**2 + 1) * ang1 + asinh(ang1)) - (sqrt(ang0**2 + 1) * ang0 + asinh(ang0)))
    # looks hard
    # can we get a good guess? Pretend it is part of a circle?
    # len = r * (ang1 - ang0)
    # len = const * ang0 * (ang1 - ang0)
    # Yeah, the difference goes to tiny very quickly between the real and fake len
    # ang1 = ang0 + (len/(const*ang0))

    pts = [(0.5, 0.5)]
    prev = (0,0)
    dist = 0

    const = 0.5 / (2 * math.pi * rot)
    ang = math.pi # can't start as 0
    while True:
        r = const * ang

        y = 0.5 + math.sin(ang) * r
        x = 0.5 + math.cos(ang) * r

        dist += math.sqrt((x - prev[0])**2 + (y - prev[1])**2)
        #dist += (x - prev[0]) + (y - prev[1])      # for fun, this is mahnatan dist
        #dist += max((x - prev[0]), (y-prev[1]))    # and this is l-inf

        prev = (x,y)

        # modulate
        v = get_val(img, p2px((x,y), img.size))
        w = math.sin(dist * 693) * const * math.pi
        w *= v
        r += w

        # get modulated x,y
        y = 0.5 + math.sin(ang) * r
        x = 0.5 + math.cos(ang) * r

        if x <= 0 or x >= 1 or y <= 0 or y >= 1:
            break

        pts.append((x,y))

        # get next ang
        ang = ang + (step/(const * ang))
    
    return [pts]


def points_to_img(sz, pts_list, thick=2, lines=True):

    ba = bytearray(b"\xff" * (sz[0] * sz[1]))

    for pts in pts_list:
        if lines and len(pts) >= 2:
            prev = pts[0]
            for p in pts[1:]:
                lineto(ba, sz, p2px(prev, sz), p2px(p, sz), thick)
                prev = p
        else:
            for p in pts:
                dot(ba, sz, p2px(p, sz), thick)
    
    return Image.frombytes("L", sz, bytes(ba))

MAX_P = 10000
def points_to_path(pts_list):
    # file format:  (all little endian)
    # rest of file len  uint32
    # number of lines   uint32
    #   [
    #       num points  uint32
    #       [
    #           x       uint16
    #           y       uint16
    #           ...
    #       ]
    #       ...
    #   ] 
    
    f = struct.pack("<I", len(pts_list))
    for pts in pts_list:
        f += struct.pack("<I", len(pts))
        for p in pts:
            f += struct.pack("<HH", int(p[0] * MAX_P), int(p[1] * MAX_P))
    
    f = struct.pack("<I", len(f)) + f

    return f

def total_pts(pts_list):
    n = 0
    for l in pts_list:
        n += len(l)
    return n

def main():
    img_path = "C:/Users/jorda/Downloads/spiralme.png"
    if len(sys.argv) > 1:
        img_path = sys.argv[1]

    img = Image.open(img_path)
    img = img.convert(mode="L") # 8 bit pix, b & w

    pts_list = draw_spiral_am(img, 27, 0.003)
    #pts = draw_spiral_am(img, 42, 0.0001)
    binf = points_to_path(pts_list)
    print(total_pts(pts_list), "points,", len(binf), "bytes")
    sbimg = points_to_img(img.size, pts_list, 1, True)
    #Image.blend(img, sbimg, 0.5).show()
    sbimg.show()
    img_name = img_path[:img_path.rfind('.')] + "_spiral"
    if os.path.exists(img_name + ".png"):
        i = 0
        while os.path.exists(img_name + str(i)+".png"):
            i += 1
        img_name = img_name + str(i)
    print(img_name + ".png")
    sbimg.save(img_name + ".png")
    print(img_name + ".bin")
    with open(img_name + ".bin", "wb") as fp:
        fp.write(binf)

if __name__ == '__main__':
    main()