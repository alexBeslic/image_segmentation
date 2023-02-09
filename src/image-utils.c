/**
 * @file image-utils.c
 * @author Aleksandar Beslic (beslic.alex@gmail.com)
 * @brief Image utility functions
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "image-utils.h"


#ifdef _MISRA_2004_RULES
#pragma diag(push)

#pragma diag(suppress:misra_rule_17_4: \
"Array indexing shall be the only allowed form of pointer arithmetic")
#pragma diag(suppress:misra_rule_6_1: \
"The plain char type shall be used only for the storage and use of character values")
#pragma diag(suppress:misra_rule_6_2: \
"The plain char type shall be used only for the storage and use of character values")
#pragma diag(suppress:misra_rule_10_1_a: \
"Implicitly converted to a different underlying type")
#pragma diag(suppress:misra_rule_10_4: \
"Implicitly converted to a different underlying type")

#endif /* _MISRA_2004_RULES */

/**
 * @brief Reads an .bmp format image and puts it to image variable.
 * 
 * @param fileName File name of the image 
 * @param image Read image (pixel array need to be preallocated)
 * @return int if 0 every thing is OK, if -1 something went wrong
 */
int read_image(char *fileName, IMAGE *image)
{
	size_t i;
	int return_val = 0;
	unsigned short bitsPerPixel = 0u;
	unsigned int dataOffset = 0u;
	unsigned int paddedRowSize = 0u;
	unsigned int unpaddedRowSize = 0u;
	float paddedRowSizeTMP;
	unsigned char *currentRowPointer;
	unsigned char reader[4] = {0u};

	FILE *inputFile = fopen(fileName, "r");

	if (inputFile != NULL)
	{
		/* Reading Pixel Data Offset */
		fseek(inputFile, DATA_OFFSET_OFFSET, SEEK_SET);
		fread(reader, 1u, 4u, inputFile);
		dataOffset = get_bytes_to_read(reader);

		/* Reading Image Width */
		fseek(inputFile, WIDTH_OFFSET, SEEK_SET);
		fread(reader, 1u, 4u, inputFile);
		image->width = get_bytes_to_read(reader);

		/* Reading Image Height */
		fseek(inputFile, HEIGHT_OFFSET, SEEK_SET);
		fread(reader, 1u, 4u, inputFile);
		image->height = get_bytes_to_read(reader);

		/* Reading The number of bits per pixel */
		fseek(inputFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
		fread(reader, 1u, 2u, inputFile);
		bitsPerPixel = (reader[1] << 8u) | reader[0];

		image->bytesPerPixel = ((unsigned int)bitsPerPixel) / 8u;

		paddedRowSizeTMP = (4.0f * (float)ceil((float)(image->width) / 4.0f));
		paddedRowSize = (unsigned int)paddedRowSizeTMP * image->bytesPerPixel;
		unpaddedRowSize = (image->width)*(image->bytesPerPixel);

		currentRowPointer = image->pixels+((image->height-1u)*unpaddedRowSize);

		for (i = 0u; i < image->height; ++i)
		{
			fseek(inputFile, (i*paddedRowSize) + dataOffset, SEEK_SET);
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
 * @brief Preps an image for writing
 * 
 * @param input edge or gray scale image
 * @param output image ready for writing (pixel array need to be preallocated with size => width * height)
 */
#pragma const
void prep_for_writing(IMAGE *input, IMAGE *output)
{
	size_t i;
	unsigned int size = input->width * input->height;
	unsigned int onePixel = 3u;
	unsigned char * restrict pixelsI = input->pixels;
	unsigned char * restrict pixelsO = output->pixels;

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = onePixel;

	for (i = 0u; i < size; i++)
	 {
		pixelsO[i*onePixel] = pixelsI[i];
		pixelsO[i*onePixel +1u] =  pixelsI[i];
		pixelsO[i*onePixel +2u] =  pixelsI[i];
	 }
}

/**
 * @brief Converts a 4 element array to a singel 4B value
 * 
 * @param array input
 * @return unsigned int 
 */
unsigned int get_bytes_to_read(unsigned char array[4])
{
	return (array[3] << 24u) | (array[2] << 16u) | (array[1] << 8u) | array[0];
}

/**
 * @brief Converts a 4B value to an array of 4 elements
 * 
 * @param array output
 * @param whatTowrite input
 */
void get_bytes_to_write(unsigned char array[4], unsigned int whatTowrite)
{
	array[0] = whatTowrite & 0xFFu;
	array[1] = (whatTowrite >> 8)  & 0xFFu;
	array[2] = (whatTowrite >> 16) & 0xFFu;
	array[3] = (whatTowrite >> 24) & 0xFFu;
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
	unsigned char writer[4] = {0u};

	/* File info */
	const char BM[] = "BM";
	float paddedRowSizeTMP = (4.0f * (float)ceil((float) image->width/4.0f));
	unsigned int paddedRowSize = (unsigned int) paddedRowSizeTMP * image->bytesPerPixel;
	unsigned int fileSize = paddedRowSize*image->height + HEADER_SIZE + INFO_HEADER_SIZE;
	unsigned int reserved = 0x0000u;
	unsigned int dataOffset = HEADER_SIZE+INFO_HEADER_SIZE;

	/* Header info */
	unsigned int infoHeaderSize = INFO_HEADER_SIZE;
	unsigned short planes = 1u; /* always 1 */
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

	if (outputFile != NULL) 
	{
		/* Write file info */
		fwrite(&BM[0], 1u, 1u, outputFile);
		fwrite(&BM[1], 1u, 1u, outputFile);

		get_bytes_to_write(writer, fileSize);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, reserved);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, dataOffset);
		fwrite(writer, 1u, 4u, outputFile);

		/* Write header info */
		get_bytes_to_write(writer, infoHeaderSize);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, image->width);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, image->height);
		fwrite(writer, 1u, 4u, outputFile);

		writer[0] = planes & 0xFFu;
		writer[1] = (planes >> 8) & 0xFFu;
		fwrite(writer, 1u, 2u, outputFile);

		writer[0] = bitsPerPixel & 0xFFu;
		writer[1] = (bitsPerPixel >> 8) & 0xFFu;
		fwrite(writer, 1u, 2u, outputFile);

		/* Write compression */
		get_bytes_to_write(writer, compression);
		fwrite(writer, 1u, 4u, outputFile);

		/* Write image size (in bytes) */
		get_bytes_to_write(writer, imageSize);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, resolutionX);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, resolutionY);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, colorsUsed);
		fwrite(writer, 1u, 4u, outputFile);

		get_bytes_to_write(writer, importantColors);
		fwrite(writer, 1u, 4u, outputFile);

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
 * @param grayscaleImage output image (pixel array need to be preallocated with size => width * height)
 */
