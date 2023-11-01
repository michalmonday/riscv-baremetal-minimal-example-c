#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Notes about adapting EEMBC algorithms to run on a standalone
baremetal Flute processor, without having the th_lib.h and 
without following the official/proper EEMBC benchmark harness:

- replace every occurence of tcdef
- copy data input (algotst.c) to the back of the main file (with forward declaration)
- remove CRC check
- replace functions that start with "th_" except those declared in "includes/eembc_adaptation.h"
- remove first parameter from each call of th_exit (this is some kind of ID that is defined in the unavailable th_lib.h)
- remove all lines that mention RAMfile...
- remove GetTestData function, it is reduntant (possibly by design, it would allow to load data from a file into ram, but in this case the data is already in ram)
- rename data variable name to match "inpAngleROM" array name
- remove "struct TCDef *tcdef parameter" from "t_run_test" function
- copy algo.h to algorithms directory, rename it to "<main file name>.h" and put guard headers "ifndef" inside it. Remove functions declarations from it.
- the "<main file name>.h" should include <eembc_adaptation.h> at the top
- the code has 2 output functions: "WriteOut" and "DebugOut"

- replace each call to GetInputValues with:

    isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
    angleCounter = inpAngleROM[input_index]; //    angleCounter = inpAngleROM[loop_cnt % (sizeof(inpAngleROM)/sizeof(inpAngleROM[0]))];

*/

// originally a2time.h was "algo.h"
#include "a2time.h"
//#include <eembc_adaptation.h>



// forward declarations
static int t_run_test( int argc, const char *argv[] );
int a2time(int argc, const char *argv[]) {
    return t_run_test(argc, argv);
}

