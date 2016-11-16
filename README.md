IIDuinoX
========

IIDuinoX is a simple open-source implementation of a
Beatmania IIDX controller based on Arduino and USB HID.

All that is needed is some appropriate arcade buttons, a USB-capable Arduino
and an encoder system for the turntable -- I repurposed a DJ Hero turntable.

Accuracy and Timing
===================

I've never actually seen a real IIDX arcade machine or controller, so the
firmware target is simply "as low latency as possible" and "what feels
acceptable", rather than trying to match arcade timings.

(If someone with arcade experience wants to contribute, feel free! :) )

The keys are scanned every 250us. Any faster than that, and the USB HID system
starts dropping updates.

Due to software debouncing, the keys have a minimum latency of 2ms. In my
tests, I can reduce the debouncing mask to around 1ms before my cheap
microswitches start getting uncomfortably jumpy.

Getting the turntable working well is an ongoing struggle. I'm using a
repurposed DJ Hero controller as the turntable for my prototype, which has an
incredible amount of shutter windows for it's encoder. Most of the turntable
handling code is just to slow down the turntable readings into something
comprehensible. If you make your own encoder system, you will certainly need
to adjust the turntable timing values.

The turntable has a minimum latency of 16ms (1ms debounce * 16 shutters to
activate), and has a 200ms cooldown before it can be hit in the same direction
consecutively. That means for scratch notes less than 200ms apart, you need to
scratch back and forth. This prevents generating multiple scratches when the
turntable spins too fast. This seems to work pretty well for me.

I'm playing mostly level 8s and 9s, and the turntable is definitely what's
holding up my progression, but I don't know if it's just my inexperience with
the turntable or a problem with the firmware. I wasn't that great at precise
scratching in DJ Hero either. Comments from more skilled players are welcome.

Construction
============

I basically just put everything in a MDF box.

More detailed construction information might appear here someday,
after I build a better unit than my prototype. :)
