// use this if you want shortcut to common colors
#define INSPIRE3D_DISPLAY_COMMON_COLOR 
#define CH32V003_I2C_IMPLEMENTATION // using i2c

#include "ch32v003fun.h"
#include <stdio.h>
#include "driver.h"
#include "inspire3d_display.h"
/// coords to led index
// Back layer
// 0,5,10,15,20
// 1,6,11,16,21
// 2,7,12,17,22
// 3,8,13,18,23
// 4,9,14,19,24

// Front layer
// 100,105,110,115,120
// 101,106,111,116,121
// 102,107,112,117,122
// 103,108,113,118,123
// 104,109,114,119,124

Inspire3D_Color canvas[125];

// https://stackoverflow.com/a/29019938
#define MINDIFF 2.25e-308                   // use for convergence check

double sqroot(double square)
{
    double root=square/3, last, diff=1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}

void pushCanvas(Inspire3D_Display * display, Inspire3D_Color buffer[125]){
    for(int i = 0; i < 125; i++){
        Inspire3D_Display_SetColor(display, i, buffer[i]);
    }
    Inspire3D_Display_Update(display);
}


void show_color_pallete(Inspire3D_Display * display){
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 5; y++){
            for(int z = 0; z < 5;z++){
                uint8_t index = Inspire3D_Display_Coords2Index(x,y,z);
                Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
            }
        }
    }
    Inspire3D_Display_Update(display);
    Delay_Ms(1000);
}

// reference: https://www.geeksforgeeks.org/bresenhams-algorithm-for-3-d-line-drawing/ C++ implementation
void drawLine(Inspire3D_Color * buffer,uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    printf("Drawing line\n");
    int dx = x2 > x1 ? x2 - x1 : x1 - x2;
    int dy = y2 > y1 ? y2 - y1 : y1 - y2;
    int dz = z2 > z1 ? z2 - z1 : z1 - z2;
    int xs = (x2 > x1) ? 1 : -1;
    int ys = (y2 > y1) ? 1 : -1;
    int zs = (z2 > z1) ? 1 : -1;

    buffer[Inspire3D_Display_Coords2Index(x1, y1, z1)] = color;

    // Driving axis is X-axis
    if (dx >= dy && dx >= dz) {
        int p1 = 2 * dy - dx;
        int p2 = 2 * dz - dx;
        while (x1 != x2) {
            printf("x1: %d, y1: %d, z1: %d\n",x1,y1,z1);
            x1 += xs;
            if (p1 >= 0) {
                y1 += ys;
                p1 -= 2 * dx;
            }
            if (p2 >= 0) {
                z1 += zs;
                p2 -= 2 * dx;
            }
            p1 += 2 * dy;
            p2 += 2 * dz;
            buffer[Inspire3D_Display_Coords2Index(x1, y1, z1)] = color;
        }
    }
    // Driving axis is Y-axis
    else if (dy >= dx && dy >= dz) {
        int p1 = 2 * dx - dy;
        int p2 = 2 * dz - dy;
        while (y1 != y2) {
            printf("x1: %d, y1: %d, z1: %d\n",x1,y1,z1);
            y1 += ys;
            if (p1 >= 0) {
                x1 += xs;
                p1 -= 2 * dy;
            }
            if (p2 >= 0) {
                z1 += zs;
                p2 -= 2 * dy;
            }
            p1 += 2 * dx;
            p2 += 2 * dz;
            buffer[Inspire3D_Display_Coords2Index(x1, y1, z1)] = color;
        }
    }
    // Driving axis is Z-axis
    else {
        int p1 = 2 * dy - dz;
        int p2 = 2 * dx - dz;
        while (z1 != z2) {
            printf("x1: %d, y1: %d, z1: %d\n",x1,y1,z1);
            z1 += zs;
            if (p1 >= 0) {
                y1 += ys;
                p1 -= 2 * dz;
            }
            if (p2 >= 0) {
                x1 += xs;
                p2 -= 2 * dz;
            }
            p1 += 2 * dy;
            p2 += 2 * dx;
            buffer[Inspire3D_Display_Coords2Index(x1, y1, z1)] = color;
        }
    }
    buffer[Inspire3D_Display_Coords2Index(x2, y2, z2)] = color;
}

