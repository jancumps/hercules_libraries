#ifndef __HET_IO_TEST_h
#define __HET_IO_TEST_h

#define AID1_7

#include "std_nhet.h"

#define HET_i2c_init_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_init_0)
#define pHET_i2c_init_0  	0

#define HET_i2c_init_01_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_init_01_0)
#define pHET_i2c_init_01_0  	1

#define HET_i2c_init_02_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_init_02_0)
#define pHET_i2c_init_02_0  	2

#define HET_i2c_start_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_start_0)
#define pHET_i2c_start_0  	3

#define HET_i2c_repeat_start_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_repeat_start_0)
#define pHET_i2c_repeat_start_0  	4

#define HET_i2c_write_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_write_0)
#define pHET_i2c_write_0  	5

#define HET_i2c_read_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_read_0)
#define pHET_i2c_read_0  	6

#define HET_i2c_ack_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_ack_0)
#define pHET_i2c_ack_0  	7

#define HET_i2c_stop_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_stop_0)
#define pHET_i2c_stop_0  	8

#define HET_i2c_endlabel_0	(e_HETPROGRAM0_UN.Program0_ST.i2c_endlabel_0)
#define pHET_i2c_endlabel_0  	9



typedef union 
{ 
 	HET_MEMORY	Memory0_PST[10];
	struct
	{
		DJZ_INSTRUCTION i2c_init_0;
		ECMP_INSTRUCTION i2c_init_01_0;
		ECMP_INSTRUCTION i2c_init_02_0;
		BR_INSTRUCTION i2c_start_0;
		BR_INSTRUCTION i2c_repeat_start_0;
		BR_INSTRUCTION i2c_write_0;
		BR_INSTRUCTION i2c_read_0;
		BR_INSTRUCTION i2c_ack_0;
		BR_INSTRUCTION i2c_stop_0;
		BR_INSTRUCTION i2c_endlabel_0;
	} Program0_ST; 

} HETPROGRAM0_UN;

extern volatile HETPROGRAM0_UN e_HETPROGRAM0_UN;

extern const HET_MEMORY HET_INIT0_PST[10];

#endif

