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
