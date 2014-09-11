Simple Terminal, Augmented
==========

Fork of st.suckless.org


Patches applied:
----------------

- Theme support + theme randomizer
- Dynamic NETWM icon to match the current theme bg & fg
- AlienFX code for the keyboard to match the current theme (requires libusb).

Thanks:
-------

Big kudos to @defanor for the instant theme switching code.<br />
Thanks to @Xqua for helping me figure out the AlienFX rgb math.

Disclaimer:
-----------

I do not bear responsibility for frying your AlienFX controller chip with my code.<br />
Haven't fried mine, but so just I sleep tighter...

Installation:
-------------

- Copy the `51-alienfx.rules` file to /etc/udev/rules.d/ dir and run `service udev restart` as su;
- Run `make clean && make st-alienfx`;
- Copy st-alienfx to /usr/bin as su;
- Make sure you're part of the _video_ group;


TODO
----

- improve the theme switching (only color characters which have no color, etc)

