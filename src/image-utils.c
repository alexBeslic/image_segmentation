/*
 * image-loader.c
 *
 *  Created on: 30.12.2022.
 *      Author: Aleksandar Beslic
 *       email: beslic.alex@gmail.com
 */

#include "image-utils.h"

/**
 * @brief Reads an .bmp format image and puts it to image variabel.
 * 
 * @param fileName File name of the image 
 * @param image Read image
 * @return int if 0 evrery thing is OK, if -1 something whent wrong
 */
int read_image(char *fileName, IMAGE *image)
{
	size_t i;
	int return_val = 0;
	short bitsPerPixel = 0;
	unsigned int dataOffset = 0u;
	unsigned int paddedRowSize = 0u;
	unsigned int unpaddedRowSize = 0u;
	unsigned int totalSize = 0u;
	float paddedRowSizeTMP;

	FILE *inputFile = fopen(fileName, "r");

	if (inputFile != NULL)
	{
		
		/* Reading Pixel Data Offset */
		fseek(inputFile, DATA_OFFSET_OFFSET, SEEK_SET);
		fread(&dataOffset, 4u, 1u, inputFile);

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

		paddedRowSizeTMP = (4.0f * (float)ceil((float)(image->width) / 4.0f));
		paddedRowSize = (unsigned int)paddedRowSizeTMP * image->bytesPerPixel;
		unpaddedRowSize = (image->width)*(image->bytesPerPixel);
		totalSize = unpaddedRowSize*(image->height);


		(image->pixels) = (unsigned char *)malloc(totalSize);
		unsigned char *currentRowPointer = image->pixels+((image->height-1)*unpaddedRowSize);

		for (i = 0u; i < image->height; ++i)
		{
			fseek(inputFile, dataOffset+(i*paddedRowSize), SEEK_SET);
			fread(currentRowPointer, 1u, unpaddedRowSize, inputFile);
			currentRowPointer -= unpaddedRowSize;
		}

		fclose(inputFile);
	} else
	{
		/* Something went wrong */
		return_val = -1;
	}
	
	return 0;
}

/**
 * @brief Writes the image in a .bmp format
 * 
 * @param fileName File name of the image 
 * @param image to be written
 * @return int if 0 evrery thing is OK, if -1 something whent wrong
 */
int write_image(char *fileName, IMAGE *image)
{
	size_t i;
	int return_val = 0;
	/* File info */
	const char BM[] = "BM";
	float paddedRowSizeTMP = (4.0f * (float)ceil((float) image->width/4.0f));
	unsigned int paddedRowSize = (unsigned int) paddedRowSizeTMP * image->bytesPerPixel;
	unsigned int fileSize = paddedRowSize*image->height + HEADER_SIZE + INFO_HEADER_SIZE;
	unsigned int reserved = 0x0000u;
	unsigned int dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;

	/* Header info */
	unsigned int infoHeaderSize = INFO_HEADER_SIZE;
	short planes = 1; /* always 1 */
	unsigned int bitsPerPixel = image->bytesPerPixel * 8u;

	unsigned int compression = NO_COMPRESION;

	/* Image properties */
	unsigned int imageSize = image->width * image->height * image->bytesPerPixel;
	unsigned int resolutionX = 11811u; /* 300 dpi */
	unsigned int resolutionY = 11811u; /* 300 dpi */
	unsigned int colorsUsed = MAX_NUMBER_OF_COLORS;
	unsigned int importantColors = ALL_COLORS_REQUIRED;

	unsigned int unpaddedRowSize = image->width * image->bytesPerPixel;

	FILE *outputFile = fopen(fileName, "w");

	if (outputFile != NULL) {
		/* Write file info */
		fwrite(&BM[0], 1u, 1u, outputFile);
		fwrite(&BM[1], 1u, 1u, outputFile);
		fwrite(&fileSize, 4u, 1u, outputFile);
		fwrite(&reserved, 4u, 1u, outputFile);
		fwrite(&dataOffset, 4u, 1u, outputFile);

		/* Write header info */
		fwrite(&infoHeaderSize, 4u, 1u, outputFile);
		fwrite(&image->width, 4u, 1u, outputFile);
		fwrite(&image->height, 4u, 1u, outputFile);
		fwrite(&planes, 2u, 1u, outputFile);
		fwrite(&bitsPerPixel, 2u, 1u, outputFile);

		/* Write compression */
		fwrite(&compression, 4u, 1u, outputFile);

		/* Write image size (in bytes) */
		fwrite(&imageSize, 4u, 1u, outputFile);
		fwrite(&resolutionX, 4u, 1u, outputFile);
		fwrite(&resolutionY, 4u, 1u, outputFile);
		fwrite(&colorsUsed, 4u, 1u, outputFile);
		fwrite(&importantColors, 4u, 1u, outputFile);

		/* Write pixels to file */
		for (i = 0u; i < image->height; ++i)
		{
				unsigned int pixelOffset = ((image->height - i) - 1u)*unpaddedRowSize;
				fwrite(&image->pixels[pixelOffset], 1u, paddedRowSize, outputFile);
		}
	
		fclose(outputFile);
	}
	else { 
		/* Something went wrong*/
		return_val = -1;
	}

	return return_val;
}

