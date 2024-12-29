/*
 * ringBuffer.h
 *
 *  Created on: Dec 25, 2024
 *      Author: burakguzeller
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#include "main.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RING_MAX_SIZE 64

typedef struct {
	uint16_t tail;
	uint16_t head;
	uint8_t buffer[RING_MAX_SIZE];
}Ring_Buffer_t;

void resetBuffer(Ring_Buffer_t *pRingBuffer);
bool bufferIsEmpty(Ring_Buffer_t *pRingBuffer);
bool bufferIsFull(Ring_Buffer_t *pRingBuffer);
void writeBufferData(Ring_Buffer_t *pRingBuffer, uint8_t data);
uint8_t readBufferData(Ring_Buffer_t *pRingBuffer);
bool checkDataInBuffer(Ring_Buffer_t *pRingBuffer, const char *str);
bool findDataInBuffer(Ring_Buffer_t *pRingBuffer, const char *str);
void copyBufferData(Ring_Buffer_t *pRingBuffer, char copyField[], uint8_t fieldSize, const char *copy);
void copyAfterPattern(Ring_Buffer_t *pRingBuffer, char copyField[], const char *copyStart, char copyFinish, uint16_t len);
void copyCircularBuffer(Ring_Buffer_t *pRingBuffer, uint8_t *dest, uint16_t count);

#endif /* INC_RINGBUFFER_H_ */