void convert_to_grayscale(IMAGE *originalImage, IMAGE *grayscaleImage)
{
	size_t i = 0u;
	unsigned int size =  originalImage->height * originalImage->width;
	unsigned int onePixel = originalImage->bytesPerPixel;

	/* Initialize the output image */
	grayscaleImage->height = originalImage->height;
	grayscaleImage->width = originalImage->width;
	grayscaleImage->bytesPerPixel = 1u;

	/* Converting a pixel to gray scale */
	for (i = 0u; i < size; ++i)
	{
		/* Grayscale conversiony 0.11*B + 0.59*G + 0.3*R */
		grayscaleImage->pixels[i] = (unsigned char) (0.11f * originalImage->pixels[i*onePixel] + 0.59f * originalImage->pixels[i*onePixel + 1u] + 0.3f * originalImage->pixels[i*onePixel + 2u]);
	}

}

/**
 * @brief Used for edge detection using 2D convolution method.
 * Note: It can be used for other operation not just edge detection,
 * 		 it only depends on what kernel is providet.
 * 
 * @param input Image
 * @param output Image (pixel array need to be preallocated with size => width * height)
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

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 1u;

	for(i = 0u; i < row; ++i)
	{
		for(j = 0u; j < column; ++j)
		{
			for(m = 0u; m < k_row; ++m)
			{
				mm = k_row - 1u - m;  /* row index of flipped kernel */

				for(n = 0u; n < k_col; ++n)
				{
					nn = k_col - 1u - n;  /* column index of flipped kernel */

					/* index of input signal, used for checking boundary */
					ii = i + (centerY - mm);
					jj = j + (centerX - nn);

					/* ignore input samples which are out of bound */
					if((ii < row) && (jj < column))
					{
						output->pixels[i*column + j] += input->pixels[ii * column + jj] * kernel[mm * k_col + nn];
					}
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
	pixels[0] = C_MASK_B(colors[color_index]);
	pixels[1] = C_MASK_G(colors[color_index]);
	pixels[2] = C_MASK_R(colors[color_index]);
}

