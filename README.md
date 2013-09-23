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
	and risk the hardware - after all, writing non-supported
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

	---

	Some ground rules not mentioned in documents:
	1) Settings (imagesize, downsize and compression) always apply only after systemReset.
	2) systemReset is not necessary when starting communication with camera, when system resets, responseData contains:
		3 1.00
		Ctrl infr exist
		USER-defined sensor
		525
		Init end

	  after systemReset() baudrate is always 38400.
	3) if genVersion() reports strange characters, you should perform a systemReset(); check it once in a while - does not occur often, used baudrate does not affect, happens with both 38400 and 115200 - when the "thing" hits in the fan..
	4) Guidance for using higher baudrates? Camera always starts communication with 38400 when reset of power cycled. Check samples in src directory - function is called initiateConnection();
	5) Sometimes camera just looses (very rare, been toying with this thing for 2 days now and this happened once) itself, and you cannot communicate with it. In this case, cycle power for a moment, and try again. It should come up after awhile.

	---

	Version history
	1.0	22.9.2013	initial commit, fully functional except for the reset has minor issues
	1.1	23.9.2013	Fixed reset, added more documentation, made sample setImageSize to reset at the end. Removed unused variables and defines. Some clean up. Changed some static variables to dynamically allocated. Added class destructor.

