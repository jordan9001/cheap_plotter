from PIL import Image
import sys
import math

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


def draw_spiral(rot = 16, step = 0.4):
    pts = [(0.5, 0.5)]
    
    stepcount = int(2*math.pi*rot // step)

    maxrad = 0.5
    spi_const = maxrad / (stepcount * step)
    for i in range(stepcount):
        ang = step * i
        r = spi_const * ang
        y = math.sin(ang) * r # SOH
        x = math.cos(ang) * r

        y = y + 0.5
        x = x + 0.5
        pts.append((x,y))

    return pts

def points_to_img(sz, pts, thick=2):
    if len(pts) < 2:
        return None

    ba = bytearray(b"\xff" * (sz[0] * sz[1]))

    prev = pts[0]
    for p in pts[1:]:
        lineto(ba, sz, p2px(prev, sz), p2px(p, sz), thick)
        prev = p
    
    return Image.frombytes("L", sz, bytes(ba))

def main():
    img_path = "C:/Users/jorda/Downloads/spiralme.png"
    if len(sys.argv) > 1:
        img_path = sys.argv[1]

    img = Image.open(img_path)
    img = img.convert(mode="L") # 8 bit pix, b & w

    pts = draw_spiral(32, 0.1)
    sbimg = points_to_img(img.size, pts, 1)
    Image.blend(img, sbimg, 0.5).show()

if __name__ == '__main__':
    main()