/**
 * @brief Color an image based on the code in that pixel
 * 
 * @param input image that has been already codet 
 * @param output image (pixel array need to be preallocated with size => width * height)
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
			 /* -1 because color_index starts from 1 but the color_pixel function start indexing from 0 */ 
			color_pixel(&output->pixels[i * onePixel], color_index - 1u);
		}
	}
}

/**
 * @brief Codes an image, every closed contour gets its own code.
 * Codes are in range of NUM_COLORS
 * 
 * @param image input
 * @param codes helper array, (need to be preallocated with size => width * height)
 */
void code_segments(IMAGE *image, unsigned char *codes, int *queue[2])
{
	size_t i, j;
	unsigned int width = image->width;
	unsigned int height = image->height;
	unsigned int size = width*height;
	unsigned int next_code = 1u;

	for (i = 0u; i < height; i++) 
	{
		for (j = 0u; j < width; j++) 
		{
			/* Check if the pixel is empty and if that pixel isn't already color  then calls flood fill function */
			if ((image->pixels[i * width + j] == 0u) && (codes[i * width + j] == 0u)) 
			{
				flood_fill(image, j, i, next_code, codes, queue);
				next_code = (next_code + 1u) % NUM_COLORS;
			}
		}
	}

	/* Assign the color codes to the image */
	for (i = 0u; i < size; i++) 
	{
		image->pixels[i] = codes[i];
	}
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
void flood_fill(IMAGE *image, unsigned int x, unsigned int y, unsigned int color, unsigned char *codes, int *queue[2])
{
	int i, j, xx, yy;
	unsigned int width = image->width;
	unsigned int height = image->height;
	unsigned char * restrict pixels = image->pixels;
	int front = 0;
	int rear = 0;
	int currentX;
	int currentY;

	queue[0][rear] = x;
	queue[1][rear] = y;
	rear++;
	codes[y * width + x] = color;

	while (front != rear) {
		currentX = queue[0][front];
		currentY = queue[1][front];
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
					queue[0][rear] = xx;
					queue[1][rear] = yy;
					rear++;
				}
			}
		}
	}
}

/**
 * @brief Converts the image to gray scale using average method
 * Optimized version
 * 
 * @param originalImage input image
 * @param grayscaleImage output image (pixel array need to be preallocated with size => width * height)
 */
#pragma const
void convert_to_grayscale_dsp(IMAGE *originalImage, IMAGE *grayscaleImage)
{
	size_t i = 0u;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned int imageSize =  originalImage->height * originalImage->width;
	unsigned int onePixel = originalImage->bytesPerPixel;
	unsigned char * restrict pixels = originalImage->pixels;
	unsigned char * restrict gray_pixels = grayscaleImage->pixels;

	/* Initialize the output image */
	grayscaleImage->height = originalImage->height;
	grayscaleImage->width = originalImage->width;
	grayscaleImage->bytesPerPixel = 1u;

 	/* Converting a pixel to gray scale */
	#pragma no_vectorization
	for (i = 0u; i < imageSize; ++i)
	{
		B = pixels[i * onePixel];
		G = pixels[i * onePixel + 1u];
		R = pixels[i * onePixel + 2u];
		gray_pixels[i] = (R + G + B) / 3u;

	}
}


/**
 * @brief Used for edge detection using 2D convolution method
 * Note: It can be used for other operation not just edge detection,
 * 		 it only depends on what kernel is providet.
 * Optimized version
 *
 * @param input Image
 * @param output Image (pixel array needs to be preallocated with size => width * height)
 * @param kernel array
 * @param k_row number of rows in the kernel
 * @param k_col number of columns in the kernel
 */
#pragma const
void edge_detection_dsp(IMAGE *input, IMAGE *output, char * restrict kernel, unsigned int k_row, unsigned int k_col)
{
	size_t i, j, n, m;
	unsigned int mm, nn, ii, jj;
	unsigned int column = input->width;
	unsigned int row = input->height;
	unsigned int centerX = k_col / 2u;
	unsigned int centerY = k_row / 2u;
	unsigned char * restrict pixelsI = input->pixels;
	unsigned char * restrict pixelsO = output->pixels;

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 1u;

	#pragma vector_for
	for(m = 0u; m < k_row; ++m)
	{
		mm = k_row - 1u - m; /* row index of flipped kernel */

		#pragma vector_for
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
					if(expected_true((ii < row) && (jj < column)))
					{
						pixelsO[i*column + j] += pixelsI[ii * column + jj] * kernel[mm * k_col + nn];
					}
				}
			}
		}
	}
}


