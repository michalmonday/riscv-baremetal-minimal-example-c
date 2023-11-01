#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Notes about adapting EEMBC algorithms to run on a standalone
baremetal Flute processor, without having the th_lib.h and 
without following the official/proper EEMBC benchmark harness:

- replace every occurence of tcdef
- copy data input (algotst.c) 
- remove CRC check
- replace functions that start with "th_" except those declared in "includes/eembc_adaptation.h"
- remove first parameter from each call of th_exit (this is some kind of ID that is defined in the unavailable th_lib.h)
- remove all lines that mention RAMfile...
- remove GetTestData function, it is reduntant (possibly by design, it would allow to load data from a file into ram, but in this case the data is already in ram)
- rename data variable names to match "inpAngleROM" array name
- remove "struct TCDef *tcdef parameter" from "t_run_test" function
- copy algo.h to algorithms directory, rename it to "<main file name>.h" and put guard headers "ifndef" inside it. Remove functions declarations from it.
- the "<main file name>.h" should include <eembc_adaptation.h> at the top
- the code has 2 output functions: "WriteOut" and "DebugOut"

- replace each call to GetInputValues with:

    isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
    angleCounter = inpAngleROM[input_index]; //    angleCounter = inpAngleROM[loop_cnt % (sizeof(inpAngleROM)/sizeof(inpAngleROM[0]))];

*/

// originally file.h was "algo.h"
#include "rspeed.h" // change to the name of the main file (eg. "a2time.h")

// forward declarations
static int t_run_test( int argc, const char *argv[] );
static n_int GetInputValues(n_void);

/*
* The following test data represents values read from a realtime clock
* which is captured( in hardware )on each rising edge of a tonewheel
* tooth.  The counter is expected to roll over at maximum count, and
* it is presumed to be an "up" counter.
*
*/    

