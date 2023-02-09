/*
 * image-loader.c
 *
 *  Created on: 30.12.2022.
 *      Author: Aleksandar Beslic
 *       email: beslic.alex@gmail.com
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 //#include "slika.h"

#define DATA_OFFSET_OFFSET 0x000AL
#define WIDTH_OFFSET 0x0012L
#define HEIGHT_OFFSET 0x0016L
#define BITS_PER_PIXEL_OFFSET 0x001CL
#define HEADER_SIZE 14u
#define INFO_HEADER_SIZE 40u
#define NO_COMPRESION 0u
#define MAX_NUMBER_OF_COLORS 0u
#define ALL_COLORS_REQUIRED 0u

typedef struct img
{
	unsigned int width;
	unsigned int height;
	unsigned int bytesPerPixel;
	unsigned char *pixels;
} IMAGE;


/**
 * @brief Reads an .bmp format image and puts it to image variable.
 * 
 * @param fileName File name of the image 
 * @param image Read image (pixel array need to be preallocated)
 * @return int if 0 every thing is OK, if -1 something went wrong
 */
int read_image(char *fileName, IMAGE *image)
{
	short bitsPerPixel;
	unsigned int dataOffset;
	int paddedRowSize;

	FILE *inputFile = fopen(fileName, "r");

	if (inputFile == NULL)
	{
		return -1;
	}

	/* Reading Pixel Data Offset */
	fseek(inputFile, DATA_OFFSET_OFFSET, SEEK_SET);
	fread(&dataOffset, 4, 1, inputFile);

	/* Reading Image Width */
	fseek(inputFile, WIDTH_OFFSET, SEEK_SET);
	fread(&image->width, 4u, 1u, inputFile);

	/* Reading Image Height */
	fseek(inputFile, HEIGHT_OFFSET, SEEK_SET);
	fread(&image->height, 4u, 1u, inputFile);

	/* Reading The number of bits per pixel */
	fseek(inputFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
	fread(&bitsPerPixel, 2u, 1u, inputFile);
	image->bytesPerPixel = ((unsigned int)bitsPerPixel) / 8u;

	paddedRowSize = (int)(4 * ceil((float)(image->width) / 4.0f))*(image->bytesPerPixel);
	int unpaddedRowSize = (image->width)*(image->bytesPerPixel);
	int totalSize = unpaddedRowSize*(image->height);

	(image->pixels) = (unsigned char *)malloc(totalSize); // TODO: malo bolje
	unsigned char *currentRowPointer = image->pixels+((image->height-1)*unpaddedRowSize);

	for (int i = 0; i < image->height; ++i)
	{
		fseek(inputFile, dataOffset+(i*paddedRowSize), SEEK_SET);
		fread(currentRowPointer, 1, unpaddedRowSize, inputFile);
		currentRowPointer -= unpaddedRowSize;
	}

	fclose(inputFile);

	return 0;
}

/**
 * @brief Writes the image to .h file
 * 
 * @param fileName File name of the image 
 * @param image to be written
 * @return int if 0 evrery thing is OK, if -1 something whent wrong
 */
int write_image(char *fileName, IMAGE *image)
{
	size_t i;
	unsigned int size = image->height * image->bytesPerPixel * image->width;
	
	FILE *file = fopen(fileName, "w");

	if (file == NULL)
	{
		return -1;
	}
	

	fprintf(file, "#define FAST_READ %uu\n\n", 1u);
	fprintf(file, "#define IMAGE_WIDTH %uu\n", image->width);
	fprintf(file, "#define IMAGE_HEIGTH %uu\n", image->height);
	fprintf(file, "#define IMAGE_B_PIXEL %uu\n", image->bytesPerPixel);
	fprintf(file, "unsigned char slika[] = {");


	for (i = 0; i < size; i++)
	{
		fprintf(file, "0x%x,", image->pixels[i]);
	}

	fprintf(file, "\n};\n\n");

	fclose(file);

	return 0;
}


int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("You need to provide the image name\n");
		return 1;
	}
	

	IMAGE image;

	read_image(argv[1], &image);

	write_image("slika.h", &image);

	free(image.pixels);

	return 0;
}

