/*****************************************************************************
 * image_segmentation.h
 *****************************************************************************/

#ifndef __IMAGE_SEGMENTATION_H__
#define __IMAGE_SEGMENTATION_H__

/* Add your custom header content here */

#include <stdio.h>

#include <cycle_count.h>
#include <cycles.h>

#include "image-utils.h"
#include "led.h"
#include "kernels.h"

/* Heap index on sdram */
#define IMAGE_HEAP_INDEX 1u
/* 4MB of memory */
#define IMAGE_HEAP_SIZE 4000000L;

void test_1(void);
void test_2(void);
void test_1_v2(void);
void test_2_v2(void);
void test_720p(void);


#endif /* __IMAGE_SEGMENTATION_H__ */
