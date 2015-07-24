# smartWindow
Smart window that detects the amount of sun exposure and changes the opacity of a piece of electrochromic film.

This is a the repository for the  node js server code and our Energia code for the smartWindow project. 

In order to run these files on your local machine, you must create a Local Event Library because Energia does not have an official library for events.

To create this library create a Directory in $USER/Documents/Energia/Libraries  called Event. In the Event directory, place the following files.
	Event.h
	Event.cpp

To compile the Energia code, create a directory in $USER/Documents/Energia called myWifiSensor and copy and past all of the github files into it. That way we can create the project in the Energia "Sketchbook"

You should be able to compile just fine from there 

The smartwindow server directory contains the code that I wrote for the node-red server. To use this code, copy and paste the code into the appropriate function blocks
