# VisX
The VisX project is a set of tools contained in one application which serve the purpose to help demonstrate and calculate
various values in the scope of physics, such as Uncertainty and Significant Figures. Currently, Uncertainty and Significant Figures
are the only planned features of this project, but there exist plans to expand to kinematics and electricity later.

This project is written in C++ with hooks for C coders. The project is divided into sections *backend* and *frontend*. The *backend* is a static library
which contains the code which serves to calculate the various values, while the *frontend* serves to display the calculations.
There are two different front-ends, a *console* and a *GUI*.

The GUI is coded with wxWidgets ([https://github.com/wxWidgets/wxWidgets]).
According to wxWidgets' modified LGPL license, this project would not be required to include sources, even in the case of static linking.
Thus, when distributing the binaries of the project, only the source code of this project will be distributed. Nonetheless, proper credit
to the creators and contributors of *wxWidgets* will be given.
