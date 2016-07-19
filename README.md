**BEFORE FILING AN ISSUE, READ THE RELEVANT SECTION IN THE [CONTRIBUTING](https://github.com/citra-emu/citra/blob/master/CONTRIBUTING.md#reporting-issues) FILE!!!**


mv /usr/local/Cellar/qt5/5.6.1-1 /usr/local/Cellar/qt5/5.6.2

https://github.com/citra-emu/citra/issues/1902


Dyld Error Message:
Library not loaded: /usr/local/Cellar/qt5/5.6.1-1/lib/QtWidgets.framework/Versions/5/QtWidgets
Referenced from: /Volumes/.../citra-20160719-f43c587-osx-amd64/citra-qt.app/Contents/Frameworks/QtOpenGL.framework/Versions/5/QtOpenGL
Reason: image not found


Dyld Error Message:
Library not loaded: /usr/local/Cellar/qt5/5.6.1-1/lib/QtWidgets.framework/Versions/5/QtWidgets
Referenced from: /Volumes/.../citra-20160714-f95d119-osx-amd64___/citra-qt.app/Contents/Frameworks/QtOpenGL.framework/Versions/5/QtOpenGL
Reason: image not found


Even after copying the .framework binaries, why does it not link to a local version (like distributing the dlls on Windows?) - instead still trying to pull up the brew version?


https://github.com/citra-emu/citra/issues/1902

Otool link? That doesn't work. Solid crash...


'This application failed to start because it could not find or load the Qt platform plugin "cocoa"' - debugging on OS X is fun!




Citra Emulator
==============
[![Travis CI Build Status](https://travis-ci.org/extramaster/citra.svg)](https://travis-ci.org/extramaster/citra)

Citra is an experimental open-source Nintendo 3DS emulator/debugger written in C++. It is written with portability in mind, with builds actively maintained for Windows, Linux and pretending to be on OS X. Citra only emulates a subset of 3DS hardware, and therefore is generally only useful for running/debugging homebrew applications. At this time, Citra is even able to boot several commercial games! Most of these do not run to a playable state, but we are working every day to advance the project forward.

Citra is licensed under the GPLv2 (or any later version). Refer to the license.txt file included. Please read the [FAQ](https://github.com/citra-emu/citra/wiki/FAQ) before getting started with the project.

Check out our [website](https://citra-emu.org/)!

For development discussion, please join us @ #citra on freenode.

### Development

Most of the development happens on GitHub. It's also where [our central repository](https://github.com/citra-emu/citra) is hosted.

If you want to contribute please take a look at the [Contributor's Guide](CONTRIBUTING.md), [TODO list](https://docs.google.com/document/d/1SWIop0uBI9IW8VGg97TAtoT_CHNoP42FzYmvG1F4QDA) and [Developer Information](https://github.com/citra-emu/citra/wiki/Developer-Information). You should as well contact any of the developers in the forum in order to know about the current state of the emulator.

### Building

* __Windows__: [Windows Build](https://github.com/citra-emu/citra/wiki/Building-For-Windows)
* __Linux__: [Linux Build](https://github.com/citra-emu/citra/wiki/Building-For-Linux)
* __OSX__: [OS X Build](https://github.com/citra-emu/citra/wiki/Building-For-OS-X)


### Support
If you like, you can [donate by PayPal](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=K899FANUJ2ZXW) - any donation received will go towards things like:
* 3DS consoles for developers to explore the hardware
* 3DS games for testing
* Any equipment required for homebrew
* Infrastructure setup
* Eventually 3D displays to get proper 3D output working
* ... etc ...

We also more than gladly accept used 3DS consoles, preferrably ones with firmware 4.5 or lower! If you would like to give yours away, don't hesitate to join our IRC channel #citra on [Freenode](http://webchat.freenode.net/?channels=citra) and talk to neobrain or bunnei. Mind you, IRC is slow-paced, so it might be a while until people reply. If you're in a hurry you can just leave contact details in the channel or via private message and we'll get back to you.
