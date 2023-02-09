/*
 * image-loader.h
 *
 *  Created on: 30.12.2022.
 *      Author: Aleksandar Beslic
 *       email: beslic.alex@gmail.com
 */

#ifndef IMAGE_UTILS_H_
#define IMAGE_UTILS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <builtins.h>

/* For use with BMP format images */
#define DATA_OFFSET_OFFSET 0x000AL
#define WIDTH_OFFSET 0x0012L
#define HEIGHT_OFFSET 0x0016L
#define BITS_PER_PIXEL_OFFSET 0x001CL
#define HEADER_SIZE 14u
#define INFO_HEADER_SIZE 40u
#define NO_COMPRESION 0u
#define MAX_NUMBER_OF_COLORS 0u
#define ALL_COLORS_REQUIRED 0u

/* Color definition */
#define NUM_COLORS 11u
#define COLOR_1 0x3AAFA9u
#define COLOR_2 0xDEE2E1u
#define COLOR_3 0xFFE33Au
#define COLOR_4 0xDA7B93u
#define COLOR_5 0x2F4454u
#define COLOR_6 0xFFCC77u
#define COLOR_7 0xFF2255u
#define COLOR_8 0xD0F2C7u
#define COLOR_9 0x60F2F7u
#define COLOR_10 0x5F236Bu

/* Macro function definition for extracting individual RGB values */
#define C_MASK_R(COLOR) (((COLOR) >> 16u) & 0xffu)
#define C_MASK_G(COLOR) (((COLOR) >> 8u) & 0xffu)
#define C_MASK_B(COLOR) ((COLOR) & 0xffu)


/* Image strutter definition */
typedef struct img
{
	unsigned int width;
	unsigned int height;
	unsigned int bytesPerPixel;
	unsigned char *pixels;
} IMAGE;

int read_image(char *fileName, IMAGE *image);
int write_image(char *fileName, IMAGE *image);
void prep_for_writing(IMAGE *input, IMAGE *output);
unsigned int get_bytes_to_read(unsigned char array[4]);
void get_bytes_to_write(unsigned char array[4], unsigned int whatTowrite);

void convert_to_grayscale(IMAGE *originalImage, IMAGE *grayscaleImage);
void edge_detection(IMAGE *input, IMAGE *output, char *kernel, unsigned int k_row, unsigned int k_col);
void flood_fill(IMAGE *image, unsigned int x, unsigned int y, unsigned int color, unsigned char *codes, int *queue[2]);
void color_segments(IMAGE *input, IMAGE *output);
void code_segments(IMAGE *image, unsigned char *codes, int *queue[2]);
void color_pixel(unsigned char *pixels, unsigned int color_index);


void convert_to_grayscale_dsp(IMAGE *originalImage, IMAGE *grayscaleImage);
void edge_detection_dsp(IMAGE *input, IMAGE *output, char * restrict kernel, unsigned int k_row, unsigned int k_col);
void flood_fill_dsp(IMAGE *image, unsigned int x,unsigned int y,unsigned int color, unsigned char * restrict codes, int *queue[2]);
void color_segments_dsp(IMAGE *input, IMAGE *output);
void code_segments_dsp(IMAGE *image, unsigned char * restrict codes, int *queue[2]);

#endif /* IMAGE_UTILS_H_ */
