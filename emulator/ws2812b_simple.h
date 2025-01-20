#pragma once
#include "buttons.h"
#include "ch32v003fun.h"

// websocket server to connect to inspire3d_display emulator
#include "ws.h"
#include <stdio.h>
#include <string.h>
#define WebServerPort 8000
#define indexHTMLPath "../emulator/inspire3d_emu/index.html"


#define BORDER_X 2
//dumy
#define NUM_LEDS 0
#define horizontalButtons 0
#define verticalButtons 0

#ifdef WAIT_WS_CONNECT
bool WS_CONNECT = false;
#endif

void openHTML();
// For driver to invoke keys
static uint16_t adc_arrow = 0;

void set_adc_arrow(uint16_t adc){
    adc_arrow = adc;
}


static uint16_t adc_abcd = 0;

void set_adc_abcd(uint16_t adc){
    adc_abcd = adc;
}



void onopen(ws_cli_conn_t client)
{
	char *cli, *port;
	cli  = ws_getaddress(client);
	port = ws_getport(client);
#ifndef DISABLE_VERBOSE
	printf("Connection opened, addr: %s, port: %s\n", cli, port);
#endif
#ifdef WAIT_WS_CONNECT
	WS_CONNECT = true;
#endif
}

void onmessage(ws_cli_conn_t client,
	const unsigned char *msg, uint64_t size, int type)
{
	char *cli;
	cli = ws_getaddress(client);
#ifndef DISABLE_VERBOSE
	printf("I receive a message: %s (size: %" PRId64 ", type: %d), from: %s\n",
		msg, size, type, cli);
#endif
	//set buttons values
	if (size == 2 && msg[1] == 'D') {
		if (msg[0] == 'w') {
			set_adc_arrow(ARROW_UP_L);
		} else if (msg[0] == 's') {
			set_adc_arrow(ARROW_DOWN_L);
		} else if (msg[0] == 'a') {
			set_adc_arrow(ARROW_LEFT_L);
		} else if (msg[0] == 'd') {
			set_adc_arrow(ARROW_RIGHT_L);
		} 

		if (msg[0] == 'i') {
			set_adc_abcd(ABCD_A_L);
		} else if (msg[0] == 'k') {
			set_adc_abcd(ABCD_B_L);
		} else if (msg[0] == 'j') {
			set_adc_abcd(ABCD_C_L);
		} else if (msg[0] == 'l') {
			set_adc_abcd(ABCD_D_L);
		}
	}else if(size == 2 && msg[1] == 'U'){
		if(
			msg[0] == 'w' ||
			msg[0] == 's' ||
			msg[0] == 'a' ||
			msg[0] == 'd' 
		){
			set_adc_arrow(0);
		}
		if(
			msg[0] == 'i' ||
			msg[0] == 'j' ||
			msg[0] == 'k' ||
			msg[0] == 'l' 
		){
			set_adc_abcd(0);
		}
	}
	
}

void onclose(ws_cli_conn_t client)
{
	char *cli;
	cli = ws_getaddress(client);
#ifndef DISABLE_VERBOSE
	printf("Connection closed, addr: %s\n", cli);
#endif
}

void WS_init() {
	ws_socket(&(struct ws_server){
		.host = "127.0.0.1",
		.port = WebServerPort,
		.thread_loop   = 1,
		.timeout_ms    = 1000,
		.evs.onopen    = &onopen,
		.evs.onclose   = &onclose,
		.evs.onmessage = &onmessage
	});
    openHTML();
#ifdef WAIT_WS_CONNECT
	while(!WS_CONNECT);
#endif
}


void openHTML(){
    //for windows
#ifdef _WIN32
    char * command = "start ";
    char * path = indexHTMLPath;
    char fullCommand[strlen(command) + strlen(path) + 1];
    strcpy(fullCommand, command);
    strcat(fullCommand, path);
#endif
    // for linux
#ifdef __linux__
    char * command = "xdg-open ";
    char * path = indexHTMLPath;
    char * fullCommand = malloc(strlen(command) + strlen(path) + 1);
    strcpy(fullCommand, command);
    strcat(fullCommand, path);
#endif
#ifdef __APPLE__
    char * fullCommand = "open ";
    char * path = indexHTMLPath;
    char * fullCommand = malloc(strlen(command) + strlen(path) + 1);
    strcpy(fullCommand, command);
    strcat(fullCommand, path);
#endif
    system(fullCommand);

}


void WS2812BSimpleSend(GPIO_TypeDef * port, int pin, uint8_t * data, int len_in_bytes) {
    //send data to inspire3d_display emulator
    // using websocket
    ws_sendframe_bin_bcast(WebServerPort, (char *)data, len_in_bytes);
}