static const int inpAngleROM[] = {        
    123, 456, 796, 1143, 1497, 1858, 2226, 2601, 3358, 3743, 4136, 4535, 
    4942, 5355, 5776, 6203, 6638, 7079, 7528, 7983, 8446, 8915, 9392, 9875,
    10366, 10863, 11368, 11879, 12398, 12923, 13456, 13995, 14542, 15095,
    15656, 16223, 16798, 17379, 17968, 18563, 19166, 19775, 20392, 21015, 
    21646, 22283, 22928, 23579, 24238, 24903, 25576, 26255, 26942, 27635, 
    28336, 29043, 29758, 30479, 31208, 31943, 32686, 668, 1425, 2189, 
    2960, 3738, 4523, 5315, 6906, 7708, 8518, 9334, 10158, 10988, 
    11826, 12670, 13522, 14380, 15246, 16118, 16998, 17884, 18778, 19678, 
    20586, 21500, 22422, 23350, 24286, 25228, 26178, 27134, 28098, 29068, 
    30046, 31030, 32022, 250, 1253, 2263, 3280, 4304, 5335, 6373, 
    7418, 8470, 9529, 10595, 11668, 12748, 13835, 14929, 16030, 17138, 
    18253, 19375, 20504, 21640, 22783, 23933, 25090, 26254, 27425, 28603, 
    29788, 30980, 32179, 620, 3045, 4264, 5491, 6724, 7965, 9212, 
    10467, 11728, 12997, 14272, 15555, 16844, 18141, 19444, 20755, 22072, 
    23397, 24728, 26067, 27412, 28765, 30124, 31491, 100, 1484, 2875, 
    4273, 5678, 7090, 8509, 9935, 11368, 12808, 14255, 15709, 17170, 
    18638, 20113, 21595, 23084, 24580, 26083, 27593, 29110, 30634, 32165, 
    930, 2470, 4017, 5571, 7132, 8700, 10275, 11857, 13446, 15042, 
    16645, 18255, 19872, 21496, 24751, 26385, 28027, 29675, 31331, 230, 
    1904, 3585, 5273, 6968, 8670, 10379, 12095, 13818, 15548, 17285, 
    19029, 20780, 22538, 24303, 26075, 27854, 29640, 31433, 460, 2262, 
    4071, 5887, 7710, 9540, 11377, 13221, 15072, 16930, 18795, 20667, 
    22546, 24432, 26325, 28225, 30132, 32046, 1200, 3129, 5065, 7008, 
    8958, 10915, 12879, 14850, 16828, 18813, 20805, 22804, 24810, 26823, 
    28843, 30870, 130, 2165, 6242, 8287, 10340, 12399, 14466, 16539, 
    18620, 20707, 22802, 24903, 27012, 29127, 31250, 610, 2745, 4887, 
    7036, 9192, 11355, 13525, 15702, 17886, 20077, 22275, 24480, 26692, 
    28911, 31137, 600, 2838, 5083, 7335, 9594, 11860, 14133, 16413, 
    18700, 20994, 23295, 25603, 27918, 30240, 32569, 2140, 4486, 6812, 
    9118, 11404, 13670, 15916, 18142, 20348, 22534, 24700, 26846, 28972, 
    31078, 430, 2530, 4610, 8777, 10867, 12938, 14988, 17019, 19029, 
    21020, 22990, 24941, 26871, 28782, 30672, 32543, 1650, 3505, 5340, 
    7155, 8950, 10725, 12480, 14215, 15930, 17625, 19300, 20955, 22590, 
    24205, 25800, 27375, 28930, 30465, 31980, 720, 2208, 3676, 5124, 
    6552, 7960, 9371, 10785, 12202, 13622, 15045, 16471, 17900, 19332, 
    20767, 22205, 23646, 25090, 26537, 27987, 29440, 30896, 32355, 1050, 
    2516, 3985, 5457, 6932, 9889, 11374, 12863, 14354, 15849, 17346, 
    18847, 20350, 21857, 23366, 24879, 26394, 27913, 29434, 30959, 32486, 
    1250, 2784, 4322, 5862, 7406, 8952, 10502, 12054, 13610, 15168, 
    16730, 18294, 19862, 21432, 23006, 24582, 26162, 27744, 29330, 30918, 
    32510, 1340, 2941, 4545, 6152, 7762, 9375, 10991, 12610, 14232, 
    15857, 17485, 19116, 20750, 22387, 24027, 25670, 27316, 28965, 30617, 
    32272, 1160, 2819, 4481, 7812, 9484, 11160, 12838, 14520, 16204, 
    17892, 19582, 21276, 22972, 24672, 26374, 28080, 29788, 31500, 450, 
    2171, 3895, 5622, 7352, 9085, 10821, 12560, 14302, 16047, 17795, 
    19546, 21300, 23057, 24817, 26580, 28346, 30115, 31887, 900, 2684, 
    4471, 6261, 8054, 9850, 11649, 13451, 15256, 17064, 18875, 20689, 
    22503, 24317, 26131, 27945, 29759, 31573, 620, 2435, 4250, 6065, 
    7880, 9695, 11510, 13325, 16962, 18787, 20613, 22438, 24264, 26089, 
    27915, 29740, 31566, 620, 2442, 4264 
} ; 
    
    /* End of test values :  inpAngleROM[] */


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
static varsize angleCounter ;      /* Current 'angleCounter' pulled from test data */
static varsize *inpAngleCount ;    /* Array of 'angleCounter' test data values */
static varsize tonewheelTeeth ;    /* Number of teeth on the tonewheel */

