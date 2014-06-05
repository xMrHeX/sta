st-alienfx
==========

Fork of st.suckless.org


Patches applied:
----------------

- Theme support + theme randomizer
- Static xmp icon
- AlienFX code for the keyboard to match the current theme

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

- home/end and ctrl+arrows support for the command line
- Ctrl+Alt+x to reset the terminal
- repaint the application icon to match the current theme
- improve the theme switching to only color characters which have no color
