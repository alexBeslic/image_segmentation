/*****************************************************************************
 * image_segmentation.c
 *****************************************************************************/

#include <sys/platform.h>
#include "adi_initialize.h"
#include "image_segmentation.h"


cycle_t start_count ;
cycle_t final_count ;

enum PROCESS_DONE {READING_DONE, GRAY_SCALE_DONE, EDGE_DETECT_DONE, CODING_DONE, COLORING_DONE, WRITING_DONE};

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

	return 0;
}



/**
 * @brief Turns on a LED on the ADSP-21489 board whaen a specific proces is done
 * 
 * @param what_led to turn on in range of PROCESS_DONE
 */
void status_led(unsigned int what_led)
{
	switch (what_led)
	{
	case READING_DONE:
		sysreg_bit_set(sysreg_FLAGS, FLG4);
		break;
	case GRAY_SCALE_DONE:
		sysreg_bit_set(sysreg_FLAGS, FLG5);
		break;
	case EDGE_DETECT_DONE:
		sysreg_bit_set(sysreg_FLAGS, FLG6);
		break;
	case CODING_DONE:
		SRU(HIGH,DAI_PB03_I);
		break;
	case COLORING_DONE:
		SRU(HIGH,DAI_PB04_I);
		break;
	case WRITING_DONE:
		SRU(HIGH,DAI_PB15_I);
		break;
	default:
		break;
	}
}


/**
 * @brief Initializes LEDs on the ADSP-21489 board 
 * 
 */
void init_led(void)
{
	/* LED01 */
	SRU(HIGH,DPI_PBEN06_I);
	SRU(FLAG4_O,DPI_PB06_I);
	/* LED02 */
	SRU(HIGH,DPI_PBEN13_I);
	SRU(FLAG5_O,DPI_PB13_I);
	/* LED03 */
	SRU(HIGH,DPI_PBEN14_I);
	SRU(FLAG6_O,DPI_PB14_I);
	/* LED04 */
	SRU(HIGH,DAI_PBEN03_I);
	SRU(HIGH,DAI_PB03_I);
	/* LED05 */
	SRU(HIGH,DAI_PBEN04_I);
	SRU(HIGH,DAI_PB04_I);
	/* LED06 */
	SRU(HIGH,DAI_PBEN15_I);
	SRU(HIGH,DAI_PB15_I);
	/* LED07 */
	SRU(HIGH,DAI_PBEN16_I);
	SRU(HIGH,DAI_PB16_I);
	/* LED08 */
	SRU(HIGH,DAI_PBEN17_I);
	SRU(HIGH,DAI_PB17_I);

	/* Setting flag pins as outputs */
	sysreg_bit_set(sysreg_FLAGS, (FLG4O|FLG5O|FLG6O) );
	/* Setting HIGH to flag pins */
	sysreg_bit_set(sysreg_FLAGS, (FLG4|FLG5|FLG6) );

	/* Turn off LEDs */
	sysreg_bit_clr(sysreg_FLAGS, FLG4);
	sysreg_bit_clr(sysreg_FLAGS, FLG5);
	sysreg_bit_clr(sysreg_FLAGS, FLG6);
	SRU(LOW,DAI_PB03_I);
	SRU(LOW,DAI_PB04_I);
	SRU(LOW,DAI_PB15_I);
	SRU(LOW,DAI_PB16_I);
	SRU(LOW,DAI_PB17_I);
}
