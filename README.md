# A super cheap plotter
-----------------------

### Note
This code is not portable, and I don't intend to make it so.
It is just hobby-ware, enough to draw what I want and hack on easily.

That said, if anyone has questions about the math used, or wants to try and port this to your own setup, I will be happy to help. Just message me. You can reach me on twitter at the same handle as here.

-----------------------


I have always wanted a plotter, but not enough to pay the hundreds of dollars for a professional one.
Still, at it's core it is just moving a pen around, right?

So I brainstormed a options, and looked at what sorts of parts I already had lying around. I eventually decided to go with suspended pen hanging from two stepper motors. Using belts as opposed to just chord meant that I could be fairly certain about the length of the arm.

### Parts List and Estimated Cost
I say estimated because most of this stuff I had lying around anyways.
* Teensy 3.2 ($20)
  * The Teensy is great, but this would probably work with most any simple dev board you have lying around
* 5mm Timing Belt & Gears ($15)
* Stepper Motors and Drivers ($4)
  * Got them in a set long time ago, specifically they are ULN2003 steppers
  * I was able to run these and everything off computer USB 5V, and never had problems
* Fine Tip Sharpie ($1)
  * Again, just whatever I had nearby
* Cardboard, a few loose screws, some putty ($0)
  * Maybe not technically 0 dollars, but I just used stuff I would have otherwise thrown out
  
So even if you didn't have anything lying about, I would guess you could get this working for under $50 dollars.

### How it works
Like I mentioned above, this is hobby ware, and doesn't suppor GCODE or SVG or whatever the cool kids are using these days. I have my own format I use. In the scripts folder you will find a python script that can generate a single stroke spiral path based on an image. It will output in the format I use.

The controller itself is able to know how to set the paths with some basic trig. I hardcode how far away the motors are from eachother, and the band length at startup time. I also have hardcoded lengths for corners of the canvas. I may fix this so that corners are adjustable at runtime later. Knowing this allows us to do some triangle math to find what lengths the bands need to be for any given x and y coordinate.

linearlly interpolating band lenghts between two points will not result in a straight line, so the path is expected to be broken up into pretty small chunks so that we get "close-enough" to a real straight line.

### Lessons learned
The biggest issues I ran into was working with such large numbers. All my lengths are measured in stepper motor steps, which are very fine. The lenghts all fit fine into a 32 bit integer, which is the register size for the teensy I was using. They fit fine, that is, until you try to square or multiply them. I had to cast to 64 bit integers in quite a few place to make sure I wasn't being killed by integer overflow.

Another lesson I learned was just how precise I didn't need to be. Small mistakes in measurement and inital position have a pretty not-noticable change on the output. They should introduce non-linearities but if I constrained my drawing to a decent size center they were not noticeable.
