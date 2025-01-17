# Description

* Windows/MacOS port for OLED/Buzzer/keyboard functions
* 'Emulator' folder is for compiling Windows and MacOs version

## How to use

* Type `make emulator` for compiling the emulator version on Windows/MacOs
* Run the executable

### Note
> If you run the executable on other directory, you will have to open the emulator/inspire3d_emu/index.html in your browser manually

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

### Layout of buttons
![buttons_layout](https://github.com/user-attachments/assets/2855c0de-9625-47f0-9548-8b75798563e1)

## Known limitation
> For blinking stuff, it is not emulating it well


## Notes for emulator

* Highly recommended to use dark theme terminal for best user experience
* For Windows, Disable "Device default effects" before using the `JOY_sound()` function.
* For MacOS version, go to settings -> Privacy & Security -> Accessibility and allow terminal/your IDE to control your computer to enable keyboard control (will pop up during first run, need to restart the program after enable)
