# VisX
The VisX project is a set of tools contained in one application which serve the purpose to help demonstrate and calculate
various values in the scope of physics, such as Uncertainty and Significant Figures. Currently, Uncertainty and Significant Figures
are the only planned features of this project, but there exist plans to expand to kinematics and electricity later.

This project is written in C++ with hooks for C coders. The project is divided into sections *backend* and *frontend*. The *backend* is a static library
which contains the code which serves to calculate the various values, while the *frontend* serves to display the calculations.
There are two different front-ends, a *console* and a *GUI*.

The GUI is coded with wxWidgets (https://github.com/wxWidgets/wxWidgets).
According to wxWidgets' modified LGPL license, this project would not be required to include sources, even in the case of static linking.
Thus, when distributing the binaries of the project, only the source code of this project will be distributed. Nonetheless, proper credit
to the creators and contributors of *wxWidgets* will be given.

## Building
To build the library only, set cmake variable `VISX_NOEXE` to `TRUE`. To build the console version, set `VISX_CONONLY` to `TRUE`.
On Windows, this project is compiled using MinGW and the `MinGW Makefiles` generator. On Linux, this project is compiled with the
`Unix Makefiles` generator. On Mac, the project uses the XCode generator (that I don't recall the exact name for right now). So far,
this project's GUI has yet to be built on platforms other than Windows. As well, this project uses wxWidgets version 3.1.5. A tar
archive containing this release is within the project root. To build the project, it must be extracted to the `wxWidgets` directory.
On Linux, building requires GTK 3. On Debian, you can install it with
```sh
$ sudo apt install libgtk-3-dev
```
This program has not yet been built for Mac.
