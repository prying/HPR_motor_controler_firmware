/*
 * buffers.c
 *
 *  Created on: 5 Aug 2021
 *      Author: flynn
 */

#include "buffers.h"
#include <string.h>

// Sets up cyc buff
void initCycBuff(cyclicBuffer_s * cb)
{
	// Initilise read and write
	cb->rp = cb->buffer;
	cb->wp = cb->buffer;

	// Initilise with the empty value
	memset(cb->buffer, CYCBUF_EMPTY, CYCBUF_SIZE);
}

void enqueueCycBuff(cyclicBuffer_s * cb, int data)
{
	int *nwp;

	// Save data
	*(cb->wp) = data;

	// Move the write pointer
	nwp = cb->wp + 1 >= cb->buffer + CYCBUF_SIZE ? cb->buffer : cb->wp + 1;
	// Collision detect
	cb->wp = cb->rp == nwp ? cb->wp : nwp;
}

int dequeueCycBuff(cyclicBuffer_s * cb)
{
	int data;
	int *nrp;

	// Get data and if its a empty value leave and dont move read pointer
	data = *(cb->rp);
	if (data == CYCBUF_EMPTY)
		return data;
	*(cb->rp) = CYCBUF_EMPTY;

	// Move the read pointer
	nrp = cb->rp + 1 >= cb->buffer + CYCBUF_SIZE ? cb->buffer : cb->rp + 1;
	// Collision detect
	cb->rp = cb->wp == nrp ? cb->rp : nrp;

	return data;
}
