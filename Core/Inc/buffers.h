/*
 * buffers.h
 *
 *  Created on: 5 Aug 2021
 *      Author: flynn
 */

/**
 * TODO: STILL NEEDS A UNIT TEST TO CHECK IF IT WORKS AS EXPECTED
 * 
 */

#ifndef INC_BUFFERS_H_
#define INC_BUFFERS_H_

// Config
// Size of cyclic buffers
#define CYCBUF_SIZE 10
// Returns this value when the cyclic buffer is empty
#define CYCBUF_EMPTY -1

// Data structures
typedef struct cyclicBuffer{
	int * rp;
	int * wp;
	int buffer[CYCBUF_SIZE];
}cyclicBuffer_s;

// Public functions
/**
 * @brief Initilises cyclic buffer
 * 
 * @param cb 
 */
void initCycBuff(cyclicBuffer_s * cb);

/**
 * @brief Adds a int to the cyclic buffer. If the buffer is fall it throws away the new input
 * 
 * @param cb 
 * @param data 
 */
void enqueueCycBuff(cyclicBuffer_s * cb, int data);

/**
 * @brief Dequeues data from cyclic buffer, if there is no more data to dequeue then the function will return a CYCBUF_EMPTY
 * 
 * @param cb 
 * @return int 
 */
int dequeueCycBuff(cyclicBuffer_s * cb);


#endif /* INC_BUFFERS_H_ */