/**
 * @brief Converts the image to gray scale using Luminosity Method
 * 
 * @param originalImage input image
 * @param grayscaleImage output image (pixel array need to be preallocated)
 */
void convert_to_grayscale(IMAGE *originalImage, IMAGE *grayscaleImage)
{
	size_t i = 0u;
	unsigned int imageSize =  originalImage->height * originalImage->width;
	unsigned int onePixel = originalImage->bytesPerPixel;

	grayscaleImage->height = originalImage->height;
	grayscaleImage->width = originalImage->width;
	grayscaleImage->bytesPerPixel = 1u;

	for (i = 0u; i < imageSize; ++i)
	{
		/* Grayscale conversiony 0.11*B + 0.59*G + 0.3*R */
		grayscaleImage->pixels[i] = (unsigned int) (0.11f * originalImage->pixels[i*onePixel] + 0.59f * originalImage->pixels[i*onePixel + 1] + 0.3f * originalImage->pixels[i*onePixel + 2]);
	}

}

/**
 * @brief Used for edge detection using 2D convolution method
 * 
 * @param input Image
 * @param output Image (pixel array need to be preallocated)
 * @param kernel array
 * @param k_row number of rows in the kernel
 * @param k_col number of columns in the kernel
 */
void edge_detection(IMAGE *input, IMAGE *output, char *kernel, unsigned int k_row, unsigned int k_col)
{
	size_t i, j, n, m;
	unsigned int mm, nn, ii, jj;
	unsigned int column = input->width;
	unsigned int row = input->height;
	unsigned int centerX = k_col / 2u;
	unsigned int centerY = k_row / 2u;

	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 1u;

	for(i = 0u; i < row; ++i)
	{
		for(j = 0u; j < column; ++j)
		{
			for(m = 0u; m < k_row; ++m)
			{
				mm = k_row - 1u - m;

				for(n = 0u; n < k_col; ++n)
				{
					nn = k_col - 1u - n;  /* column index of flipped kernel */

					/* index of input signal, used for checking boundary */
					ii = i + (centerY - mm);
					jj = j + (centerX - nn);

					/* ignore input samples which are out of bound */
					if((ii < row) && (jj < column))
						output->pixels[i*column + j] += input->pixels[ii * column + jj] * kernel[mm * k_col + nn];
				}
			}
		}
	}
}

/**
 * @brief Colors one pixel by with color detriment by color index
 * 
 * @param pixels to be colored 
 * @param color_index in range of NUM_COLORS
 */
void color_pixel(unsigned char *pixels, unsigned int color_index)
{
	unsigned int colors[] = {COLOR_1, COLOR_2, COLOR_3, COLOR_4,
							COLOR_5, COLOR_6, COLOR_7, COLOR_8,
							COLOR_9, COLOR_10};

	 /* Setting RGB values */
	pixels[2] = C_MASK_R(colors[color_index]);
	pixels[1] = C_MASK_G(colors[color_index]);
	pixels[0] = C_MASK_B(colors[color_index]);
}

