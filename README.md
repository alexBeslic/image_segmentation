# Image segmentation using the edge detection algorithm on the ADSP-21489 development platform

## Basic overview of the project
- An image in .bmp format is loaded onto the ADSP-21489 development platform, which is then converted to gray
scale image.
- Then edge detection is done using the Laplace algorithm (i.e. the second derivative of the signal)
- After the edges are detecting on the image, it is then coded and colored so that each closed contour has its own code, that is, each contour is colored with a separate color. The coloring principle is based on the flood fill algorithm that use BFS searches to find a closed contour.


## Developer board
The board is a _Analog Devices_ **ADSP-21489** _rev. 0.2_ from the  _SHARC_ family.  
[From more info](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/21489-EZBRD.html#eb-overview)

## Compile and run
The simplest way to compile and run the program is to use _CrossCore Embedded Studio_.

### Provided files
The codebase has the following files: 
- **image_segmentation.c** (Contains the main function)
- **image-utils.c** (Cointns function for image manipulation)