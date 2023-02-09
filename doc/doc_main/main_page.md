@mainpage

# Image segmentation using the edge detection algorithm on the ADSP-21489 development platform

## Basic overview of the project
- An image in .bmp format is loaded onto the ADSP-21489 development platform, which is then converted to gray
scale image.
- Then edge detection is done using 2D convolution and Laplacian operator (i.e. the second derivative of the signal)
- After the edges are detected in the image, it is then coded and colored so that each closed contour has its own code, that is, each contour is colored with a separate color. The coloring principle is based on the flood fill algorithm that uses BFS searches to find a closed contour.
- You can track the process of segmentation on the included LED loading bar on the board.


## Developer board
The board is a _Analog Devices_ **ADSP-21489** _rev. 0.2_ from the  _SHARC_ family.  
[From more info](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/21489-EZBRD.html#eb-overview)

## Compile and run
The simplest way to compile and run the program is to use _CrossCore Embedded Studio_. The _image_segmentation.c_ contaist test function that you can use. The image that you want to test needs to be in the _Debug_ directory.

## Provided files
The codebase has the following files: 
- **image_segmentation.c** (Contains the main and test function)
- **image-utils.c** (Cointns function for image manipulation)
- **led.c** (Cointns function for LED manipulation on the board)
- **kernels.h** (Cointns kernel examples of edge detection)
- **slika.h** (An image in the form of a .h file)
- **convertor.c** (Converst an image from .bmp format to a .h file)  
- **doc/** (In the doc diretory you can find documentation on all the provided functions)


**NOTE:** For the love of GOD and all that is holy DON'T use this board and especially DON'T use this IDE.