#ifndef __HET_IO_TEST_h
#define __HET_IO_TEST_h

#define AID1_7

#include "std_nhet.h"

#define HET_START_SCK_001_0	(e_HETPROGRAM0_UN.Program0_ST.START_SCK_001_0)
#define pHET_START_SCK_001_0  	0

#define HET_PWM001_0	(e_HETPROGRAM0_UN.Program0_ST.PWM001_0)
#define pHET_PWM001_0  	1

#define HET_PWM002_0	(e_HETPROGRAM0_UN.Program0_ST.PWM002_0)
#define pHET_PWM002_0  	2

#define HET_PWM003_0	(e_HETPROGRAM0_UN.Program0_ST.PWM003_0)
#define pHET_PWM003_0  	3

#define HET_END_LABEL_0	(e_HETPROGRAM0_UN.Program0_ST.END_LABEL_0)
#define pHET_END_LABEL_0  	4



typedef union 
{ 
 	HET_MEMORY	Memory0_PST[5];
	struct
	{
		DJZ_INSTRUCTION START_SCK_001_0;
		CNT_INSTRUCTION PWM001_0;
		ECMP_INSTRUCTION PWM002_0;
		ECMP_INSTRUCTION PWM003_0;
		BR_INSTRUCTION END_LABEL_0;
	} Program0_ST; 

} HETPROGRAM0_UN;

extern volatile HETPROGRAM0_UN e_HETPROGRAM0_UN;

extern const HET_MEMORY HET_INIT0_PST[5];

#endif

