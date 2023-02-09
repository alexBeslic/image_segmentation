/*
 * 
 * kernels.h
 *
 *  Created on: 03.01.2023.
 *      Author: Aleksandar Beslic
 *       email: beslic.alex@gmail.com
 */

#ifndef KERNELS_H_
#define KERNELS_H_


/**
 * @brief Laplas kernel
 * 
 */
char kernel1[] = {-1, 0, -1,
				   0, 4, 0,
				  -1, 0, -1};

/**
 * @brief The best one
 * 
 */
char kernel2[] = {1, 4, 1,
				  4, -20, 4,
				  1, 4, 1};

/**
 * @brief Quick kernel
 * 
 */
char kernel3[] = {0, 1, 0,
				 1, -4, 1,
				 0, 1, 0};

/**
 * @brief Laplas kernel v2
 * 
 */
char kernel4[] = {-1, -1, -1,
				  -1, 8, -1,
				  -1, -1, -1};

/**
 * @brief LoG kernel
 * 
 */
char kernel5[] = {0, 0, 1, 0, 0,
				  0, 1, 2, 1, 0,
				  1, 2,-16, 2, 1,
				  0, 1, 2, 1, 0,
				  0, 0, 1, 0, 0};

#endif /* KERNELS_H_ */
