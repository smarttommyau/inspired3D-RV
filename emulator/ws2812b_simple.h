#pragma once
#include "buttons.h"
#include "ch32v003fun.h"

// websocket server to connect to inspire3d_display emulator
#include "mongoose.h"

#include <stdio.h>

#define BORDER_X 2
//dumy
#define NUM_LEDS 0
#define horizontalButtons 0
#define verticalButtons 0


static const char *s_listen_on = "ws://localhost:8000";
struct mg_mgr mgr;
struct mg_connection *CurrentClient;
mg_event_handler_t  * ws_ev_handler(
    struct mg_connection *c, int ev, void *ev_data
){
    printf("ws_ev_handler\n");
    if (ev != MG_EV_CLOSE) {
        printf("Client::\n");
        CurrentClient = c;
    }
}

void WS_init() {
    mg_mgr_init(&mgr);
    mg_listen(&mgr, s_listen_on, ws_ev_handler, NULL);    
}


void WS2812BSimpleSend(GPIO_TypeDef * port, int pin, uint8_t * data, int len_in_bytes) {
    //send data to inspire3d_display emulator
    // using websocket
    mg_send_websocket_frame(CurrentClient, WEBSOCKET_OP_BINARY, data, len_in_bytes);
}
