/**
 * @file image_segmentation.c
 * @author Aleksandar Beslic (beslic.alex@gmail.com)
 * @brief Image segmentation using the edge detection algorithm on the ADSP-21489 development platform
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/platform.h>
#include "adi_initialize.h"
#include "image_segmentation.h"

//#include "slika.h"


cycle_t start_count ;
cycle_t final_count ;


/** 
 * If you want to use command program arguments, then place them in the following string. 
 */
char __argv_string[] = "";

int main(int argc, char *argv[])
{
	/**
	 * Initialize managed drivers and/or services that have been added to 
	 * the project.
	 * @return zero on success 
	 */
	adi_initComponents();

	/* Begin adding your custom code here */

	init_led();

	printf("radi\n");

	test_720p();

	printf("gotovo\n");

	return 0;
}

/**
 * @brief Test function of images with resolution of 720x480 or less.
 * Uses unoptimized functions. 
 * 
 */
void test_1(void)
{
	IMAGE image;
	IMAGE gray;
	IMAGE edge;

 	char kernel[] = {1, 4, 1, 4, -20, 4, 1, 4, 1};
	char INPUT_IMAGE_NAME[] = "test.bmp";
	char OUTPUT_IMAGE_NAME[] = "test1_colored.bmp";

	int index = IMAGE_HEAP_INDEX;
	size_t heap_size = IMAGE_HEAP_SIZE;
	unsigned char *codes;

	image.pixels = (unsigned char *)heap_calloc(index, heap_size / 4u, 1u);
	gray.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	edge.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);

	int *queue[2];

	START_CYCLE_COUNT(start_count);
		read_image(INPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles for Reding : ", final_count);

	status_led(READING_DONE);

	START_CYCLE_COUNT(start_count);
		convert_to_grayscale(&image, &gray);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to convert to gray scale : ", final_count);

	prep_for_writing(&gray, &image);
	write_image("gray1t.bmp", &image);

	status_led(GRAY_SCALE_DONE);


	START_CYCLE_COUNT(start_count);
		edge_detection(&gray, &edge, kernel, 3u, 3u);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to detect edges : ", final_count);

	prep_for_writing(&edge, &image);
	write_image("edge1t.bmp", &image);

	status_led(EDGE_DETECT_DONE);

	/* Gray image is no longer in use so we use it's pixel array for other things*/
	heap_free(index, gray.pixels);

	codes = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	queue[0] = (int *)heap_malloc(index, image.width * image.height * sizeof(int));
	queue[1] = (int *)heap_malloc(index, image.width * image.height * sizeof(int));

	if (codes == NULL || queue[0] == NULL || queue[1] == NULL)
	{
		printf("NO MEMORY!\n");
		return;
	}

	START_CYCLE_COUNT(start_count);
		code_segments(&edge, codes, queue);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to code image : ", final_count);

	status_led(CODING_DONE);

	START_CYCLE_COUNT(start_count);
		color_segments(&edge, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to color image : ", final_count);

	status_led(COLORING_DONE);

	START_CYCLE_COUNT(start_count);
		write_image(OUTPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to write image : ", final_count);

	status_led(WRITING_DONE);

	free(queue[0]);
	free(queue[1]);

	heap_free(index, codes);
	heap_free(index, edge.pixels);
	heap_free(index, image.pixels);

}

/**
 * @brief Test function of images with resolution of 720x480 or less.
 * Uses optimized functions. 
 * 
 */
void test_2(void)
{
	IMAGE image;
	IMAGE gray;
	IMAGE edge;

	char kernel[] = {1, 4, 1, 4, -20, 4, 1, 4, 1};
	char FILE_NAME[] = "test.bmp";
	char OUTPUT_IMAGE_NAME[] = "test2_colored.bmp";

	int index = IMAGE_HEAP_INDEX;
	size_t heap_size = IMAGE_HEAP_SIZE;
	unsigned char *codes;
	int *queue[2];

	image.pixels = (unsigned char *)heap_calloc(index, heap_size / 4u, 1u);
	gray.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	edge.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);


	START_CYCLE_COUNT(start_count);
		read_image(FILE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles for Reding : ", final_count);

	status_led(READING_DONE);

	START_CYCLE_COUNT(start_count);
		convert_to_grayscale_dsp(&image, &gray);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to convert to gray scale : ", final_count);

	prep_for_writing(&gray, &image);
		write_image("gray2t.bmp", &image);

	status_led(GRAY_SCALE_DONE);

	START_CYCLE_COUNT(start_count);
		edge_detection_dsp(&gray, &edge, kernel, 3u, 3u);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to detect edges : ", final_count);

	prep_for_writing(&edge, &image);
		write_image("edge2t.bmp", &image);

	status_led(EDGE_DETECT_DONE);

	/* Gray image is no longer in use so we use it's pixel array for other things*/
	heap_free(index, gray.pixels);

	codes = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	queue[0] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	queue[1] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	if (codes == NULL || queue[0] == NULL || queue[1] == NULL)
	{
		printf("NO MEMORY!\n");
		return;
	}


	START_CYCLE_COUNT(start_count);
		code_segments_dsp(&edge, codes, queue);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to code image : ", final_count);

	status_led(CODING_DONE);

	START_CYCLE_COUNT(start_count);
		color_segments_dsp(&edge, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to color image : ", final_count);

	status_led(COLORING_DONE);

	START_CYCLE_COUNT(start_count);
		write_image(OUTPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to write image : ", final_count);

	status_led(WRITING_DONE);

	free(queue[0]);
	free(queue[1]);
	heap_free(index, codes);
	heap_free(index, edge.pixels);
	heap_free(index, image.pixels);

}


#ifdef FAST_READ
/**
 * @brief Test function of images with resolution of 720x480 or less.
 * Image is loaded as a .h file
 * Uses unoptimized functions. 
 * 
 */
void test_1_v2(void)
{
	IMAGE image;
	IMAGE gray;
	IMAGE edge;

 	char kernel[] = {1, 4, 1, 4, -20, 4, 1, 4, 1};
	char OUTPUT_IMAGE_NAME[] = "test1v2_colored.bmp";

	int index = IMAGE_HEAP_INDEX;
	size_t heap_size = IMAGE_HEAP_SIZE;
	unsigned char *codes;
	int *queue[2];

	gray.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	edge.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);

	image.bytesPerPixel = IMAGE_B_PIXEL;
	image.height = IMAGE_HEIGTH;
	image.width = IMAGE_WIDTH;
	image.pixels = slika;

	status_led(READING_DONE);

	START_CYCLE_COUNT(start_count);
		convert_to_grayscale(&image, &gray);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to convert to gray scale : ", final_count);

	status_led(GRAY_SCALE_DONE);

	START_CYCLE_COUNT(start_count);
		edge_detection(&gray, &edge, kernel, 3u, 3u);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to detect edges : ", final_count);

	status_led(EDGE_DETECT_DONE);

	/* Gray image is no longer in use so we use it's pixel array for other things*/
	heap_free(index, gray.pixels);

	codes = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	queue[0] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	queue[1] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	if (codes == NULL || queue[0] == NULL || queue[1] == NULL)
	{
		printf("NO MEMORY!\n");
		return;
	}

	START_CYCLE_COUNT(start_count);
		code_segments(&edge, codes, queue);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to code image : ", final_count);

	status_led(CODING_DONE);

	START_CYCLE_COUNT(start_count);
		color_segments(&edge, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to color image : ", final_count);

	status_led(COLORING_DONE);

	START_CYCLE_COUNT(start_count);
		write_image(OUTPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to write image : ", final_count);

	status_led(WRITING_DONE);

	free(queue[0]);
	free(queue[1]);

	heap_free(index, codes);
	heap_free(index, edge.pixels);

}
#endif


#ifdef FAST_READ
/**
 * @brief Test function of images with resolution of 720x480 or less.
 * Image is loaded as a .h file
 * Uses unoptimized functions.
 * 
 */
void test_2_v2(void)
{
	IMAGE image;
	IMAGE gray;
	IMAGE edge;

	char kernel[] = {1, 4, 1, 4, -20, 4, 1, 4, 1};
	char OUTPUT_IMAGE_NAME[] = "test2V2_colored.bmp";

	int index = IMAGE_HEAP_INDEX;
	size_t heap_size = IMAGE_HEAP_SIZE;
	unsigned char *codes;
	 int *queue[2];

	gray.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	edge.pixels = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);

	image.bytesPerPixel = IMAGE_B_PIXEL;
	image.height = IMAGE_HEIGTH;
	image.width = IMAGE_WIDTH;
	image.pixels = slika;

	status_led(READING_DONE);

	START_CYCLE_COUNT(start_count);
		convert_to_grayscale_dsp(&image, &gray);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to convert to gray scale : ", final_count);

	status_led(GRAY_SCALE_DONE);

	START_CYCLE_COUNT(start_count);
		edge_detection_dsp(&gray, &edge, kernel, 3u, 3u);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to detect edges : ", final_count);

	status_led(EDGE_DETECT_DONE);

	/* Gray image is no longer in use so we use it's pixel array for other things*/
	heap_free(index, gray.pixels);

	codes = (unsigned char *)heap_calloc(index, heap_size / 8u, 1u);
	queue[0] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	queue[1] = (int *)heap_calloc(index, image.width * image.height , sizeof(int));
	if (codes == NULL || queue[0] == NULL || queue[1] == NULL)
	{
		printf("NO MEMORY!\n");
		return;
	}

	START_CYCLE_COUNT(start_count);
		code_segments_dsp(&edge, codes, queue);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to code image : ", final_count);

	status_led(CODING_DONE);

	START_CYCLE_COUNT(start_count);
		color_segments_dsp(&edge, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to color image : ", final_count);

	status_led(COLORING_DONE);

	START_CYCLE_COUNT(start_count);
		write_image(OUTPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to write image : ", final_count);

	status_led(WRITING_DONE);

	free(queue[0]);
	free(queue[1]);
	heap_free(index, codes);
	heap_free(index, edge.pixels);

}
#endif


/**
 * @brief Test function of images with resolution of 1280x720
 * Uses optimized functions.
 * 
 */
void test_720p(void)
{
	/* It works it really works */
	IMAGE image;
	IMAGE gray;
	IMAGE edge;

	char kernel[] = {1, 4, 1, 4, -20, 4, 1, 4, 1};
	char FILE_NAME[] = "test720p.bmp";
	char OUTPUT_IMAGE_NAME[] = "test720p_colored.bmp";

	int index = IMAGE_HEAP_INDEX;
	size_t heap_size = IMAGE_HEAP_SIZE;
	unsigned int WIDTH = 1280u;
	unsigned int HEIGHt = 720u;
	unsigned int BYTES_PER_PIXEL = 3u;
	unsigned char *codes;
	int *queue[2];

	image.pixels = (unsigned char *)heap_calloc(index, WIDTH * HEIGHt * BYTES_PER_PIXEL, 1u);
	gray.pixels = (unsigned char *)heap_calloc(index,  WIDTH * HEIGHt, 1u);

	START_CYCLE_COUNT(start_count);
		read_image(FILE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles for Reding : ", final_count);
	status_led(READING_DONE);

	START_CYCLE_COUNT(start_count);
		convert_to_grayscale_dsp(&image, &gray);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to convert to gray scale : ", final_count);

	status_led(GRAY_SCALE_DONE);

	/* Little memory shuffle*/
	heap_free(index, image.pixels);
	edge.pixels = (unsigned char *)heap_calloc(index, WIDTH * HEIGHt, 1u);

	START_CYCLE_COUNT(start_count);
		edge_detection_dsp(&gray, &edge, kernel, 3u, 3u);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to detect edges : ", final_count);

	status_led(EDGE_DETECT_DONE);

	/* Gray image is no longer in use so we use it's pixel array for other things*/
	heap_free(index, gray.pixels);

	codes = (unsigned char *)heap_calloc(index, WIDTH * HEIGHt, 1u);
	queue[0] = (int *)heap_calloc(index, WIDTH * HEIGHt , sizeof(int));
	queue[1] = (int *)heap_calloc(index, WIDTH * HEIGHt , sizeof(int));
	if (codes == NULL || queue[0] == NULL || queue[1] == NULL)
	{
		printf("NO MEMORY!\n");
		return;
	}

	START_CYCLE_COUNT(start_count);
		code_segments_dsp(&edge, codes, queue);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to code image : ", final_count);

	status_led(CODING_DONE);

	/* Little memory shuffle*/
	heap_free(index, queue[0]);
	heap_free(index, queue[1]);
	heap_free(index, codes);

	image.pixels = (unsigned char *)heap_calloc(index, WIDTH * HEIGHt * BYTES_PER_PIXEL, 1u);

	START_CYCLE_COUNT(start_count);
		color_segments_dsp(&edge, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to color image : ", final_count);

	status_led(COLORING_DONE);

	START_CYCLE_COUNT(start_count);
		write_image(OUTPUT_IMAGE_NAME, &image);
	STOP_CYCLE_COUNT(final_count, start_count);
	PRINT_CYCLES("Number of cycles to write image : ", final_count);

	status_led(WRITING_DONE);

	heap_free(index, edge.pixels);
	heap_free(index, image.pixels);

}
