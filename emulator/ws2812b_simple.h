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
		if (msg[0] == 'W') {
			set_adc_arrow(ARROW_UP_L);
		} else if (msg[0] == 'S') {
			set_adc_arrow(ARROW_DOWN_L);
		} else if (msg[0] == 'A') {
			set_adc_arrow(ARROW_LEFT_L);
		} else if (msg[0] == 'D') {
			set_adc_arrow(ARROW_RIGHT_L);
		} 

		if (msg[0] == 'I') {
			set_adc_abcd(ABCD_A_L);
		} else if (msg[0] == 'K') {
			set_adc_abcd(ABCD_B_L);
		} else if (msg[0] == 'J') {
			set_adc_abcd(ABCD_C_L);
		} else if (msg[0] == 'L') {
			set_adc_abcd(ABCD_D_L);
		}
	}else if(size == 1 && msg[0] == 'U'){
		if(
			msg[0] == 'W' ||
			msg[0] == 'S' ||
			msg[0] == 'A' ||
			msg[0] == 'D' 
		){
			set_adc_arrow(0);
		}
		if(
			msg[0] == 'I' ||
			msg[0] == 'J' ||
			msg[0] == 'K' ||
			msg[0] == 'L' 
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