static const varsize inpTonewheelROM[] = 
{        
    123, 456, 796, 1143, 1497, 1858, 2226, 2601, 3358, 3743, 4136, 4535, 4942, 5355, 5776, 
    6203, 6638, 7079, 7528, 7983, 8446, 8915, 9392, 9875, 10366, 10863, 11368, 11879, 12398, 12923, 
    13456, 13995, 14542, 15095, 15656, 16223, 16798, 17379, 17968, 18563, 19166, 19775, 20392, 21015, 21646, 
    22283, 22928, 23579, 24238, 24903, 25576, 26255, 26942, 27635, 28336, 29043, 29758, 30479, 31208, 31943, 
    32686, 668, 1425, 2189, 2960, 3738, 4523, 5315, 6906, 7708, 8518, 9334, 10158, 10988, 11826, 
    12670, 13522, 14380, 15246, 16118, 16998, 17884, 18778, 19678, 20586, 21500, 22422, 23350, 24286, 25228, 
    26178, 27134, 28098, 29068, 30046, 31030, 32022, 250, 1253, 2263, 3280, 4304, 5335, 6373, 7418, 
    8470, 9529, 10595, 11668, 12748, 13835, 14929, 16030, 17138, 18253, 19375, 20504, 21640, 22783, 23933, 
    25090, 26254, 27425, 28603, 29788, 30980, 32179, 620, 3045, 4264, 5491, 6724, 7965, 9212, 10467, 
    11728, 12997, 14272, 15555, 16844, 18141, 19444, 20755, 22072, 23397, 24728, 26067, 27412, 28765, 30124, 
    31491, 100, 1484, 2875, 4273, 5678, 7090, 8509, 9935, 11368, 12808, 14255, 15709, 17170, 18638, 
    20113, 21595, 23084, 24580, 26083, 27593, 29110, 30634, 32165, 930, 2470, 4017, 5571, 7132, 8700, 
    10275, 11857, 13446, 15042, 16645, 18255, 19872, 21496, 24751, 26385, 28027, 29675, 31331, 230, 1904, 
    3585, 5273, 6968, 8670, 10379, 12095, 13818, 15548, 17285, 19029, 20780, 22538, 24303, 26075, 27854, 
    29640, 31433, 460, 2262, 4071, 5887, 7710, 9540, 11377, 13221, 15072, 16930, 18795, 20667, 22546, 
    24432, 26325, 28225, 30132, 32046, 1200, 3129, 5065, 7008, 8958, 10915, 12879, 14850, 16828, 18813, 
    20805, 22804, 24810, 26823, 28843, 30870, 130, 2165, 6242, 8287, 10340, 12399, 14466, 16539, 18620, 
    20707, 22802, 24903, 27012, 29127, 31250, 610, 2745, 4887, 7036, 9192, 11355, 13525, 15702, 17886, 
    20077, 22275, 24480, 26692, 28911, 31137, 600, 2838, 5083, 7335, 9594, 11860, 14133, 16413, 18700, 
    20994, 23295, 25603, 27918, 30240, 32569, 2140, 4486, 6812, 9118, 11404, 13670, 15916, 18142, 20348, 
    22534, 24700, 26846, 28972, 31078, 430, 2530, 4610, 8777, 10867, 12938, 14988, 17019, 19029, 21020, 
    22990, 24941, 26871, 28782, 30672, 32543, 1650, 3505, 5340, 7155, 8950, 10725, 12480, 14215, 15930, 
    17625, 19300, 20955, 22590, 24205, 25800, 27375, 28930, 30465, 31980, 720, 2208, 3676, 5124, 6552, 
    7960, 9371, 10785, 12202, 13622, 15045, 16471, 17900, 19332, 20767, 22205, 23646, 25090, 26537, 27987, 
    29440, 30896, 32355, 1050, 2516, 3985, 5457, 6932, 9889, 11374, 12863, 14354, 15849, 17346, 18847, 
    20350, 21857, 23366, 24879, 26394, 27913, 29434, 30959, 32486, 1250, 2784, 4322, 5862, 7406, 8952, 
    10502, 12054, 13610, 15168, 16730, 18294, 19862, 21432, 23006, 24582, 26162, 27744, 29330, 30918, 32510, 
    1340, 2941, 4545, 6152, 7762, 9375, 10991, 12610, 14232, 15857, 17485, 19116, 20750, 22387, 24027, 
    25670, 27316, 28965, 30617, 32272, 1160, 2819, 4481, 7812, 9484, 11160, 12838, 14520, 16204, 17892, 
    19582, 21276, 22972, 24672, 26374, 28080, 29788, 31500, 450, 2171, 3895, 5622, 7352, 9085, 10821, 
    12560, 14302, 16047, 17795, 19546, 21300, 23057, 24817, 26580, 28346, 30115, 31887, 900, 2684, 4471, 
    6261, 8054, 9850, 11649, 13451, 15256, 17064, 18875, 20689, 22503, 24317, 26131, 27945, 29759, 31573, 
    620, 2435, 4250, 6065, 7880, 9695, 11510, 13325, 16962, 18787, 20613, 22438, 24264, 26089, 27915, 
    29740, 31566, 620, 2442, 4264 
} ; /* End of test values :  inpTonewheelROM[] */


#ifndef RANDOM_FUNCTION_GENERATOR
int main(void) {
    t_run_test(0, NULL);

    // stop the program
    asm("wfi");
    return 0;
}
#endif

/*  DECLARATIONS */    
static n_int   *RAMfile ;          /* Pointer to test output RAM file */
static n_int   *RAMfilePtr ;       /* Pointer to position in output RAM file */
static n_int   RAMfileSize ;       /* Size of the debug output RAM file */
static n_int   tableCount ;        /* Number of passes through table */
static n_int   *RAMfileEOF;        /* points to end of RAM file */
static n_int   RAMfile_increment;  /* difference between varsize and n_int */

