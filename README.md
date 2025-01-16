# inspired3D-RV
inspired3d-RISC-V codes

# installation
- Build essentials for building c
  - e.g. use mounriver build tool
# build
- Run `make` in individual directory or build all in root
- Run `make emulator` to build emulator for each program
- Modify [ch32v003fun\ch32v003fun.mk](.\ch32v003fun\ch32v003fun.mk) change to prefix to your toolchain
# Program the RISC-V
## Pins required for programming
![Porgram](./images/program_pins.png)
> swio to swio
> 5v to 5v
> gnd to gnd 
## Pins for Serial communication(uart)
![Uart](./images/uart_pins.png)
> RX to UTX
> TX to URX


# TODO
- [x] Basic Button key detection 
- [ ] Button click detection
- [x] Basic LED control
- [x] emulator of inspired3d
- [ ] eeprome read/write
- [ ] Intresting games and animations
- [ ] Add more features

### Note
If you want to develop upon this, you may reference those test codes.
- `template` for all the basic setup required for developing a new program, using similar structure as arduino  
- `test_inspire3d_screen_control` for controlling inspired3d screen
- `test_inspire3d_button_detection` for button key detection
- `pong` for seeing the limitiation with abcd keys detection when controlling led display