/*
 * led.h
 *
 *  Created on: 03.01.2023.
 *      Author: Aleksandar Beslic
 *       email: beslic.alex@gmail.com
 */

#ifndef LED_H_
#define LED_H_

#include <def21489.h>
#include <sru21489.h>
#include <SYSREG.h>

/* Used in combination with status_led() */
enum PROCESS_DONE {READING_DONE, GRAY_SCALE_DONE, EDGE_DETECT_DONE, CODING_DONE, COLORING_DONE, WRITING_DONE};

void init_led(void);
void status_led(unsigned int what_led);

#endif /* LED_H_ */