/**
 * @brief Color an image based on the code in that pixel
 * 
 * @param input image that has been already codet 
 * @param output image
 */
void color_segments(IMAGE *input, IMAGE *output)
{
	size_t i;
	unsigned int size = input->height * input->width;
	unsigned int color_index = 0u;
	unsigned int onePixel = output->bytesPerPixel;

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 3u;

	/* Colors each pixel*/
	for (i = 0u; i < size; i++)
	{
		color_index = input->pixels[i];

		if (color_index < NUM_COLORS)
		{
			color_pixel(&output->pixels[i * onePixel], color_index - 1u);
		}
	}
}

/**
 * @brief Codes an image, evrey closed contour gets its own code
 * 
 * @param image input
 */
void code_segments(IMAGE *image) {
	size_t i, j;
    unsigned int width = image->width;
    unsigned int height = image->height;
    unsigned int next_code = 1u;
    unsigned int *codes = calloc(width * height, sizeof(unsigned int));

	/* Check if the pixel is empty and if that pixel isn't already color  then calls flood fill function */
    for (i = 0u; i < height; i++) {
        for (j = 0u; j < width; j++) {
            if ((image->pixels[i * width + j] == 0u) && (codes[i * width + j] == 0u)) {
                flood_fill(image, j, i, next_code, codes);
                next_code = (next_code + 1u) % NUM_COLORS;
            }
        }
    }

    /* Assign the color codes to the image */
    for (i = 0u; i < width*height; i++) {
        image->pixels[i] = codes[i];
    }
    free(codes);
}

/**
 * @brief Uses BFS algoritam to give each pixel in a closed contour unique color code
 * 
 * @param image input
 * @param x position in image
 * @param y position in image
 * @param color current color code
 * @param codes array of previous codet pixels
 */
void flood_fill(IMAGE *image, unsigned int x, unsigned int y, unsigned int color, unsigned int *codes) {
    int i, j, xx, yy;
	int width = image->width;
    int height = image->height;
    unsigned char *pixels = image->pixels;
    int front = 0, rear = 0;
    int queue[width*height][2];
    queue[rear][0] = x;
    queue[rear][1] = y;
    rear++;
    codes[y * width + x] = color;

    while (front != rear) {
        int current_x = queue[front][0];
        int current_y = queue[front][1];
        front++;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                xx = current_x + i;
                yy = current_y + j;
                if (((xx < 0) || (xx >= width)) || ((yy < 0) || (yy >= height))) {
                    continue;
                }
                if ((pixels[yy * width + xx] == 0u) && (codes[yy * width + xx] == 0u)) {
                    codes[yy * width + xx] = color;
                    queue[rear][0] = xx;
                    queue[rear][1] = yy;
                    rear++;
                }
            }
        }
    }
}

/**
 * @brief Reads an .bmp format image and puts it to image varibel.
 * Pixel array needs to be preallocated.
 * 
 * @param fileName File name of the image 
 * @param image Read image
 * @return int if 0 evrery thing is OK, if -1 something whent wrong
 */
int read_image_dsp(char *fileName, IMAGE *image)
{
	size_t i;
	int return_val = 0;
	short bitsPerPixel = 0;
	unsigned int dataOffset = 0u;
	unsigned int paddedRowSize = 0u;
	unsigned int unpaddedRowSize = 0u;
	unsigned int totalSize = 0u;
	float paddedRowSizeTMP;

	FILE *inputFile = fopen(fileName, "r");

	if (inputFile != NULL)
	{

		/* Reading Pixel Data Offset */
		fseek(inputFile, DATA_OFFSET_OFFSET, SEEK_SET);
		fread(&dataOffset, 4u, 1u, inputFile);

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

		paddedRowSizeTMP = (4.0f * (float)ceil((float)(image->width) / 4.0f));
		paddedRowSize = (unsigned int)paddedRowSizeTMP * image->bytesPerPixel;
		unpaddedRowSize = (image->width)*(image->bytesPerPixel);
		totalSize = unpaddedRowSize*(image->height);


		(image->pixels) = (unsigned char *)malloc(totalSize);
		unsigned char *currentRowPointer = image->pixels+((image->height-1)*unpaddedRowSize);

		for (i = 0u; i < image->height; ++i)
		{
			fseek(inputFile, dataOffset+(i*paddedRowSize), SEEK_SET);
			fread(currentRowPointer, 1u, unpaddedRowSize, inputFile);
			currentRowPointer -= unpaddedRowSize;
		}

		fclose(inputFile);
	} else
	{
		/* Something went wrong */
		return_val = -1;
	}

	return 0;
}


