# 3D paint

## Description
Select color and draw points, straight line, circle and sphere in 3D space.<br>
arrows to control movement in 2D space(x,y), A/B to control movement of z.<br>
C/D to draw and select point<br>
up arrow + C/D control mode: 0:point, 1:line, 2:circle, 3: Box, 4: Fill<br>
down arrow + C: select color. then C/D to select<br>
left arrow + C: clear screen.<br>
right arrow + D: save screen to file.(different slot accoording to x-coordinate)<br>
right arrow + C: load screen from file.(different slot accoording to x-coordinate)<br>

### Note
> To support larger display some variables need to be changed to uint16_t
- save/load depends simply on x-coordinate to reduce flash size