// 2 point as diameter
// only works when z1 == z2
// reference https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
// and https://en.wikipedia.org/wiki/Midpoint_circle_algorithm#Jesko's_Method
void _plotCirclePoints(Inspire3D_Color * buffer, int xc, int yc, int x, int y, uint8_t z, Inspire3D_Color color) {
    if (xc + x >= 0 && xc + x < 5 && yc + y >= 0 && yc + y < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc + x, yc + y, z)] = color;
    }
    if (xc - x >= 0 && xc - x < 5 && yc + y >= 0 && yc + y < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc - x, yc + y, z)] = color;
    }
    if (xc + x >= 0 && xc + x < 5 && yc - y >= 0 && yc - y < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc + x, yc - y, z)] = color;
    }
    if (xc - x >= 0 && xc - x < 5 && yc - y >= 0 && yc - y < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc - x, yc - y, z)] = color;
    }
    if (xc + y >= 0 && xc + y < 5 && yc + x >= 0 && yc + x < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc + y, yc + x, z)] = color;
    }
    if (xc - y >= 0 && xc - y < 5 && yc + x >= 0 && yc + x < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc - y, yc + x, z)] = color;
    }
    if (xc + y >= 0 && xc + y < 5 && yc - x >= 0 && yc - x < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc + y, yc - x, z)] = color;
    }
    if (xc - y >= 0 && xc - y < 5 && yc - x >= 0 && yc - x < 5) {
        buffer[Inspire3D_Display_Coords2Index(xc - y, yc - x, z)] = color;
    }
}

void drawCircle(Inspire3D_Color * buffer,uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    if(z1 != z2){
        return;
    }
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int r = sqroot((double)(dx*dx+dy*dy));
    int x = 0, y = r;
    int xc = (x1 + x2) / 2;
    int yc = (y1 + y2) / 2;
    int d = 3 - 2 * r;
    _plotCirclePoints(buffer,xc, yc, x, y, z1,color);
    while (y >= x){
      
        // check for decision parameter
        // and correspondingly 
        // update d, y
        if (d > 0) {
            y--; 
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;

        // Increment x after updating decision parameter
        x++;
        
        // Draw the circle using the new coordinates
        _plotCirclePoints(buffer,xc, yc, x, y, z1,color);

    }
}

void drawBox(Inspire3D_Color * buffer,uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    // only draw outline
    int x = x1>x2?x2:x1;
    int ex = x1>x2?x1:x2;
    int ey = y1>y2?y1:y2;
    int ez = z1>z2?z1:z2;
    for(; x <= ex; x++){
        int y = y1>y2?y2:y1;
        for(; y <= ey; y++){
            int z = z1>z2?z2:z1;
            for(; z <= ez; z++){
                if(((x == x1)+(x == x2)+(y == y1)+(y == y2)+(z == z1)+(z == z2))>1){
                    buffer[Inspire3D_Display_Coords2Index(x,y,z)] = color;
                }
            }
        }
    }
}


typedef enum {
    CANVAS_POINT,
    CANVAS_LINE,
    CANVAS_CIRCLE,
    CANVAS_BOX,
    CANVAS_FILL,
    END_OF_CANVAS_MODE
} CANVAS_MODE;


void show_canvas_mode(Inspire3D_Display * display, CANVAS_MODE mode){
    printf("mode: %d\n",mode);
    Inspire3D_Color temp[125];
    for(int i = 0; i < 125; i++){
        temp[i] = Inspire3D_Color_Black;
    }
    switch(mode){
        case CANVAS_POINT:
            break;
        case CANVAS_LINE:
            drawLine((Inspire3D_Color*)&temp,0,0,0,4,4,4,Inspire3D_Color_Red);
            break;
        case CANVAS_CIRCLE:
            drawCircle((Inspire3D_Color*)&temp,1,1,4,3,3,4,Inspire3D_Color_Green);
            break;
        case CANVAS_BOX:
            drawBox((Inspire3D_Color*)&temp,0,0,0,4,4,4,Inspire3D_Color_Blue);
            break;
        case CANVAS_FILL:
            Inspire3D_Display_SetBGColor(display, Inspire3D_Color_Yellow);
            Inspire3D_Display_Update(display);
            Delay_Ms(1000);
            return;
            break;
        default:
            break;
    }
    pushCanvas(display, temp);
    Delay_Ms(1000);
}

void blink_point(Inspire3D_Display * display, int index, Inspire3D_Color color, Inspire3D_Color selected_color){
    // blink color with the maze
    if(color.r == selected_color.r && color.g == selected_color.g && color.b == selected_color.b){
        if(color.r == 0 && color.g == 0 && color.b == 0){
            Inspire3D_Display_SetColor(display, index, Inspire3D_Color_White);
        }else{
            Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Black);
        }
    }else{
        Inspire3D_Display_SetColor(display, index, selected_color);
    }
    Inspire3D_Display_Update(display);
    Delay_Ms(200);
    Inspire3D_Display_SetColor(display, index, color);
    Inspire3D_Display_Update(display);

}