int write_image_dsp(char *fileName, IMAGE *image)
{
	size_t i;
	/* File info */
	const char BM[] = "BM";
	unsigned int paddedRowSize = (unsigned int)(4 * ceil((float) image->width/4.0f))*image->bytesPerPixel;
	unsigned int fileSize = paddedRowSize*image->height + HEADER_SIZE + INFO_HEADER_SIZE;
	unsigned int reserved = 0x0000u;
	unsigned int dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;

	/* Header info */
	unsigned int infoHeaderSize = INFO_HEADER_SIZE;
	short planes = 1; /* always 1 */
	unsigned int bitsPerPixel = image->bytesPerPixel * 8u;

	unsigned int compression = NO_COMPRESION;

	/* Image properties */
	unsigned int imageSize = image->width * image->height * image->bytesPerPixel;
	unsigned int resolutionX = 11811u; /* 300 dpi */
	unsigned int resolutionY = 11811u; /* 300 dpi */
	unsigned int colorsUsed = MAX_NUMBER_OF_COLORS;
	unsigned int importantColors = ALL_COLORS_REQUIRED;

	unsigned int unpaddedRowSize = image->width * image->bytesPerPixel;

	FILE *outputFile = fopen(fileName, "w");

	if (outputFile == NULL) {
		return -1;
	}

	/* Write file info */
	fwrite(&BM[0], 1u, 1u, outputFile);
	fwrite(&BM[1], 1u, 1u, outputFile);
	fwrite(&fileSize, 4u, 1u, outputFile);
	fwrite(&reserved, 4u, 1u, outputFile);
	fwrite(&dataOffset, 4u, 1u, outputFile);

	/* Write header info */
	fwrite(&infoHeaderSize, 4u, 1u, outputFile);
	fwrite(&image->width, 4u, 1u, outputFile);
	fwrite(&image->height, 4u, 1u, outputFile);
	fwrite(&planes, 2u, 1u, outputFile);
	fwrite(&bitsPerPixel, 2u, 1u, outputFile);

	/* Write compression */
	fwrite(&compression, 4u, 1u, outputFile);

	/* Write image size (in bytes) */
	fwrite(&imageSize, 4u, 1u, outputFile);
	fwrite(&resolutionX, 4u, 1u, outputFile);
	fwrite(&resolutionY, 4u, 1u, outputFile);
	fwrite(&colorsUsed, 4u, 1u, outputFile);
	fwrite(&importantColors, 4u, 1u, outputFile);

	/* Write pixels to file */
	for (i = 0u; i < image->height; ++i)
	{
			unsigned int pixelOffset = ((image->height - i) - 1u)*unpaddedRowSize;
			fwrite(&image->pixels[pixelOffset], 1u, paddedRowSize, outputFile);
	}

	fclose(outputFile);

	return 0;
}

void convert_to_grayscale_dsp(IMAGE *originalImage, IMAGE *grayscaleImage)
{
	size_t i = 0u;
	unsigned int imageSize =  originalImage->height * originalImage->width;
	unsigned int onePixel = originalImage->bytesPerPixel;
	unsigned char sumRGB;

	grayscaleImage->height = originalImage->height;
	grayscaleImage->width = originalImage->width;
	grayscaleImage->bytesPerPixel = 1u;

	#pragma SIMD_for
	for (i = 0u; i < imageSize; ++i*onePixel)
	{
		sumRGB = originalImage->pixels[i];
		sumRGB += originalImage->pixels[i + 1u];
		sumRGB += originalImage->pixels[i + 2u];
		grayscaleImage->pixels[i] = sumRGB / 3u;
	}

}


