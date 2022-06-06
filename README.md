# scalebotjr
Arduino based dual-scale espresso scale
See https://www.donationcoder.com/forum/index.php?topic=52308
See https://www.youtube.com/watch?v=byn376XC340

Showing off my latest electronics project, JrScaleBot, a custom made espresso scale system that uses two separate scales (one embedded into the drip tray of a Breville Bambino), and performs automatic shot timing, taring, ratio calculations, and recipe guidance.

This was built and coded over the course of about 6 weeks.  Would have been easier if I knew what I was doing.

The big box in the upper right controls the brains of the thing (an Arduino Mega), and there are three separate "modules" that connect via custom Usb-C cables to that.  The modules are: A small scale built into the bottom of a Breville Bambino, a standalone scale with some lcd displays (in the hollowed out shell of a Timemore black mirror scale), and a wood control panel box with some keys, a 20x4 screen, and a dial button for options.  After you make a coffee drink it logs the result to an Internet of Things database on the web.

The motivation for this project was my desire to avoid having to remember to do the manual "tare" operation constantly, and having to try to move the scale/timer under the cup, etc.  Originally my idea was for the machine to recognize the cup by apperance/rfid and subtract the known cup weight to avoid needing to tare.  This turned out to be a pain because of problems using marked cups in dishwasher or microwave.  So instead I turned to a workflow mode that made it fairly straightforward to figure out how to auto-tare the espresso and bean cups.  The main tricky part here was the need to have 2 separate scales feeing into one central processor that knows how to automatically switch between them.

Note that you can use the scale as a manual scale and timer, but what you see me doing here is using it in "workflow" mode, where I press my button (J) to advance it through the steps of weighing beans, pouring a shot, and then adding half+half and water to make a specific recipe proportional to bean weight.  I make a single shot because I enjoy the process of making multiple smaller espresso's rather than one double.

The Breville Bambino (not plus model) is a pretty bare bones espresso machine, but my primary criteria for an espresso machine is that it be small and can despense water, and that's what makes this machine so perfect in my way of thinking.