char display_buffer[sizeof(Inspire3D_Display)];
int main(void) {
    SystemInit();
    GPIO_ADCinit();
    MY_I2C_init();

    printf("I2C initialized\n");
    // int seed = 0;
    int x = 0,y = 0,z = 0;// selector coords
    Inspire3D_Display * display = (Inspire3D_Display *)&display_buffer;
    Inspire3D_Display_Init(display,GPIOA, PA2);
    Inspire3D_Display_SetBrightness(display, .02);
    Inspire3D_Display_Clear(display);// reset data and update
    uint8_t mode = 0; //0: canvas; 1: color pallete
    CANVAS_MODE canvas_mode = CANVAS_POINT;
    Inspire3D_Color selected_color = Inspire3D_Color_White;
    int points_buff[2] = { -1, -1};
    uint8_t points_buff_index = 0;
    printf("Main Loop\n");
    while(1){
        // read ABCD key
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        bool compound_detected = (arrow != ARROW_NOT_FOUND && abcd != ABCD_NOT_FOUND);
        if(mode == 0){
            // search compound key 
            if(arrow == ARROW_DOWN && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = -1;
                points_buff[1] = -1;
                points_buff_index = 0;
                mode = 1;
                show_color_pallete(display);
            }else if(arrow == ARROW_UP && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = -1;
                points_buff[1] = -1;
                points_buff_index = 0;
                // decrement paint mode
                canvas_mode -= 1;
                canvas_mode = canvas_mode < 0 ? 1 : canvas_mode;
                canvas_mode = canvas_mode >= END_OF_CANVAS_MODE ? END_OF_CANVAS_MODE - 1 : canvas_mode;
                show_canvas_mode(display,canvas_mode);
                pushCanvas(display, canvas);
            }else if(arrow == ARROW_UP && abcd == ABCD_D){
                // clear buffer
                points_buff[0] = -1;
                points_buff[1] = -1;
                points_buff_index = 0;
                // increment paint mode
                canvas_mode += 1;
                canvas_mode = canvas_mode % END_OF_CANVAS_MODE;
                show_canvas_mode(display,canvas_mode);
                pushCanvas(display, canvas);
            }else if(arrow == ARROW_LEFT && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = -1;
                points_buff[1] = -1;
                points_buff_index = 0;
                // clear canvas
                for(int i = 0; i < 125; i++){
                    canvas[i] = Inspire3D_Color_Black;
                }
                Inspire3D_Display_Clear(display);
                Inspire3D_Display_Update(display);
            }
        }
        // normal moving logic
        if(!compound_detected){
        if(arrow == ARROW_UP){
            y = (y + 1) % 5;
        } else if(arrow == ARROW_DOWN){
            y = (y - 1) % 5;
            y = y < 0 ? 4 : y;
        } else if(arrow == ARROW_RIGHT){
            x = (x + 1) % 5;
        } else if(arrow == ARROW_LEFT){
            x = (x - 1) % 5;
            x = x < 0 ? 4 : x;
        }
        if(abcd == ABCD_A){
            z = (z + 1) % 5;
        } else if(abcd == ABCD_B){
            z = (z - 1) % 5;
            z = z < 0 ? 4 : z;
        }
        //select
        if(abcd == ABCD_D || abcd == ABCD_C){
            if(mode == 1){
                selected_color = Inspire3D_Color_setRGB(y*50,x*50,z*50);
                Inspire3D_Display_Clear(display);
                mode = 0;
            }else if(mode == 0){
                canvas[Inspire3D_Display_Coords2Index(x,y,z)] = selected_color;
                points_buff[points_buff_index] = Inspire3D_Display_Coords2Index(x,y,z);
                points_buff_index = (points_buff_index + 1) % 2;
                if(points_buff_index == 0){
                    uint8_t x1,y1,z1,x2,y2,z2;
                    Inspire3D_Display_Index2Coords(points_buff[0], &x1, &y1, &z1);
                    Inspire3D_Display_Index2Coords(points_buff[1], &x2, &y2, &z2);
                    switch(canvas_mode){
                        case CANVAS_LINE:
                            drawLine((Inspire3D_Color*)&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_CIRCLE:
                            drawCircle((Inspire3D_Color*)&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_BOX:
                            drawBox((Inspire3D_Color*)&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_FILL:
                            // fill
                            for(int i = 0; i < 125; i++){
                                canvas[i] = selected_color;
                            }
                            break;
                        case CANVAS_POINT:
                        default:
                            break;
                    }
                }
                pushCanvas(display, canvas);
            }
        }
        }
        // printf("x: %d, y: %d, z: %d\n",x,y,z);
        if(mode == 0)
            blink_point(display, Inspire3D_Display_Coords2Index(x,y,z), canvas[Inspire3D_Display_Coords2Index(x,y,z)],selected_color);
        else
            blink_point(display, Inspire3D_Display_Coords2Index(x,y,z), Inspire3D_Color_setRGB(y*50,x*50,z*50),Inspire3D_Color_Black);
        Delay_Ms(200);
    }
}