void edge_detection_dsp(IMAGE *input, IMAGE *output, char *kernel, unsigned int k_row, unsigned int k_col)
{
	size_t i, j, n, m;
	unsigned int mm, nn, ii, jj;
	unsigned int column = input->width;
	unsigned int row = input->height;
	unsigned int centerX = k_col / 2u;
	unsigned int centerY = k_row / 2u;

	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 1u;

	#pragma SIMD_for
	for(m = 0u; m < k_row; ++m)
	{
		mm = k_row - 1u - m; /* row index of flipped kernel */
		for(n = 0u; n < k_col; ++n)
		{
			nn = k_col - 1u - n;  /* column index of flipped kernel */
			for(i = 0u; i < row; ++i)
			{
				for(j = 0u; j < column; ++j)
				{
					/* index of input signal, used for checking boundary */
					ii = i + (centerY - mm);
					jj = j + (centerX - nn);

					/* ignore input samples which are out of bound */
					if((ii < row) && (jj < column))
						output->pixels[i*column + j] += input->pixels[ii * column + jj] * kernel[mm * k_col + nn];
				}
			}
		}
	}
}

void color_segments_dsp(IMAGE *input, IMAGE *output)
{
	unsigned int colors[] = {COLOR_1, COLOR_2, COLOR_3, COLOR_4,
								COLOR_5, COLOR_6, COLOR_7, COLOR_8,
								COLOR_9, COLOR_10};
	size_t i;
	unsigned int size = input->height * input->width;
	unsigned int color_index = 0u;
	unsigned int onePixel = output->bytesPerPixel;

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 3u;

	/* Coloring each pixel */
	#pragma SIMD_for
	for (i = 0u; i < size; i++)
	{
		color_index = input->pixels[i];

		if (expected_true(color_index < NUM_COLORS))
		{

			 /* Setting RGB values */
			output->pixels[i* onePixel + 2] = C_MASK_R(colors[color_index]);
			output->pixels[i* onePixel + 1] = C_MASK_G(colors[color_index]);
			output->pixels[i* onePixel + 0] = C_MASK_B(colors[color_index]);
		}
	}
}


void code_segments_dsp(IMAGE *image) {
	size_t i, j;
	unsigned int width = image->width;
	unsigned int height = image->height;
	unsigned int next_code = 1u;
	unsigned int *codes = calloc(width * height, sizeof(unsigned int));

	/* Check if the pixel is empty and if that pixel isn't already color  then calls flood fill function */
	for (i = 0u; i < height; i++) {
		for (j = 0u; j < width; j++) {
			if ((image->pixels[i * width + j] == 0u) && (codes[i * width + j] == 0u)) {
				flood_fill(image, j, i, next_code, codes);
				next_code = (next_code + 1u) % NUM_COLORS;
			}
		}
	}

	/* Assign the color codes to the image */
	for (i = 0u; i < width*height; i++) {
		image->pixels[i] = codes[i];
	}
	free(codes);
}

void flood_fill_dsp(IMAGE *image, unsigned int x,unsigned int y,unsigned int color, unsigned int *codes) {
    int i, j, xx, yy;
	unsigned int width = image->width;
    unsigned int height = image->height;
    unsigned char *pixels = image->pixels;
    int front = 0, rear = 0;
    int queue[width*height][2];
    queue[rear][0] = x;
    queue[rear][1] = y;
    rear++;
    codes[y * width + x] = color;

    while (front != rear) {
        int currentX = queue[front][0];
        int currentY = queue[front][1];
        front++;
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                xx = currentX + i;
                yy = currentY + j;
                if (((xx < 0) || (xx >= width)) || ((yy < 0) || (yy >= height))) {
                    continue;
                }
                if ((pixels[yy * width + xx] == 0u) && (codes[yy * width + xx] == 0u)) {
                    codes[yy * width + xx] = color;
                    queue[rear][0] = xx;
                    queue[rear][1] = yy;
                    rear++;
                }
            }
        }
    }
}
