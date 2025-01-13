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

	/**
	 * Mimicks the same frame type received and re-send it again
	 *
	 * Please note that we could just use a ws_sendframe_txt()
	 * or ws_sendframe_bin() here, but we're just being safe
	 * and re-sending the very same frame type and content
	 * again.
	 *
	 * Alternative functions:
	 *   ws_sendframe()
	 *   ws_sendframe_txt()
	 *   ws_sendframe_txt_bcast()
	 *   ws_sendframe_bin()
	 *   ws_sendframe_bin_bcast()
	 */
	ws_sendframe_bcast(8080, (char *)msg, size, type);
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
    char * fullCommand = malloc(strlen(command) + strlen(path) + 1);
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