/**
 * @brief Color an image based on the code in that pixel
 * Optimized version.
 *
 * @param input image that has been already coded
 * @param output image (pixel array need to be preallocated with size => width * height)
 */
#pragma const
void color_segments_dsp(IMAGE *input, IMAGE *output)
{
	/* Array of colors */
	unsigned int colors[] = {COLOR_1, COLOR_2, COLOR_3, COLOR_4,
								COLOR_5, COLOR_6, COLOR_7, COLOR_8,
								COLOR_9, COLOR_10};
	size_t i,ii;
	unsigned int size = input->height * input->width;
	unsigned int color_index = 0u;
	unsigned int onePixel = output->bytesPerPixel;
	unsigned char * restrict codes = input->pixels;
	unsigned char * restrict pixels = output->pixels;

	/* Initialize the output image */
	output->height = input->height;
	output->width = input->width;
	output->bytesPerPixel = 3u;

	/* Coloring each pixel */
	for (i = 0u; i < size; i++)
	{
		color_index = codes[i];

		if (expected_true(color_index < NUM_COLORS))
		{
			/* Setting RGB values */
			pixels[i * onePixel]      = C_MASK_B(colors[color_index]);
			pixels[i * onePixel + 1u] = C_MASK_G(colors[color_index]);
			pixels[i * onePixel + 2u] = C_MASK_R(colors[color_index]);
		}
	}
}

/**
 * @brief Codes an image, every closed contour gets its own code.
 * Codes are in range of NUM_COLORS.
 * Optimized version.
 *
 * @param image input
 * @param codes helper array, need to be preallocated with size => width * height
 */
#pragma const
void code_segments_dsp(IMAGE *image, unsigned char * restrict codes, int *queue[2])
{
	size_t i, j;
	unsigned int width = image->width;
	unsigned int height = image->height;
	unsigned int size = width * height;
	unsigned int next_code = 1u;
	unsigned char * restrict pixels = image->pixels;

	#pragma vector_for
	for (i = 0u; i < height; i++) 
	{
		for (j = 0u; j < width; j++) 
		{
			/* Check if the pixel is empty and if that pixel isn't already color  then calls flood fill function */
			if (expected_true((pixels[i * width + j] == 0u) && (codes[i * width + j] == 0u)))
			{
				flood_fill_dsp(image, j, i, next_code, codes, queue);
				next_code = (next_code + 1u) % NUM_COLORS;
			}
		}
	}

	/* Assign the color codes to the image */
	#pragma SIMD_for
	for (i = 0u; i < size; i++) 
	{
		pixels[i] = codes[i];
	}
}

/**
 * @brief Uses BFS algoritam to give each pixel in a closed contour unique color code
 * Optimized version.
 * 
 * @param image input
 * @param x position in image
 * @param y position in image
 * @param color current color code
 * @param codes array of previous codet pixels
 */
#pragma const
void flood_fill_dsp(IMAGE *image, unsigned int x,unsigned int y,unsigned int color, unsigned char * restrict codes, int *queue[2])
{
    int i, j, xx, yy;
    int currentX, currentY;
    int front = 0, rear = 0;
	unsigned int width = image->width;
    unsigned int height = image->height;
    unsigned char * restrict pixels = image->pixels;

	queue[0][rear] = x;
	queue[1][rear] = y;
    rear++;
    codes[y * width + x] = color;

    while (front != rear) 
	{
        currentX = queue[0][front];
        currentY = queue[1][front];
        front++;
        for (i = -1; i <= 1; i++) 
		{
            for (j = -1; j <= 1; j++) 
			{
                xx = currentX + i;
                yy = currentY + j;

				/* ignore out of bound positions */
                if  (expected_false(((xx < 0) || (xx >= width)) || ((yy < 0) || (yy >= height))))
				{
                    continue;
                }
                if (expected_true((pixels[yy * width + xx] == 0u) && (codes[yy * width + xx] == 0u)))
				{
                    codes[yy * width + xx] = color;
                    queue[0][rear] = xx;
                    queue[1][rear] = yy;
                    rear++;
                }
            }
        }
    }
}

#ifdef _MISRA_2004_RULES
#pragma diag(pop)
#endif /* _MISRA_2004_RULES */
