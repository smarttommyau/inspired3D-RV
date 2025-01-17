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


void pushCanvas(Inspire3D_Display * display, Inspire3D_Color buffer[125]){
    for(int i = 0; i < 125; i++){
        Inspire3D_Display_SetColor(display, i, buffer[i]);
    }
    Inspire3D_Display_Update(display);
}


void show_color_pallete(Inspire3D_Display * display){
    Inspire3D_Display_Reset(display);
    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 5; y++){
            for(int z = 0; z < 5;z++){
                uint8_t index = Inspire3D_Display_Coords2Index(x,y,z);
                Inspire3D_Display_SetColor(display, index, Inspire3D_Color_setRGB(y*50,x*50,z*50));
            }
        }
    }
    Inspire3D_Display_Update(display);
}


void drawLine(Inspire3D_Color * buffer[125],uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int dz = abs(z2 - z1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int sz = z1 < z2 ? 1 : -1;
    int err1 = (dx > dy ? dx : -dy) / 2;
    int err2 = err1;
    while(1){
        uint8_t index = Inspire3D_Display_Coords2Index(x1,y1,z1);
        *buffer[index] = color;
        if(x1 == x2 && y1 == y2 && z1 == z2){
            break;
        }
        err2 = err1;
        if(err2 > -dx){
            err1 -= dy;
            x1 += sx;
        }
        if(err2 < dz){
            err1 += dx;
            y1 += sy;
        }
        if(err2 < dx){
            err1 += dz;
            z1 += sz;
        }
    }
}

// 2 point as diameter
// only works when z1 == z2
void drawCircle(Inspire3D_Color * buffer[125],uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    if(z1 != z2){
        return;
    }
    int r = abs(x2 - x1) / 2;
    int x = r;
    int y = 0;
    int err = 0;
    while(x >= y){
        uint8_t index = Inspire3D_Display_Coords2Index(x1+x,y1+y,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1+y,y1+x,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1-x,y1+y,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1-y,y1+x,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1-x,y1-y,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1-y,y1-x,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1+x,y1-y,z1);
        *buffer[index] = color;
        index = Inspire3D_Display_Coords2Index(x1+y,y1-x,z1);
        *buffer[index] = color;
        if(err <= 0){
            y += 1;
            err += 2*y + 1;
        }
        if(err > 0){
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

// Also 2 point as diameter
void drawSphere(Inspire3D_Color * buffer[125],uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2, Inspire3D_Color color){
    int r = abs(x2 - x1) / 2;
    int r2 = r * r;
    int cx = (x1 + x2) / 2;
    int cy = (y1 + y2) / 2;
    int cz = (z1 + z2) / 2;
    for(int x = -r; x <= r; x++){
        for(int y = -r; y <= r; y++){
            for(int z = -r; z <= r; z++){
                if(x*x + y*y + z*z <= r2){
                    uint8_t index = Inspire3D_Display_Coords2Index(cx + x, cy + y, cz + z);
                    *buffer[index] = color;
                }
            }
        }
    }
}

typedef enum {
    CANVAS_POINT,
    CANVAS_LINE,
    CANVAS_CIRCLE,
    CANVAS_SPHERE,
    END_OF_CANVAS_MODE
} CANVAS_MODE;


void show_canvas_mode(Inspire3D_Display * display, CANVAS_MODE mode){
    Inspire3D_Display_Reset(display);
    Inspire3D_Color temp[125];
    switch(mode){
        case CANVAS_POINT:
            break;
        case CANVAS_LINE:
            drawLine(&temp,0,0,0,4,4,4,Inspire3D_Color_Red);
            break;
        case CANVAS_CIRCLE:
            drawCircle(&temp,0,0,0,4,4,0,Inspire3D_Color_Green);
            break;
        case CANVAS_SPHERE:
            drawSphere(&temp,0,0,0,4,4,4,Inspire3D_Color_Blue);
            break;
        default:
            break;
    }
    pushCanvas(display, temp);
    Delay_Ms(1000);
}

void blink_point(Inspire3D_Display * display, int index, Inspire3D_Color color){
    // blink color with the maze
    if(color.r == Inspire3D_Color_Black.r && color.g == Inspire3D_Color_Black.g && color.b == Inspire3D_Color_Black.b){
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_White);
    }else{
        Inspire3D_Display_SetColor(display, index, Inspire3D_Color_Black);
    }
    Inspire3D_Display_Update(display);
    Delay_Ms(500);
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
    Inspire3D_Display_Clear(display);// reset data and update
    uint8_t mode = 0; //0: canvas; 1: color pallete
    CANVAS_MODE canvas_mode = CANVAS_POINT;
    Inspire3D_Color selected_color = Inspire3D_Color_Black;
    uint8_t points_buff[2] = { 0 };
    uint8_t points_buff_index = 0;
    printf("Main Loop\n");
    while(1){
        // read ABCD key
        uint16_t abcd_reading   = abcd_key_read_ADC();
        uint16_t arrow_reading  = arrow_key_read_ADC();
        ARROW_KEY arrow         = arrow_key_down(arrow_reading);
        ABCD_KEY abcd           = abcd_key_down(abcd_reading);
        if(mode == 0){
            // search compound key 
            if(arrow == ARROW_DOWN && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = 0;
                points_buff[1] = 0;
                points_buff_index = 0;
                mode = 1;
                show_color_pallete(display);
            }else if(arrow == ARROW_UP && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = 0;
                points_buff[1] = 0;
                points_buff_index = 0;
                // decrement paint mode
                canvas_mode -= 1;
                canvas_mode = canvas_mode < 0 ? 1 : canvas_mode;
                canvas_mode = canvas_mode >= END_OF_CANVAS_MODE ? END_OF_CANVAS_MODE - 1 : canvas_mode;
                show_canvas_mode(display,canvas_mode);
            }else if(arrow == ARROW_UP && abcd == ABCD_D){
                // clear buffer
                points_buff[0] = 0;
                points_buff[1] = 0;
                points_buff_index = 0;
                // increment paint mode
                canvas_mode += 1;
                canvas_mode = canvas_mode % END_OF_CANVAS_MODE;
                show_canvas_mode(display,canvas_mode);
            }else if(arrow == ARROW_LEFT && abcd == ABCD_C){
                // clear buffer
                points_buff[0] = 0;
                points_buff[1] = 0;
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
                selected_color = canvas[Inspire3D_Display_Coords2Index(x,y,z)];
            }else if(mode == 0){
                canvas[Inspire3D_Display_Coords2Index(x,y,z)] = selected_color;
                points_buff[points_buff_index] = Inspire3D_Display_Coords2Index(x,y,z);
                points_buff_index = (points_buff_index + 1) % 2;
                if(points_buff_index == 1){
                    uint8_t x1,y1,z1,x2,y2,z2;
                    Inspire3D_Display_Index2Coords(points_buff[0], &x1, &y1, &z1);
                    Inspire3D_Display_Index2Coords(points_buff[1], &x2, &y2, &z2);
                    switch(canvas_mode){
                        case CANVAS_LINE:
                            drawLine(&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_CIRCLE:
                            drawCircle(&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_SPHERE:
                            drawSphere(&canvas,x1,y1,z1,x2,y2,z2,selected_color);
                            break;
                        case CANVAS_POINT:
                        default:
                            break;
                    }
                }
                pushCanvas(display, canvas);
            }
        }
        printf("x: %d, y: %d, z: %d\n",x,y,z);
        blink_point(display, Inspire3D_Display_Coords2Index(x,y,z), canvas[Inspire3D_Display_Coords2Index(x,y,z)]);
        Delay_Ms(200);
    }
}