static int t_run_test(int argc, const char *argv[] )
{    
    int loop_cnt;
    int iterations = 1; 
    int input_index;
    scanf("%d", &input_index);
    int inputs_count = sizeof(inpAngleROM) / sizeof(inpAngleROM[0]);
    if (input_index >= inputs_count) {
        printf("ERROR: input_index %d is out of range, max is %d\n", input_index, inputs_count-1);
        printf("stopping execution\n");
        asm volatile ("wfi");
    }
    // size_t		loop_cnt = tcdef->rec_iterations;

#if BMDEBUG
    char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks ( c )1998-1999\n"
        "Algorithm 2 : Angle-to-Time Conversion  Rev. 1.0E0 a2time00 \n" ; 
    char *szHeader = 
        "\n\n   angle  pulse          tdc   engine "
        "tooth  delta        fire   test\n"
        "   Count, DeltaT, TDC,   Time, Speed, "
        "Count, TimeAvg,     Time   counter\n" ; 
    char szDebug[100] ; 
#else
    varsize i ;
#endif /* BMDEBUG */

    static n_char szDataSize[40] ; 
    n_int isTableLooped = false ;    /* Input test data table looped */
    varsize window ; 
    static varsize pulseDeltaTime1 ;
    static varsize pulseDeltaTime2 ;
    static varsize pulseDeltaTime3 ;
    static varsize angle1 ;
    static varsize angle2 ;
    static varsize angle3 ; 
    static varsize angleCounterLast1 ;
    static varsize angleCounterLast2 ;
    static varsize angleCounterLast3 ; 
    static varsize toothCount1 ;
    static varsize toothCount2 ;
    static varsize toothCount3 ; 
    static varsize deltaTimeAccum1 ;
    static varsize deltaTimeAccum2 ;
    static varsize deltaTimeAccum3 ; 
    static varsize deltaTimeAvg1 ;
    static varsize deltaTimeAvg2 ;
    static varsize deltaTimeAvg3 ; 
    static varsize firingTime1 ;
    static varsize firingTime2 ;
    static varsize firingTime3 ; 
    static n_ulong tdcTime1 ;
    static n_ulong tdcTime2 ;
    static n_ulong tdcTime3 ; 
    static n_ulong engineSpeed1 ;
    static n_ulong engineSpeed2 ;
    static n_ulong engineSpeed3 ; 
    static n_ulong rotationTime1 ;
    static n_ulong rotationTime2 ;
    static n_ulong rotationTime3 ; 
    static n_int isTopDeadCenter1 ; /* TRUE/FALSE flag when TDC occurs */
    static n_int isTopDeadCenter2 ; /* TRUE/FALSE flag when TDC occurs */
    static n_int isTopDeadCenter3 ; /* TRUE/FALSE flag when TDC occurs */

    /* Unused */
    argc = argc ;
    argv = argv ;
    
    /*
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     *
     */    

    /* Variable initializations at t=0 */    
    toothCount1 = 0 ;       /* Don't know which pulse we start on */
    toothCount2 = 0 ; 
    toothCount3 = 0 ; 
    deltaTimeAccum1 = 0 ;   /* ...and haven't accumulated for filter... */
    deltaTimeAccum2 = 0 ; 
    deltaTimeAccum3 = 0 ; 
    deltaTimeAvg1 = 32767 ; /* ...and not gotten an average... */
    deltaTimeAvg2 = 32767 ; 
    deltaTimeAvg3 = 32767 ; 
    tdcTime1 = 0 ;          /* ...and don't know when TDC occurs */
    tdcTime2 = 0 ; 
    tdcTime3 = 0 ; 
    angleCounterLast1 = 0 ; 
    angleCounterLast2 = 0 ; 
    angleCounterLast3 = 0 ; 
    engineSpeed1 = 0 ; 
    engineSpeed2 = 0 ; 
    engineSpeed3 = 0 ; 
    rotationTime1 = 0 ; 
    rotationTime2 = 0 ; 
    rotationTime3 = 0 ; 
    firingTime1 = 0 ; 
    firingTime2 = 0 ; 
    firingTime3 = 0 ; 
/* ALGO_GLOBALS */
    tableCount		= 0;
	angleCounter	= 0;   /* Current 'angleCounter' pulled from  data */
	inpAngleCount	= NULL; /* Array of 'angleCounter' test data values */
	tonewheelTeeth	= 60; /* Number of teeth on the tonewheel */


    /* Tell us the compiled data size */    
#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", 
        ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = (n_char)( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */

    // /* Initialize the test data -- stimuli for the algorithm. */    
    // /* Allocate for the test input data table */
    // if( !GetTestData() ) 
    // {
    //     th_exit( "Cannot Allocate Memory %s : %d", 
    //         __FILE__, __LINE__ ) ; 
    // }

#if BMDEBUG
    DebugOut( szTitle ) ;    /* Print the title message in RAM file */
    DebugOut( szDataSize ) ; /*  and the data size */
    DebugOut( szHeader ) ;   /* Print the test output file header in RAM file */
#endif     /* BMDEBUG */

    window = TENTH_DEGREES / tonewheelTeeth ; /* Only need to do this once */

    /* This is the actual benchmark algorithm. */    

    for( loop_cnt = 0 ; loop_cnt < iterations ; loop_cnt++ ) /* no stopping! */
    {

#if BMDEBUG
        if( isTableLooped )
        {
            DebugOut( "END--END--END\n" ) ;    /* Flag end-of-testfile */
        }
#endif /* BMDEBUG */

        /***********************************************************************
            First Pass                                                          
        ***********************************************************************/

        /* Gets 'angleCounter' value from test data */
        // isTableLooped = GetInputValues() ;
        isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
        angleCounter = inpAngleROM[input_index];

#if BMDEBUG
        th_sprintf( szDebug, "%8ld", (n_long)angleCounter ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Compute 'pulseDeltaTime' -- the difference in counter 
         * readings from the last pulse to the current one.  Note that
         * the realtime counter may roll over, so the elapsed time may
         * need to be corrected when this occurs.
         *
         */    
        if( angleCounterLast1 > angleCounter ) 
        {
            pulseDeltaTime1 = angleCounter + 
                ( (varsize)MAX_VARIABLE - angleCounterLast1 + 1 ) ; 
        }
        else
        {
            pulseDeltaTime1 = angleCounter - angleCounterLast1 ; 
        }

        /* Update timer history... */
        angleCounterLast1 = angleCounter ;
        /* ...and elapsed time for a revolution */
        rotationTime1 += pulseDeltaTime1 ; 

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld", (n_long)pulseDeltaTime1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Must detect TDC reference by noticing that the period between this
         * pulse and the last one is two or three times normal.  We'll set a
         * flag when TDC reference occurs, and clear it on all other pulses.
         * We also keep count of which pulse we're on relative to TDC reference.
         *
         */    

        if( pulseDeltaTime1 > ( TDC_TEETH *deltaTimeAvg1 *TDC_MARGIN ) )
        {
            isTopDeadCenter1 = true ; 
            pulseDeltaTime1 /= TDC_TEETH ; 
    
            /*
             * Compute engine speed every TDC.  Engine speed will be
             * the inverse of 'tdcTime', which is the period( in CPU
             * time )between TDC's.  Engine speed is also scaled by
             * an arbitrary constant to make it useful elsewhere in
             * the engine controller.
             *
             */    
            tdcTime1 = rotationTime1 ; 
            rotationTime1 = 0 ; 
            engineSpeed1 = RPM_SCALE_FACTOR / tdcTime1 ; 
            toothCount1 = 0 ; 
        }
        else 
        {
            toothCount1++ ; 
            isTopDeadCenter1 = false ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4d, %8ld, %5ld, %4ld", ( isTopDeadCenter1 & 1 ),
            (n_long)tdcTime1, (n_long)engineSpeed1, (n_long)toothCount1 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( (varsize)engineSpeed1 ) ; 
#endif /* BMDEBUG */

        /*
         * Compute an average delta-T for TDC pulse discrimination.
         * The delta-T will be filtered by averaging over the period
         * of one cylinder( several pulses ).
         *
         */    

        deltaTimeAccum1 += pulseDeltaTime1 ; 
        if( ( toothCount1 > 0 ) && 
            ( toothCount1 %( tonewheelTeeth / CYLINDERS ) == 0 ) )
        {        
            deltaTimeAvg1 = deltaTimeAccum1 / ( tonewheelTeeth / CYLINDERS ) ; 
            deltaTimeAccum1 = 0 ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, ", (n_long)deltaTimeAvg1 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        angle1 = ( TENTH_DEGREES * toothCount1 / tonewheelTeeth ) ; 

        /*
         * Now, output a value for the firing angle timer( a one-shot )
         * only if we're on the tooth which precedes the firing
         * angle for one of the cylinders.  We presume that there is
         * always a tooth which precedes each cylinder's firing angle.
         * The value which is output presumably goes to a "capture/compare"
         * timer which generates an interrupt used to fire that cylinder.
         * Note the special treatment for the last cylinder( #4, #6, or #8 ), 
         * we don't subtract the 'angle' because the 360th degree of rotation
         * is the same as the 0th degree of the next rotation.
         *
         */    

        /* CYLINDER 1 */    
        if( ( angle1 >= ( ( CYL1 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle1 < ( CYL1 * TENTH_DEGREES / CYLINDERS ) ) )
        {
            firingTime1 = 
                ( ( FIRE1_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 2 */    
        if( ( angle1 >= ( ( CYL2 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle1 < ( CYL2 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE2_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 3 */    
        if( ( angle1 >= ( ( CYL3 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle1 < ( CYL3 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE3_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 4 */    
        if( ( angle1 >= ( ( CYL4 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle1 < ( CYL4 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE4_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

#if( CYLINDERS > 4 )

        /* CYLINDER 5 */    
        if( ( angle1 >= ( ( CYL5 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle1 < ( CYL5 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE5_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 6 */    
        if( ( angle1 >= ( ( CYL6 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle1 < ( CYL6 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE6_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

#if( CYLINDERS > 6 )

        /* CYLINDER 7 */    
        if( ( angle1 >= ( ( CYL7 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle1 < ( CYL7 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE7_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 8 */    
        if( ( angle1 >= ( ( CYL8 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle1 < ( CYL8 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime1 = 
                ( ( FIRE8_ANGLE - angle1 ) * tdcTime1 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

#endif /* 6 cylinders */
#endif /* 4 cylinders */

        if( firingTime1 > MAX_VARIABLE )
        {
            firingTime1 -= MAX_VARIABLE ; 
        }

        /* Output the 'firingTime result */    

#if BMDEBUG
        th_sprintf( szDebug, " %10ld\n", (n_long)firingTime1 ) ; 
        DebugOut( szDebug ) ; 
#else      
        WriteOut( (varsize)firingTime1 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/
        /* Gets 'angleCounter' value from test data*/
        // isTableLooped += GetInputValues() ;
        isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
        angleCounter = inpAngleROM[input_index];

#if BMDEBUG
        /* Output some debug info, if needed */
        th_sprintf( szDebug, "%8ld", (n_long)angleCounter ) ;
        DebugOut( szDebug ) ; 
#endif     /* BMDEBUG */


        /*
         * Compute 'pulseDeltaTime' -- the difference in counter 
         * readings from the last pulse to the current one.  Note that
         * the realtime counter may roll over, so the elapsed time may
         * need to be corrected when this occurs.
         *
         */    

        if( angleCounterLast2 > angleCounter )
        {
            pulseDeltaTime2 = angleCounter +
                ( (varsize)MAX_VARIABLE - angleCounterLast2 + 2 ) ; 
        }
        else
        {
            pulseDeltaTime2 = angleCounter - angleCounterLast2 ; 
        }

        /* Update timer history... */
        angleCounterLast2 = angleCounter ;    
        /* ...and elapsed time for a revolution */
        rotationTime2 += pulseDeltaTime2 ;    

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld", (n_long)pulseDeltaTime2 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Must detect TDC reference by noticing that the period between this
         * pulse and the last one is two or three times normal.  We'll set a
         * flag when TDC reference occurs, and clear it on all other pulses.
         * We also keep count of which pulse we're on relative to TDC reference.
         *
         */    

        if( pulseDeltaTime2 > ( TDC_TEETH * deltaTimeAvg2 * TDC_MARGIN ) )
        {        
            isTopDeadCenter2 = true ; 
            pulseDeltaTime2 /= TDC_TEETH ; 

            /*
             * Compute engine speed every TDC.  Engine speed will be
             * the inverse of 'tdcTime', which is the period( in CPU
             * time )between TDC's.  Engine speed is also scaled by
             * an arbitrary constant to make it useful elsewhere in
             * the engine controller.
             *
             */    


            tdcTime2 = rotationTime2 ; 
            rotationTime2 = 0 ; 
            engineSpeed2 = RPM_SCALE_FACTOR / tdcTime2 ; 
            toothCount2 = 0 ; 
        }
        else {        
            toothCount2++ ; 
            isTopDeadCenter2 = false ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4d, %8ld, %5ld, %4ld", ( isTopDeadCenter2 & 1 ),
            (n_long)tdcTime2, (n_long)engineSpeed2, (n_long)toothCount2 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( (varsize)engineSpeed2 ) ; 
#endif /* BMDEBUG */

        /*
         * Compute an average delta-T for TDC pulse discrimination.
         * The delta-T will be filtered by averaging over the period
         * of one cylinder( several pulses ).
         *
         */    
        deltaTimeAccum2 += pulseDeltaTime2 ; 

        if( ( toothCount2 > 0 ) && 
            ( toothCount2 % ( tonewheelTeeth / CYLINDERS ) == 0 ) )
        {        
            deltaTimeAvg2 = deltaTimeAccum2 / ( tonewheelTeeth / CYLINDERS ) ; 
            deltaTimeAccum2 = 0 ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, ", (n_long)deltaTimeAvg2 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        angle2 = ( TENTH_DEGREES * toothCount2 / tonewheelTeeth ) ; 

        /*
         * Now, output a value for the firing angle timer( a one-shot )
         * only if we're on the tooth which precedes the firing
         * angle for one of the cylinders.  We presume that there is
         * always a tooth which precedes each cylinder's firing angle.
         * The value which is output presumably goes to a "capture/compare"
         * timer which generates an interrupt used to fire that cylinder.
         * Note the special treatment for the last cylinder( #4, #6, or #8 ), 
         * we don't subtract the 'angle' because the 360th degree of rotation
         * is the same as the 0th degree of the next rotation.
         *
         */    

        /* CYLINDER 1 */    
        if( ( angle2 >= ( ( CYL1 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle2 < ( CYL1 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE1_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 2 */    
        if( ( angle2 >= ( ( CYL2 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL2 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE2_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 3 */    
        if( ( angle2 >= ( ( CYL3 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL3 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE3_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 4 */    
        if( ( angle2 >= ( ( CYL4 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL4 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE4_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

#if( CYLINDERS > 4 )

        /* CYLINDER 5 */    
        if( ( angle2 >= ( ( CYL5 * TENTH_DEGREES / CYLINDERS )- window ) ) &&
            ( angle2 < ( CYL5 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE5_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 6 */    
        if( ( angle2 >= ( ( CYL6 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL6 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE6_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) +
                angleCounter ; 
        }

#if( CYLINDERS > 6 )

        /* CYLINDER 7 */    
        if( ( angle2 >= ( ( CYL7 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL7 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE7_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) +
                angleCounter ; 
        }

        /* CYLINDER 8 */    
        if( ( angle2 >= ( ( CYL8 * TENTH_DEGREES / CYLINDERS ) - window ) ) &&
            ( angle2 < ( CYL8 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime2 = 
                ( ( FIRE8_ANGLE - angle2 ) * tdcTime2 / TENTH_DEGREES ) +
                angleCounter ; 
        }

#endif /* 6 cylinders */
#endif /* 4 cylinders */

        if( firingTime2 > MAX_VARIABLE )
        {
            firingTime2 -= MAX_VARIABLE ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, " %10ld\n", (n_long)firingTime2 ) ; 
        DebugOut( szDebug ) ; 
#else 
        WriteOut( (varsize)firingTime2 ) ; 
#endif /* BMDEBUG */

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/
        /* Gets 'angleCounter' value from test data*/
        // isTableLooped += GetInputValues() ;
        isTableLooped = loop_cnt >= sizeof(inpAngleROM)/sizeof(inpAngleROM[0]);
        angleCounter = inpAngleROM[input_index];

#if BMDEBUG
        th_sprintf( szDebug, "%8ld", (n_long)angleCounter ) ;
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Compute 'pulseDeltaTime' -- the difference in counter 
         * readings from the last pulse to the current one.  Note that
         * the realtime counter may roll over, so the elapsed time may
         * need to be corrected when this occurs.
         *
         */    

        if( angleCounterLast3 > angleCounter )
        {
            pulseDeltaTime3 = angleCounter +
                ( (varsize)MAX_VARIABLE - angleCounterLast3 + 3 ) ; 
        }
        else
        {
            pulseDeltaTime3 = angleCounter - angleCounterLast3 ; 
        }
        /* Update timer history */
        angleCounterLast3 = angleCounter ;    
        /* and elapsed time for a revolution */
        rotationTime3 += pulseDeltaTime3 ;    

#if BMDEBUG
        th_sprintf( szDebug, ", %4ld", (n_long)pulseDeltaTime3 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /*
         * Must detect TDC reference by noticing that the period between this
         * pulse and the last one is two or three times normal.  We'll set a
         * flag when TDC reference occurs, and clear it on all other pulses.
         * We also keep count of which pulse we're on relative to TDC reference.
         *
         */    

        if( pulseDeltaTime3 > ( TDC_TEETH * deltaTimeAvg3 * TDC_MARGIN ) )
        {        
            isTopDeadCenter3 = true ; 
            pulseDeltaTime3 /= TDC_TEETH ; 

            /*
             * Compute engine speed every TDC.  Engine speed will be
             * the inverse of 'tdcTime', which is the period( in CPU
             * time )between TDC's.  Engine speed is also scaled by
             * an arbitrary constant to make it useful elsewhere in
             * the engine controller.
             *
             */    

            tdcTime3 = rotationTime3 ; 
            rotationTime3 = 0 ; 
            engineSpeed3 = RPM_SCALE_FACTOR / tdcTime3 ; 
            toothCount3 = 0 ; 
        }
        else 
        {        
            toothCount3++ ; 
            isTopDeadCenter3 = false ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %4d, %8ld, %5ld, %4ld", ( isTopDeadCenter3 & 1 ),
            (n_long)tdcTime3, (n_long)engineSpeed3, (n_long)toothCount3 ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( (varsize)engineSpeed3 ) ; 
#endif /* BMDEBUG */

        /*
         * Compute an average delta-T for TDC pulse discrimination.
         * The delta-T will be filtered by averaging over the period
         * of one cylinder( several pulses ).
         *
         */    

        deltaTimeAccum3 += pulseDeltaTime3 ; 

        if( ( toothCount3 > 0 ) && 
            ( toothCount3 % ( tonewheelTeeth / CYLINDERS ) == 0 ) )
        {        
            deltaTimeAvg3 = deltaTimeAccum3 / ( tonewheelTeeth / CYLINDERS ) ; 
            deltaTimeAccum3 = 0 ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %6ld, ", (n_long)deltaTimeAvg3 ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        angle3 = ( TENTH_DEGREES * toothCount3 / tonewheelTeeth ) ; 

        /*
         * Now, output a value for the firing angle timer( a one-shot )
         * only if we're on the tooth which precedes the firing
         * angle for one of the cylinders.  We presume that there is
         * always a tooth which precedes each cylinder's firing angle.
         * The value which is output presumably goes to a "capture/compare"
         * timer which generates an interrupt used to fire that cylinder.
         * Note the special treatment for the last cylinder( #4, #6, or #8 ), 
         * we don't subtract the 'angle' because the 360th degree of rotation
         * is the same as the 0th degree of the next rotation.
         *
         */    

        /* CYLINDER 1 */    
        if( ( angle3 >= ( ( CYL1 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL1 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE1_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 2 */    
        if( ( angle3 >= ( ( CYL2 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL2 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE2_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 3 */    
        if( ( angle3 >= ( ( CYL3 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 <( CYL3 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE3_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 4 */    
        if( ( angle3 >= ( ( CYL4 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL4 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE4_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

#if( CYLINDERS > 4 )

        /* CYLINDER 5 */    
        if( ( angle3 >= ( ( CYL5 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL5 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE5_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 6 */    
        if( ( angle3 >= ( ( CYL6 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL6 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE6_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

#if( CYLINDERS > 6 )

        /* CYLINDER 7 */    
        if( ( angle3 >= ( ( CYL7 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 < ( CYL7 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE7_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES ) + 
                angleCounter ; 
        }

        /* CYLINDER 8 */    
        if( ( angle3 >= ( ( CYL8 * TENTH_DEGREES / CYLINDERS ) - window ) ) && 
            ( angle3 <( CYL8 * TENTH_DEGREES / CYLINDERS ) ) )
        {        
            firingTime3 = 
                ( ( FIRE8_ANGLE - angle3 ) * tdcTime3 / TENTH_DEGREES  )+ 
                angleCounter ; 
        }

#endif /* 6 cylinders */
#endif /* 4 cylinders */

        if( firingTime3 > MAX_VARIABLE )
        {
            firingTime3 -= MAX_VARIABLE ; 
        }

#if BMDEBUG
        th_sprintf( szDebug, " %10ld", (n_long)firingTime3 ) ; 
        DebugOut( szDebug ) ; 
        th_sprintf( szDebug, ", %08lX\n", ( n_ulong )loop_cnt ) ; 
        DebugOut( szDebug ) ; 
#else
        WriteOut( (varsize)firingTime3 ) ; 

#if DATA_SIZE == 0 /* Might break up the loop counter */
        i = (varsize)( loop_cnt &0x0000FFFF ) ; 
        WriteOut( i ) ; /*  ...in the output file */
        i = (varsize)( loop_cnt >> 16 ) ; 
        WriteOut( i ) ; 
#else
        WriteOut( (varsize)loop_cnt ) ; 
#endif
        i = (varsize)0xAAAA ; 
        WriteOut( i ) ; /* Flag the end of data-block */
#endif /* BMDEBUG */

#if BMDEBUG
        if( !th_harness_poll() )
        {
            break ; 
        }
#endif
    }


// #if NON_INTRUSIVE_CRC_CHECK
// 	tcdef->CRC=0;
// /* varsize is n_short or n_long, calc crc based on e_u32 */
// /* final answers are iteration dependent */
// 	tcdef->CRC = Calc_crc32((e_u32)*inpAngleCount,tcdef->CRC ) ; 
// 	tcdef->CRC = Calc_crc32((e_u32)tonewheelTeeth,tcdef->CRC ) ; 
// #elif	CRC_CHECK
// 	tcdef->CRC=0;
// #else
// 	tcdef->CRC=0;
// #endif

	// return	th_report_results(tcdef,EXPECTED_CRC);

    return 0;
} /* End of function 't_run_test' */

