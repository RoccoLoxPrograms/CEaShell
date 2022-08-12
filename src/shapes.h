#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void shapes_RoundRectangleFill(uint8_t, uint8_t, int, uint8_t, int, uint8_t);	// Draws a filled rectangle with rounded corners

void shapes_TransparentRect(uint8_t, int, uint8_t, int, uint8_t);   // Draws a semi-transparent filled rectangle with rounded corners

void shapes_RoundRectangle(bool, uint8_t, uint8_t, uint8_t, int, uint8_t, int, uint8_t);	// Draws an unfilled rectangle with rounded corners, destroys buffer

void shapes_RoundCorners(bool, uint8_t, uint8_t, int, uint8_t, int, uint8_t);	// Rounds the corners of a rectanglular area, destroys buffer

void shapes_FileIcon(uint8_t, uint8_t, int, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
