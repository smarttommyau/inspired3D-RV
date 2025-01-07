# Description

* Windows/MacOS port for OLED/Buzzer/keyboard functions
* 'Emulator' folder is for compiling Windows and MacOs version

## How to use

* Type `make emulator` for compiling the emulator version on Windows/MacOs
* Run the executable

## Controls

press keyboard to control the matrix emulator, listing out the keys

* W -> up
* A -> left
* S -> down
* D -> right
* I -> A
* K -> B
* J -> C
* L -> D

Layout of ABCD button:\
&emsp;&nbsp;&nbsp;&emsp;A\
&emsp;C&emsp;B&emsp;D

## Notes for emulator

* Highly recommended to use dark theme terminal for best user experience
* _OLED_refresh_display() is needed for emulators to print out the monitor compare to the RISC-V version
* For Windows, Disable "Device default effects" before using the `JOY_sound()` function.
* For MacOS version, go to settings -> Privacy & Security -> Accessibility and allow terminal/your IDE to control your computer to enable keyboard control (will pop up during first run, need to restart the program after enable)
