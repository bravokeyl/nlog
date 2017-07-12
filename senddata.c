/*
 * send data to the server
 *
 *  Created on: 16 Feb 2017
 *       Latest
 */

#include "ch.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/opt.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "stdlib.h"
#include "string.h"

#include "senddata.h"
/**
 * Stack area for the senddata client thread.
 */
WORKING_AREA(wa_sendata_client, SENDDATA_THREAD_STACK_SIZE);

Semaphore sendingbufsem;

static char sendbuf[SENDDATA_BUF_NUM][SENDDATA_BUF_SIZE];
static int32_t bufsize[SENDDATA_BUF_NUM] = { 0 };

static int32_t storeidx = 0; //
static int32_t sendidx = -1; //sending index

void senddata(int size, char *data) {
	if (storeidx == sendidx) {
		return; //old data is still under sending
	}

	if (chSemGetCounterI(&sendingbufsem) == SENDDATA_BUF_NUM)
		return;

	if (bufsize[storeidx] + size > SENDDATA_BUF_SIZE) {
		//buffer is full, move to next buf
		storeidx = (storeidx + 1) % SENDDATA_BUF_NUM;

		chSemSignal(&sendingbufsem); // one buffer available
		if (storeidx == sendidx) {
			return; // do not store data
		}
		bufsize[storeidx] = 0;
	}
	// copy data
	memcpy(&sendbuf[storeidx][bufsize[storeidx]], data, size);
	bufsize[storeidx] += size;
}

/**
 * senddata client thread.
 */
msg_t senddata_client(void *p) {
	int s = 0;
	int ret = 0;
	struct sockaddr_in addr;

	LWIP_UNUSED_ARG(p);
	/* set up address to connect to */
	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = PP_HTONS(SOCK_SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(SOCK_SERVER_HOST);

	do {
		//wait one buffer is full
		chSemWait(&sendingbufsem);
		sendidx = (sendidx + 1) % SENDDATA_BUF_NUM;
		/* create the socket */
		s = lwip_socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0)
			continue;

		ret = lwip_connect(s, (struct sockaddr*) &addr, sizeof(addr));
		if (ret == 0)
			lwip_write(s, &(sendbuf[sendidx][0]), bufsize[sendidx]); /* write data to the network */

		/* close */
		ret = lwip_close(s);

		bufsize[sendidx] = 0;

	} while (1);

	chThdExit(ret);

}
