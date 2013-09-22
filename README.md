VC0706
======

	Library for VC0706 serial ttl JPEG camera
	Written by Oskari Rauta
	License: expat ( check copying.txt )

	I like C more than Python - and because I wasn't able
	to fix existing python programs used to control VC0706
	camera - I decided to write my own for C.

	It's partially based on Adafruit's similar library for
	Arduino - but it is a complete rewrite - also with this
	library, you actually can use higher baudrates, like
	115200. Although, on Adafruit forums staff suggested
	people not to do so.. So that's up to you..

	I've tried to add here all the available functions of
	the camera, so user wouldn't need to fiddle with it
	and risk the hardware - after all, writing not supported
	data to the device might brick it.

	I didn't add OSD features or ZOOMing/PANning/TILTing
	features, as they don't do anything to my camera..
	Camera answers to them, but they just don't do anything,
	feel free to add if you want to..

	Warning: I am not sure if Downsizing works, command for
	downsizing was mentioned for both, setting and getting
	twice in the datasheet - and both times they were
	different.

	CREDITS: I didn't write serial library - I used existing
	one from wiringPI project. Thank you for great work.
	
	Compiles without modifications for atleast: mac os x &
	linux.

	Currently I've discovered only 1 minor issue: it needs to
	connect twice after reset as first time output will be
	garbage.. Fixing it someday..
