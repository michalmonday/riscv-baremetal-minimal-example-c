#ifndef A2TIME_H
#define A2TIME_H
#include <eembc_adaptation.h>

/* Compilation switches to be defined( or not )based on application follow */

/* Define (=1) when compiling for DOUBLE variables */
#define DATA_SIZE   1       
/* ..or( =0 )when compiling for FLOAT variables */
#if DATA_SIZE == 0           /* SHORT variables */
typedef n_short varsize ;    /* Data and variables are 16 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. real-time ctr in test data */
#else                        /* Else, LONG variables */
typedef n_long varsize;      /* Data and variables are 32 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. real-time ctr in test data */
#endif /* DATA_SIZE */

#define RAM_OUT 0   /* Define (=1)to direct debug text to RAM file */
                    /* or( =0 )to direct debug text to console */

/*******************************************************************************
    Defines                                                                     
*******************************************************************************/

#define false           0
#define true            !false

#if (BMDEBUG && RAM_OUT == 1)	/* Debug buffer size == 32K */ 
#define MAX_FILESIZE    8192    /* Maximum size of output file */
#else
#define MAX_FILESIZE    256     /* Maximum size of output file */
#endif 
#define NUM_TESTS       500     /* Number of sets of input test data stimuli */
#define VAR_COUNT       1       /* Number of variables which must be allocated */
#define HEADER_SIZE     100     /* Approx size of text title and header messages */

#define CYLINDERS       8       /* We're simulating an eight cylinder engine */
#define TENTH_DEGREES   3600    /* Number of 1/10-degrees in a circle */

/* Cylinder #1 firing angle (*10) */
#define FIRE1_ANGLE (TENTH_DEGREES/CYLINDERS*1)
/* Cylinder #2 firing angle (*10) */
#define FIRE2_ANGLE (TENTH_DEGREES/CYLINDERS*2)
/* Cylinder #3 firing angle (*10) */
#define FIRE3_ANGLE (TENTH_DEGREES/CYLINDERS*3)   
/* Cylinder #4 firing angle (*10) */
#define FIRE4_ANGLE (TENTH_DEGREES/CYLINDERS*4)  
/* Cylinder #5 firing angle (*10) */
#define FIRE5_ANGLE (TENTH_DEGREES/CYLINDERS*5)   
/* Cylinder #6 firing angle (*10) */
#define FIRE6_ANGLE (TENTH_DEGREES/CYLINDERS*6)   
/* Cylinder #7 firing angle (*10) */
#define FIRE7_ANGLE (TENTH_DEGREES/CYLINDERS*7)   
/* Cylinder #8 firing angle (*10) */
#define FIRE8_ANGLE (TENTH_DEGREES/CYLINDERS*8)   

#define CYL1            1       /* Cylinder #1 firing window */
#define CYL2            2       /* Cylinder #2 firing window */
#define CYL3            3       /* Cylinder #3 firing window */
#define CYL4            4       /* Cylinder #4 firing window */
#define CYL5            5       /* Cylinder #5 firing window */
#define CYL6            6       /* Cylinder #6 firing window */
#define CYL7            7       /* Cylinder #7 firing window */
#define CYL8            8       /* Cylinder #8 firing window */

#define TDC_TEETH       2       /* Number of missing teeth (=1) at TDC */
#define TDC_MARGIN      0.9     /* Discrimination window for TDC teeth */
#define NUM_TEETH       60      /* Number of teeth on tonewheel */

/* Arbitrary scale factor for computing internal RPM */
#define RPM_SCALE_FACTOR        3600000 

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS            /* Don't define these twice! */

extern n_int   tableCount ;     /* Number of passes through table */
extern varsize angleCounter ;   /* Current 'angleCounter' pulled from  data */
extern varsize *inpAngleCount ; /* Array of 'angleCounter' test data values */
extern varsize tonewheelTeeth ; /* Number of teeth on the tonewheel */
/*extern n_int isTopDeadCenter ;  * TRUE/FALSE flag when TDC occurs */

#endif /* ALGO_GLOBALS */       


#endif