/* Current 'tonewheelCounter' pulled from test data */
static varsize tonewheelCounter ;   
/* Array of 'tonewheelCounter' test data values */
static varsize *inpTonewheelCount ; 
/* Number of teeth on the tonewheel */
static varsize tonewheelTeeth ;     

static int input_index;

static int t_run_test(int argc, const char *argv[] )
{    
    int iterations = 1;
    scanf("%d", &input_index);
    int inputs_count = sizeof(inpTonewheelROM) / sizeof(inpTonewheelROM[0]);
    if (input_index >= inputs_count) {
        printf("ERROR: input_index %d is out of range, max is %d\n", input_index, inputs_count-1);
        printf("stopping execution\n");
        asm volatile ("wfi");
    }


#if BMDEBUG
    char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks ( c )1998-1999\n"
        "Algorithm 6 :  Road Speed Calculation  Rev. 1.0F0\n" ; 
    char *szHeader = 
        "\n\ntonewheelCount, toothDeltaT, toothCount, toothTimeAccum, roadSpeed"
        ", counter\n" ; 
    char szDebug[100] ; 
#endif /* BMDEBUG */
    varsize i ; 

    static char szDataSize[40] ; 
    /* Input test data table looped */
    int isTableLooped = false ;    
    /* Output variable 'roadSpeed' is calculated */
    static varsize roadSpeed1 ;    
    static varsize roadSpeed2 ; 
    static varsize roadSpeed3 ; 
    /*  from inter-pulse period, 'toothDeltaTime' */
    static varsize toothDeltaTime1 ;    
    static varsize toothDeltaTime2 ; 
    static varsize toothDeltaTime3 ; 
    /* History of 'toothDeltaTime' */
    static varsize toothDeltaTimeLast1 ;    
    static varsize toothDeltaTimeLast2 ; 
    static varsize toothDeltaTimeLast3 ; 
    /* History of the pulse timer/counter */
    static varsize tonewheelCounterLast1 ;    
    static varsize tonewheelCounterLast2 ; 
    static varsize tonewheelCounterLast3 ; 
    /* Accumulator for filtering pulse time */
    static long toothTimeAccum1 ;    
    static long toothTimeAccum2 ; 
    static long toothTimeAccum3 ; 
    /* Counter for filtering pulse time */
    static varsize toothCount1 ;    
    static varsize toothCount2 ; 
    static varsize toothCount3 ; 

    /* Unused */
    argc = argc ;
    argv = argv ;

    /* 
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     *
     */    

    /* Variable initializations at t=0 */    

    /* ...and pulse filter */
    toothCount1 = 0 ;    
    toothCount2 = 0 ; 
    toothCount3 = 0 ; 
    /* ...and haven't accumulated for filter */
    toothTimeAccum1 = 0 ;    
    toothTimeAccum2 = 0 ; 
    toothTimeAccum3 = 0 ; 
    /* ...Road speed starts at '0' */
    roadSpeed1 = 0 ;    
    roadSpeed2 = 0 ; 
    roadSpeed3 = 0 ; 
    /* ...and no pulse history yet */
    tonewheelCounterLast1 = 0 ;    
    tonewheelCounterLast2 = 0 ; 
    tonewheelCounterLast3 = 0 ; 
    toothDeltaTimeLast1 = MAX_TOOTH_TIME ; 
    toothDeltaTimeLast2 = MAX_TOOTH_TIME ; 
    toothDeltaTimeLast3 = MAX_TOOTH_TIME ; 
    tableCount = 0 ;    /* Start out at beginning of input test data */

    // /* If debug output is desired, then must allocate some RAM... */    
    // RAMfilePtr = 0 ;    /* Point to beginning of test output file */
    // /* Set size of output file (1K)*/    
    // RAMfileSize = MAX_FILESIZE; 
   
    // /* Allocate some RAM for output file */    
    // RAMfile         = (n_int *)th_malloc( RAMfileSize * sizeof(n_int) + sizeof (varsize) ) ;
    // if ( RAMfile == NULL )
    //       th_exit( "Cannot Allocate Memory %s:%d", __FILE__,__LINE__ );
 
	/*  // align it to varsize boundary */ 
	// RAMfile       = ((n_int *)ROUNDUP(RAMfile, sizeof (varsize) ));
    // RAMfilePtr    = RAMfile; /* Point to beginning of test output file */
    // RAMfileEOF    = RAMfile + RAMfileSize;  /* should be OK ANSI C */

    // if (sizeof(varsize) > sizeof(n_int)) {    /* this is a floating point benchmark! */
    //     RAMfile_increment = sizeof(varsize) / sizeof(n_int);
    //     if (sizeof(varsize) % sizeof(n_int)) RAMfile_increment += 1;  /* should never happen, but ... */
    // }
    // else{
    //     RAMfile_increment = 1;
    // }

    /* Tell us the compiled data size */    
#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", 
        ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = ( char )( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */

    ///* Initialize the test data -- stimuli for the algorithm. */
    //if( !GetTestData() )   /* Allocate for the test input data table */
    //{
    //    th_exit( THE_OUT_OF_MEMORY, "Cannot Allocate Memory %s : %d",
    //        __FILE__, __LINE__ ) ; 
    //}

    //  tcdef->CRC = 0;
    //th_signal_start() ; /* Tell the host that the test has begun */

#if BMDEBUG
    DebugOut( szTitle ) ;    /* Print the title message in RAM file */
    DebugOut( szDataSize ) ; /* ...and the data size */
    DebugOut( szHeader ) ;   /* Print the test output file header in RAM file */
#endif /* BMDEBUG */

    /* This is the actual benchmark algorithm. */    

    /* Perform road-speed calculation */    

    for( int loop_cnt = 0 ; loop_cnt < iterations ; loop_cnt++ )
    {

#if BMDEBUG
        if( isTableLooped )
        {
            DebugOut( "END--END--END\n" ) ;
        }
#endif /* BMDEBUG */

        /***********************************************************************
            First Pass                                                          
        ***********************************************************************/

        isTableLooped = GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%10ld", (long)tonewheelCounter ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Compute 'toothDeltaTime' -- the difference in counter 
         * readings from the last pulse to the current one.  Note that
         * the realtime counter may roll over, so the elapsed time may
         * need to be corrected when this occurs.
         *
         * The 'roadSpeed' calculation will be performed twice for each
         * revolution of the tonewheel, so some filtering of pulse-period
         * is performed.
         *
         * If the period between pulses goes out to some maximum, then
         * road speed will be forced to zero.  Also, if the period between
         * pulses gets unrealistically small, we'll ignore it, presuming
         * that we're just picking up noise.
         *
         * We also watch out for strange excursions in the pulse period, to
         * filter out errors :  if the period suddenly jumps to several times
         * the last period measured, then it is ignored.
         *
         */    

        /* Compute period between pulses */    
        toothDeltaTime1 = (varsize)
            ( tonewheelCounter - tonewheelCounterLast1 ) ; 

        /* Check for timer overflow */
        if( tonewheelCounterLast1 > tonewheelCounter )
        {
            /* ...and correct for it */
            toothDeltaTime1 += ( varsize )( MAX_VARIABLE + 1 ) ;
        }

        /* Watch out for erroneously short */
        if( toothDeltaTime1 < MIN_TOOTH_TIME )   
        {
            /* ...pulse period */
            toothDeltaTime1 = toothDeltaTimeLast1 ;    
        }
        
        /* Watch out for erroneously long */
        if( toothDeltaTime1 > 4 * toothDeltaTimeLast1 )
        {
            /* ...pulse period */
            toothDeltaTime1 = toothDeltaTimeLast1 ;
        }

        /* Update timer history */
        tonewheelCounterLast1 = tonewheelCounter ;
        /* ...and accumulate pulses for filtering */
        toothTimeAccum1 += toothDeltaTime1 ;
        /* ...counting the pulses being filtered */
        toothCount1++ ;
        /* Remember this pulse's period */
        toothDeltaTimeLast1 = toothDeltaTime1 ;

        /* Time to update ? */
        if( toothCount1 >= tonewheelTeeth / 2 )
        {            
            /* Yes, */
            if( toothTimeAccum1 > MAX_TOOTH_TIME *tonewheelTeeth / 2 )   
            {    
                /* ...check for zero road speed */
                roadSpeed1 = 0 ;    
            }
            else
            {
                /* ...or compute road speed */
                roadSpeed1 = (varsize ) ( SPEEDO_SCALE_FACTOR  / 
                      ( toothTimeAccum1 / tonewheelTeeth * 2 ) ) ; 
                /* ...then reset the filter counter */
                toothCount1 = 0 ;
                /* ...and clear the accumulator */
                toothTimeAccum1 = 0 ;
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld, %4ld, %6ld", (long)toothDeltaTime1, 
            (long)toothCount1, (long)toothTimeAccum1 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %6ld\n", (long)roadSpeed1 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( roadSpeed1 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/

        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%10ld", (long)tonewheelCounter ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Compute period between pulses */    
        toothDeltaTime2 = (varsize)
            ( tonewheelCounter - tonewheelCounterLast2 ) ; 

        /* Check for timer overflow */
        if( tonewheelCounterLast2 > tonewheelCounter )
        {
            /* ...and correct for it */
            toothDeltaTime2 += ( varsize )( MAX_VARIABLE + 1 ) ;
        }

        /* Watch out for erroneously short */
        if( toothDeltaTime2 < MIN_TOOTH_TIME )   
        {
            /* ...pulse period */
            toothDeltaTime2 = toothDeltaTimeLast2 ;    
        }
        
        /* Watch out for erroneously long */
        if( toothDeltaTime2 > 4 * toothDeltaTimeLast2 )
        {
            /* ...pulse period */
            toothDeltaTime2 = toothDeltaTimeLast2 ;
        }

        /* Update timer history */
        tonewheelCounterLast2 = tonewheelCounter ;
        /* ...and accumulate pulses for filtering */
        toothTimeAccum2 += toothDeltaTime2 ;
        /* ...counting the pulses being filtered */
        toothCount2++ ;
        /* Remember this pulse's period */
        toothDeltaTimeLast2 = toothDeltaTime2 ;

        /* Time to update ? */
        if( toothCount2 >= tonewheelTeeth / 2 )
        {            
            /* Yes, */
            if( toothTimeAccum2 > MAX_TOOTH_TIME *tonewheelTeeth / 2 )   
            {    
                /* ...check for zero road speed */
                roadSpeed2 = 0 ;    
            }
            else
            {
                /* ...or compute road speed */
                roadSpeed2 = (varsize ) ( SPEEDO_SCALE_FACTOR  / 
                      ( toothTimeAccum2 / tonewheelTeeth * 2 ) ) ; 
                /* ...then reset the filter counter */
                toothCount2 = 0 ;
                /* ...and clear the accumulator */
                toothTimeAccum2 = 0 ;
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld, %4ld, %6ld", (long)toothDeltaTime2, 
            (long)toothCount2, (long)toothTimeAccum2 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %6ld\n", (long)roadSpeed2 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( roadSpeed2 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/

        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%10ld", (long)tonewheelCounter ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Compute period between pulses */    
        toothDeltaTime3 = (varsize)
            ( tonewheelCounter - tonewheelCounterLast3 ) ; 

        /* Check for timer overflow */
        if( tonewheelCounterLast3 > tonewheelCounter )
        {
            /* ...and correct for it */
            toothDeltaTime3 += ( varsize )( MAX_VARIABLE + 1 ) ;
        }

        /* Watch out for erroneously short */
        if( toothDeltaTime3 < MIN_TOOTH_TIME )   
        {
            /* ...pulse period */
            toothDeltaTime3 = toothDeltaTimeLast3 ;    
        }
        
        /* Watch out for erroneously long */
        if( toothDeltaTime3 > 4 * toothDeltaTimeLast3 )
        {
            /* ...pulse period */
            toothDeltaTime3 = toothDeltaTimeLast3 ;
        }

        /* Update timer history */
        tonewheelCounterLast3 = tonewheelCounter ;
        /* ...and accumulate pulses for filtering */
        toothTimeAccum3 += toothDeltaTime3 ;
        /* ...counting the pulses being filtered */
        toothCount3++ ;
        /* Remember this pulse's period */
        toothDeltaTimeLast3 = toothDeltaTime3 ;

        /* Time to update ? */
        if( toothCount3 >= tonewheelTeeth / 3 )
        {            
            /* Yes, */
            if( toothTimeAccum3 > MAX_TOOTH_TIME *tonewheelTeeth / 3 )   
            {    
                /* ...check for zero road speed */
                roadSpeed3 = 0 ;    
            }
            else
            {
                /* ...or compute road speed */
                roadSpeed3 = (varsize ) ( SPEEDO_SCALE_FACTOR  / 
                      ( toothTimeAccum3 / tonewheelTeeth * 3 ) ) ; 
                /* ...then reset the filter counter */
                toothCount3 = 0 ;
                /* ...and clear the accumulator */
                toothTimeAccum3 = 0 ;
            }
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld, %4ld, %6ld", (long)toothDeltaTime3, 
            (long)toothCount3, (long)toothTimeAccum3 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %6ld\n", (long)roadSpeed3 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( roadSpeed3 ) ; 
#if DATA_SIZE == 0
        i = (varsize)( loop_cnt &0x0000FFFF ) ; 
        WriteOut( i ) ;
        i = (varsize)( loop_cnt >> 16 ) ; 
        WriteOut( i ) ; 
#else
        WriteOut( loop_cnt ) ; 
#endif /* DATA_SIZE */
        i = ( varsize )0xAAAA ; 
        WriteOut( i ) ;
#endif /* BMDEBUG */

#if BMDEBUG
        if( !th_harness_poll() )
        {
            break ; 
        }
#endif /* BMDEBUG */
    }

    return 0;
} /* End of function 't_run_test' */



// /******************************************************************************* Functions                                                                   *******************************************************************************/ /* *  Function :  GetTestData * *    Builds the table of input data which represents 'tonewheelCounter' *    input parameter.  This is the "real world" data stream which *    drives the algorithm.  This data is installed in RAM prior to execution of the *    algorithm.  * */ 
// n_int GetTestData(n_void)
// {
//     int i;
//     tonewheelTeeth = NUM_TEETH;
//     inpTonewheelCount = (varsize *)th_malloc((NUM_TESTS + 1) * sizeof(varsize));
//     if (inpTonewheelCount == NULL)
//     {
//         return (false);
//     } /*  Copy the test values from ROM to RAM  */
//     for (i = 0; i < NUM_TESTS; i++)
//     {
//         inpTonewheelCount[i] = inpTonewheelROM[i];
//     }
//     return true;
// } /* End of function 'GetTestData' */ /* *  Function :  GetInputValues * *    On each pass of the table lookup, a value must be input for 'tonewheelCounter'.  *    Each time this function is called, the next input value is *    pulled from the table in RAM.  The table wraps around, so that input data is *    continuous.     A flag is returned TRUE whenever the table wraps around.  * */

static n_int GetInputValues(n_void)
{
    tonewheelCounter = inpTonewheelROM[input_index++];
    if (input_index >= sizeof(inpTonewheelROM) / sizeof(inpTonewheelROM[0]))
        input_index = 0;
    return false;

    // tonewheelCounter = inpTonewheelCount[tableCount];
    // if (++tableCount < NUM_TESTS)
    // {
    //     return false;
    // }
    // tableCount = 0;
    // return true;
} /* End of function 'GetInputValues' */