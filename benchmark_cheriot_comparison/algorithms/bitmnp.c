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

- implement GetInputValues or replace each call to GetInputValues with something like:

    isTableLooped = loop_cnt >= sizeof(inpROM)/sizeof(inpROM[0]);
    angleCounter = inpROM[input_index]; //    angleCounter = inpROM[loop_cnt % (sizeof(inpROM)/sizeof(inpROM[0]))];

*/

// originally file.h was "algo.h"
#include "bitmnp.h" // change to the name of the main file (eg. "a2time.h")

// forward declarations
static int t_run_test( int argc, const char *argv[] );
// n_int GetTestData(n_void);
static n_int GetInputValues(n_void);

// function to be called from a separate main file
int bitmnp(int argc, const char *argv[]) {
    return t_run_test(argc, argv);
}

/* Estimate of allocation for NUM_TESTS*( debug test + 2 variables )*/
#define T_BSIZE (MAX_FILESIZE+((NUM_TESTS+1)*VAR_COUNT*4))

// input data (inpROM)

static const varsize inpVariableROM[] = {
    1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 
    0, 6, 0, 7, 0, 8, 0, 9, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 
    0, 0, 0, 1, 0, 2, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 
    10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 
    10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 1, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 
    1, 9, 1, 0, 1, 1, 1, 2, 1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 0, 1, 1, 1, 2, 
    1, 3, 1, 4, 1, 5, 1, 6, 1, 7, 1, 8, 1, 9, 1, 0, 1, 1, 1, 2, 1, 10, 1, 10, 1, 10, 1, 10, 1, 
    10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 
    10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1 
} ; /* End of test values :  inpVariableROM[] */

static varsize digitROM[] = {
    0xFE, 0xFE, 0xFE, 0xFE, 0xFE, /* Solid character for ERROR */
    0x7C, 0xA2, 0x92, 0x8A, 0x7C, /* Character '0' */
    0x00, 0x82, 0xFE, 0x80, 0x00, /* Character '1' */
    0x84, 0xC2, 0xA2, 0x92, 0x8C, /* Character '2' */
    0x42, 0x82, 0x8A, 0x96, 0x62, /* Character '3' */
    0x30, 0x28, 0x24, 0xFE, 0x20, /* Character '4' */
    0x4E, 0x8A, 0x8A, 0x8A, 0x72, /* Character '5' */
    0x78, 0x94, 0x92, 0x92, 0x60, /* Character '6' */
    0x02, 0x02, 0xE2, 0x1A, 0x06, /* Character '7' */ 
    0x6C, 0x92, 0x92, 0x92, 0x6C, /* Character '8' */ 
    0x0C, 0x92, 0x92, 0x52, 0x3C, /* Character '9' */ 
    0x00, 0x00, 0x00, 0x00, 0x00 /* Blank character */

}; /* End of variable 'charset[]' */


// #ifndef RANDOM_FUNCTION_GENERATOR
// int main(void) {
//     t_run_test(0, NULL);

//     // stop the program
//     asm("wfi");
//     return 0;
// }
// #endif

/*  DECLARATIONS */    
/* Input stimuli test data table */
// extern const varsize inpVariableROM[] ;
// extern const int digitROM[] ;

static n_int   *RAMfile ;          /* Pointer to test output RAM file */
static n_int   *RAMfilePtr ;       /* Pointer to position in output RAM file */
static n_int   RAMfileSize ;       /* Size of the debug output RAM file */
static n_int   tableCount ;        /* Number of passes through table */
static n_int   *RAMfileEOF;        /* points to end of RAM file */
static n_int   RAMfile_increment;  /* difference between varsize and n_int */

static varsize *inpNumber = inpVariableROM;    /* Pointer to array of numbers to paint */
static n_int   *inpMode ;      /* Pointer to array of painting modes */
static varsize inputNum;      /* The input argument for computation */
static n_int   inverted ;      /* Paint 'inverted' character in display */

static int input_index;

static int t_run_test(int argc, const char *argv[] )
{    
    int iterations = argc > 1 ? atoi(argv[1]) : 1; 
    // inpVariableROM data is:
    // - 1 value for "inputNum"
    // - 1 value for "inverted"  (both these are repeated)
    // so there needs to be "/2" in the calculation below 
    // (because GetInputValues reads 2 values at a time and uses "*2" multiplication of input index)
    input_index = atoi(argv[0]);
    int inputs_count = sizeof(inpVariableROM) / sizeof(inpVariableROM[0]) / 2; 
    if (input_index >= inputs_count)
        th_exit("ERROR: input_index %d is out of range, max is %d\nStopping execution.\n", input_index, inputs_count-1);


#if BMDEBUG
    n_char *szTitle = 
        "\nEEMBC Automotive/Industrial Subcommittee Benchmarks (c)1998-1999\n"
        "Algorithm 8 :  Bit Manipulation  Rev. 1.0C0 - bitmnp01\n" ; 
    n_char *szHeader = 
        "\n\ninputNum, inverted" "\n            display[0]...display[4]"
        "\n               |            | "
        "\n            display[32nd character], counter\n" ; 
    n_char szDebug[100] ; 
#endif /* BMDEBUG */
    n_char szDataSize[40] ;
    /* Input test data table looped */ 
    n_int isTableLooped = false ;
    /* Temporary pointer to 'inputStrint' */
    n_char *strInput ;    
    static varsize i1 ;
    static varsize i2 ;
    static varsize i3 ;
    static varsize j1 ;
    static varsize j2 ;
    static varsize j3 ; 
    /* Which character in the output display we are painting */
    static int digit1 ;
    static int digit2 ;
    static int digit3 ;
    /* Result array where the character is bit-mapped */
    static varsize display1[MAX_COLUMNS + 1] ;
    static varsize display2[MAX_COLUMNS + 1] ; 
    static varsize display3[MAX_COLUMNS + 1] ; 
    /* Pointer to a character's bitmap */
    static varsize *charset1 ;
    static varsize *charset2 ;
    static varsize *charset3 ;
    /* Array to hold the input BCD string */
    static varsize inputString1[INPUT_CHARS + 1] ;
    static varsize inputString2[INPUT_CHARS + 1] ; 
    static varsize inputString3[INPUT_CHARS + 1] ; 

    /* Unused */
    argc = argc ;
    argv = argv ;

    /*
     * First, initialize the data structures we need for the test
     * and allocate memory as needed.  Report an error if we can't.
     *
     */    

    /* Variable initializations at t=0 */    

    RAMfilePtr = 0 ;   /* Point to beginning of test output text file */
    tableCount = 0 ;    /* Start out at beginning of input test data */
    digit1 = 0 ; 
    digit2 = 0 ; 
    digit3 = 0 ; 

    /* Must clear out the input string( s )before using it */    

    strInput = (n_char*)inputString1 ; 
    for( i1 = 0 ; i1 < ( INPUT_CHARS / 2 ) ; i1++ )
    {        
        *strInput++ = (n_char)0xAA ; 
    }

    strInput = (n_char*)inputString2 ; 
    for( i2 = 0 ; i2 < ( INPUT_CHARS / 2 ) ; i2++ )
    {        
        *strInput++ = (n_char)0xAA ; 
    }

    strInput = (n_char*)inputString3 ; 
    for( i3 = 0 ; i3 < ( INPUT_CHARS / 2 ) ; i3++ )
    {        
        *strInput++ = (n_char)0xAA ; 
    }

    /* Tell us the compiled data size */    
#if BMDEBUG
    th_sprintf( szDataSize, "Data size = %s\n\n", 
        ( DATA_SIZE ? "LONG" : "SHORT" ) ) ; 
#else
    szDataSize[0] = (n_char)( '0' + DATA_SIZE ) ; 
    szDataSize[1] = '\0' ; 
#endif /* BMDEBUG */

    /* Initialize the test data -- stimuli for the algorithm. */    

    // if( !GetTestData() ) 
    // {
    //     /* Allocate for the test input data table */
    //     th_exit( "Cannot Allocate Memory %s : %d",
    //         __FILE__, __LINE__ ) ; 
    // }

#if BMDEBUG
    DebugOut( szTitle ) ;    /* Print the title message in RAM file */
    DebugOut( szDataSize ) ; /*  and the data size */
    DebugOut( szHeader ) ;   /* Print the test output file hdr in RAM file */
#endif /* BMDEBUG */

    /* This is the actual benchmark algorithm. */    

    /* The Bit Manipulation algorithm presumes that we are receiving input
     * characters in BCD format( four bits )that must be stored in a FIFO
     * and then converted to dot-matrix bitmapped characters on a display.
     *
     * The steps are as follows : 
     *
     * 1.  Get a new character
     * 2.  Shift it into the bottom of a 32-character string of BCD digits.
     * 3.  Pull out each of the 32 digits from the string, starting from the
     *     bottom.
     * 4.  Find the digit in the character set.
     * 5.  Paint the selected character into the display memory (double-high).
     * 6.  Mirror-image the display memory top to bottom, bit by bit.
     * 7.  Go back to( 3 )until all 32 characters are done.
     * 8.  Do it all again from( 1 ).
     *
     * The algorithm is structured so that the bit manipulations could
     * be optimized down to BIT instructions, if available, but are
     * otherwise done in ordinary AND's and OR's. There's also plenty
     * of shifting, done four bits at a time, or multiples thereof.
     *
     */

    //   tcdef->CRC = 0;
    // th_signal_start() ;

    for( int loop_cnt = 0 ; loop_cnt < iterations  ; loop_cnt++ )
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
        th_sprintf( szDebug, "%6ld, %2d", ( long )inputNum, inverted ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Take the new input digit, and slide it into the input string */
        /* ...one nibble at a time, depending on the size of data */    
        
        for( i1 = ((INPUT_CHARS/sizeof(varsize)/2)-1) ; i1 > 0 ; i1-- )
        {        
            inputString1[i1] <<= BITS_PER_DIGIT ; 
            j1 = inputString1[i1 - 1] >> 
                ( sizeof( varsize ) * BITS_PER_BYTE - 4 ) ; 
            inputString1[i1] |= j1 ; 
        }
        /* Don't forget the last nibble */
        inputString1[i1] <<= BITS_PER_DIGIT ;
        /* And pick up the new digit */
        inputString1[i1] |= inputNum ;

        /* Now, copy each digit to the display */    
        for( digit1 = 0 ; digit1 < INPUT_CHARS ; digit1++ )
        {        
            /* Get each digit in the input string */    
            inputNum = inputString1[digit1 / ( sizeof( varsize ) * 2 )] ; 
            j1 = BITS_PER_DIGIT * ( digit1 % ( sizeof( varsize ) * 2 ) ) ; 

            /* Shift down the remainder of bits( if req'd. )*/
            if( j1 > 0 )
            {
                inputNum >>= j1 ; 
            }

            /* Get just one digit to paint */
            inputNum &= DIGIT_MASK ;

            /* Find the digit's bitmap */    
            switch( inputNum )
            {
            /* For the numeric digits... */
            case 0 : 
            case 1 : 
            case 2 : 
            case 3 : 
            case 4 : 
            case 5 : 
            case 6 : 
            case 7 : 
            case 8 : 
            case 9 : 
                charset1 = (varsize*)
                    &digitROM + ( ( inputNum + 1 ) * CHAR_COLUMNS ) ; 
                break ;

            case BLANK : 
                charset1 = (varsize*)&digitROM + BLANK_CHAR;
                break ;

            default :
                charset1 = (varsize*)&digitROM + ERROR_CHAR; 
                break ;
            }

            /* Paint the digit in the display */    
            for( i1 = 0 ; i1 < CHAR_COLUMNS ; i1++ )
            {        
                j1 = digit1 * CHAR_COLUMNS ; 

                /* First, clear out the column in display */
                display1[i1 + j1] = 0 ;

                if( !inverted )
                {
                    /* Normal orientation, not 'inverted' */
                    if( ( charset1[i1] &BIT_0 ) == 0 )
                    {            
                        /* Not a 'descender' */
                        if( BIT_1 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_1 ;
                            display1[i1 + j1] |= DOT_8 ;
                        }
                        if( BIT_2 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_2 ;
                            display1[i1 + j1] |= DOT_9 ;
                        }
                        if( BIT_3 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_3 ; 
                            display1[i1 + j1] |= DOT_10 ;
                        }
                        if( BIT_4 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_4 ; 
                            display1[i1 + j1] |= DOT_11 ;
                        }
                        if( BIT_5 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_5 ; 
                            display1[i1 + j1] |= DOT_12 ;
                        }
                        if( BIT_6 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_6 ; 
                            display1[i1 + j1] |= DOT_13 ;
                        }
                        if( BIT_7 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_7 ; 
                            display1[i1 + j1] |= DOT_14 ;
                        }
                    } /* End of 'normal' orientation, not a 'descender' */
                    else
                    {
                        /* This is 'normal orientation, and 'descender' */
                        if( BIT_1 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_3 ; 
                            display1[i1 + j1] |= DOT_10 ;
                        }
                        if( BIT_2 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_4 ; 
                            display1[i1 + j1] |= DOT_11 ;
                        }
                        if( BIT_3 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_5 ; 
                            display1[i1 + j1] |= DOT_12 ;
                        }
                        if( BIT_4 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_6 ; 
                            display1[i1 + j1] |= DOT_13 ;
                        }
                        if( BIT_5 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_7 ; 
                            display1[i1 + j1] |= DOT_14 ;
                        }
                        if( BIT_6 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_8 ; 
                            display1[i1 + j1] |= DOT_15 ;
                        }
                        if( BIT_7 &charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_9 ; 
                            display1[i1 + j1] |= DOT_16 ;
                        }
                    }
                } /* End of 'normal' orientation' */
                else
                {
                    /* Else, paint 'inverted', not a 'descender' */
                    if( ( charset1[i1] & BIT_0 ) == 0 )
                    {        
                        if( BIT_1 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_9 ; 
                            display1[i1 + j1] |= DOT_16 ;
                        }
                        if( BIT_2 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_8 ; 
                            display1[i1 + j1] |= DOT_15 ;
                        }
                        if( BIT_3 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_7 ; 
                            display1[i1 + j1] |= DOT_14 ;
                        }
                        if( BIT_4 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_6 ; 
                            display1[i1 + j1] |= DOT_13 ;
                        }
                        if( BIT_5 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_5 ; 
                            display1[i1 + j1] |= DOT_12 ;
                        }
                        if( BIT_6 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_4 ; 
                            display1[i1 + j1] |= DOT_11 ;
                        }
                        if( BIT_7 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_3 ; 
                            display1[i1 + j1] |= DOT_10 ;
                        }
                    } /* End of 'inverted' orientation, not a 'descender' */
                    else
                    {
                        /* Else, it's 'inverted', and 'descender' */
                        if( BIT_1 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_7 ; 
                            display1[i1 + j1] |= DOT_14 ;
                        }
                        if( BIT_2 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_6 ; 
                            display1[i1 + j1] |= DOT_13 ;
                        }
                        if( BIT_3 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_5 ; 
                            display1[i1 + j1] |= DOT_12 ;
                        }
                        if( BIT_4 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_4 ; 
                            display1[i1 + j1] |= DOT_11 ;
                        }
                        if( BIT_5 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_3 ; 
                            display1[i1 + j1] |= DOT_10 ;
                        }
                        if( BIT_6 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_2 ; 
                            display1[i1 + j1] |= DOT_9 ;
                        }
                        if( BIT_7 & charset1[i1] )
                        {        
                            display1[i1 + j1] |= DOT_1 ; 
                            display1[i1 + j1] |= DOT_8 ; 
                        }
                    } /* End of 'inverted' and 'descender' */
                } /* End of 'inverted' painting */
            } /* End of all the 'columns' of character */
            
            /* Now, copy the low-order bits to the high-order bits, in
             * reverse order */
            for( i1 = 0 ; i1 < CHAR_COLUMNS ; i1++ )
            {        
                if( display1[i1 + j1] & BIT_0 )
                {
                    display1[i1 + j1] |= DOT_32 ; 
                }
                if( display1[i1 + j1] & BIT_1 )
                {
                    display1[i1 + j1] |= DOT_31 ; 
                }
                if( display1[i1 + j1] & BIT_2 )
                {
                    display1[i1 + j1] |= DOT_30 ; 
                }
                if( display1[i1 + j1] & BIT_3 )
                {
                    display1[i1 + j1] |= DOT_29 ; 
                }
                if( display1[i1 + j1] & BIT_4 )
                {
                    display1[i1 + j1] |= DOT_28 ; 
                }
                if( display1[i1 + j1] & BIT_5 )
                {
                    display1[i1 + j1] |= DOT_27 ; 
                }
                if( display1[i1 + j1] & BIT_6 )
                {
                    display1[i1 + j1] |= DOT_26 ; 
                }
                if( display1[i1 + j1] & BIT_7 )
                {
                    display1[i1 + j1] |= DOT_25 ; 
                }
                if( display1[i1 + j1] & BIT_8 )
                {
                    display1[i1 + j1] |= DOT_24 ; 
                }
                if( display1[i1 + j1] & BIT_9 )
                {
                    display1[i1 + j1] |= DOT_23 ; 
                }
                if( display1[i1 + j1] & BIT_10 )
                {
                    display1[i1 + j1] |= DOT_22 ; 
                }
                if( display1[i1 + j1] & BIT_11 )
                {
                    display1[i1 + j1] |= DOT_21 ; 
                }
                if( display1[i1 + j1] & BIT_12 )
                {
                    display1[i1 + j1] |= DOT_20 ; 
                }
                if( display1[i1 + j1] & BIT_13 )
                {
                    display1[i1 + j1] |= DOT_19 ; 
                }
                if( display1[i1 + j1] & BIT_14 )
                {
                    display1[i1 + j1] |= DOT_18 ; 
                }
                if( display1[i1 + j1] & BIT_15 )
                {
                    display1[i1 + j1] |= DOT_17 ; 
                }
            } /* End of low-to-high bit reversal */
        } /* End of all 'digits' */

        for( i1 = 0 ; i1 < (INPUT_CHARS * CHAR_COLUMNS) ; i1 += CHAR_COLUMNS )
        {        
#if BMDEBUG
            th_sprintf( szDebug, 
                "\n            %08lX, %08lX, %08lX, %08lX, %08lX\n", 
                display1[i1], 
                display1[i1 + 1], 
                display1[i1 + 2], 
                display1[i1 + 3], 
                display1[i1 + 4] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( display1[i1] ) ; 
            WriteOut( display1[i1 + 1] ) ; 
            WriteOut( display1[i1 + 2] ) ; 
            WriteOut( display1[i1 + 3] ) ; 
            WriteOut( display1[i1 + 4] ) ; 
#endif /* BMDEBUG */
        }

        /***********************************************************************
            Second Pass                                                         
        ***********************************************************************/

        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%6ld, %2d", ( n_long )inputNum, inverted ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Take the new input digit, and slide it into the input string */
        /* ...one nibble at a time, depending on the size of data */    
        
        for( i2 = ((INPUT_CHARS/sizeof(varsize)/2)-1) ; i2 > 0 ; i2-- )
        {        
            inputString2[i2] <<= BITS_PER_DIGIT ; 
            j2 = inputString2[i2 - 1] >> 
                ( sizeof( varsize ) * BITS_PER_BYTE - 4 ) ; 
            inputString2[i2] |= j2 ; 
        }
        /* Don't forget the last nibble */
        inputString2[i2] <<= BITS_PER_DIGIT ;
        /* And pick up the new digit */
        inputString2[i2] |= inputNum ;

        /* Now, copy each digit to the display */    
        for( digit2 = 0 ; digit2 < INPUT_CHARS ; digit2++ )
        {        
            /* Get each digit in the input string */    
            inputNum = inputString2[digit2 / ( sizeof( varsize ) * 2 )] ; 
            j2 = BITS_PER_DIGIT * ( digit2 % ( sizeof( varsize ) * 2 ) ) ; 

            /* Shift down the remainder of bits( if req'd. )*/
            if( j2 > 0 )
            {
                inputNum >>= j2 ; 
            }

            /* Get just one digit to paint */
            inputNum &= DIGIT_MASK ;

            /* Find the digit's bitmap */    
            switch( inputNum )
            {
            /* For the numeric digits... */
            case 0 : 
            case 1 : 
            case 2 : 
            case 3 : 
            case 4 : 
            case 5 : 
            case 6 : 
            case 7 : 
            case 8 : 
            case 9 : 
                charset2 = (varsize*)
                    &digitROM + ( ( inputNum + 1 ) * CHAR_COLUMNS ) ; 
                break ;

            case BLANK : 
                charset2 = (varsize*)digitROM + BLANK_CHAR;
                break ;

            default :
                charset2 = (varsize*)&digitROM + ERROR_CHAR;
                break ;
            }

            /* Paint the digit in the display */    
            for( i2 = 0 ; i2 < CHAR_COLUMNS ; i2++ )
            {        
                j2 = digit2 * CHAR_COLUMNS ; 

                /* First, clear out the column in display */
                display2[i2 + j2] = 0 ;

                if( !inverted )
                {
                    /* Normal orientation, not 'inverted' */
                    if( ( charset2[i2] &BIT_0 ) == 0 )
                    {            
                        /* Not a 'descender' */
                        if( BIT_1 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_1 ;
                            display2[i2 + j2] |= DOT_8 ;
                        }
                        if( BIT_2 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_2 ;
                            display2[i2 + j2] |= DOT_9 ;
                        }
                        if( BIT_3 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_3 ; 
                            display2[i2 + j2] |= DOT_10 ;
                        }
                        if( BIT_4 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_4 ; 
                            display2[i2 + j2] |= DOT_11 ;
                        }
                        if( BIT_5 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_5 ; 
                            display2[i2 + j2] |= DOT_12 ;
                        }
                        if( BIT_6 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_6 ; 
                            display2[i2 + j2] |= DOT_13 ;
                        }
                        if( BIT_7 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_7 ; 
                            display2[i2 + j2] |= DOT_14 ;
                        }
                    } /* End of 'normal' orientation, not a 'descender' */
                    else
                    {
                        /* This is 'normal orientation, and 'descender' */
                        if( BIT_1 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_3 ; 
                            display2[i2 + j2] |= DOT_10 ;
                        }
                        if( BIT_2 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_4 ; 
                            display2[i2 + j2] |= DOT_11 ;
                        }
                        if( BIT_3 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_5 ; 
                            display2[i2 + j2] |= DOT_12 ;
                        }
                        if( BIT_4 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_6 ; 
                            display2[i2 + j2] |= DOT_13 ;
                        }
                        if( BIT_5 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_7 ; 
                            display2[i2 + j2] |= DOT_14 ;
                        }
                        if( BIT_6 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_8 ; 
                            display2[i2 + j2] |= DOT_15 ;
                        }
                        if( BIT_7 &charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_9 ; 
                            display2[i2 + j2] |= DOT_16 ;
                        }
                    }
                } /* End of 'normal' orientation' */
                else
                {
                    /* Else, paint 'inverted', not a 'descender' */
                    if( ( charset2[i2] & BIT_0 ) == 0 )
                    {        
                        if( BIT_1 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_9 ; 
                            display2[i2 + j2] |= DOT_16 ;
                        }
                        if( BIT_2 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_8 ; 
                            display2[i2 + j2] |= DOT_15 ;
                        }
                        if( BIT_3 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_7 ; 
                            display2[i2 + j2] |= DOT_14 ;
                        }
                        if( BIT_4 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_6 ; 
                            display2[i2 + j2] |= DOT_13 ;
                        }
                        if( BIT_5 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_5 ; 
                            display2[i2 + j2] |= DOT_12 ;
                        }
                        if( BIT_6 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_4 ; 
                            display2[i2 + j2] |= DOT_11 ;
                        }
                        if( BIT_7 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_3 ; 
                            display2[i2 + j2] |= DOT_10 ;
                        }
                    } /* End of 'inverted' orientation, not a 'descender' */
                    else
                    {
                        /* Else, it's 'inverted', and 'descender' */
                        if( BIT_1 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_7 ; 
                            display2[i2 + j2] |= DOT_14 ;
                        }
                        if( BIT_2 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_6 ; 
                            display2[i2 + j2] |= DOT_13 ;
                        }
                        if( BIT_3 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_5 ; 
                            display2[i2 + j2] |= DOT_12 ;
                        }
                        if( BIT_4 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_4 ; 
                            display2[i2 + j2] |= DOT_11 ;
                        }
                        if( BIT_5 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_3 ; 
                            display2[i2 + j2] |= DOT_10 ;
                        }
                        if( BIT_6 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_2 ; 
                            display2[i2 + j2] |= DOT_9 ;
                        }
                        if( BIT_7 & charset2[i2] )
                        {        
                            display2[i2 + j2] |= DOT_1 ; 
                            display2[i2 + j2] |= DOT_8 ; 
                        }
                    } /* End of 'inverted' and 'descender' */
                } /* End of 'inverted' painting */
            } /* End of all the 'columns' of character */
            
            /* Now, copy the low-order bits to the high-order bits, in
             * reverse order */
            for( i2 = 0 ; i2 < CHAR_COLUMNS ; i2++ )
            {        
                if( display2[i2 + j2] & BIT_0 )
                {
                    display2[i2 + j2] |= DOT_32 ; 
                }
                if( display2[i2 + j2] & BIT_1 )
                {
                    display2[i2 + j2] |= DOT_31 ; 
                }
                if( display2[i2 + j2] & BIT_2 )
                {
                    display2[i2 + j2] |= DOT_30 ; 
                }
                if( display2[i2 + j2] & BIT_3 )
                {
                    display2[i2 + j2] |= DOT_29 ; 
                }
                if( display2[i2 + j2] & BIT_4 )
                {
                    display2[i2 + j2] |= DOT_28 ; 
                }
                if( display2[i2 + j2] & BIT_5 )
                {
                    display2[i2 + j2] |= DOT_27 ; 
                }
                if( display2[i2 + j2] & BIT_6 )
                {
                    display2[i2 + j2] |= DOT_26 ; 
                }
                if( display2[i2 + j2] & BIT_7 )
                {
                    display2[i2 + j2] |= DOT_25 ; 
                }
                if( display2[i2 + j2] & BIT_8 )
                {
                    display2[i2 + j2] |= DOT_24 ; 
                }
                if( display2[i2 + j2] & BIT_9 )
                {
                    display2[i2 + j2] |= DOT_23 ; 
                }
                if( display2[i2 + j2] & BIT_10 )
                {
                    display2[i2 + j2] |= DOT_22 ; 
                }
                if( display2[i2 + j2] & BIT_11 )
                {
                    display2[i2 + j2] |= DOT_21 ; 
                }
                if( display2[i2 + j2] & BIT_12 )
                {
                    display2[i2 + j2] |= DOT_20 ; 
                }
                if( display2[i2 + j2] & BIT_13 )
                {
                    display2[i2 + j2] |= DOT_19 ; 
                }
                if( display2[i2 + j2] & BIT_14 )
                {
                    display2[i2 + j2] |= DOT_18 ; 
                }
                if( display2[i2 + j2] & BIT_15 )
                {
                    display2[i2 + j2] |= DOT_17 ; 
                }
            } /* End of low-to-high bit reversal */
        } /* End of all 'digits' */

        for( i2 = 0 ; i2 < (INPUT_CHARS * CHAR_COLUMNS) ; i2 += CHAR_COLUMNS )
        {        
#if BMDEBUG
            th_sprintf( szDebug, 
                "\n            %08lX, %08lX, %08lX, %08lX, %08lX\n", 
                display2[i2], 
                display2[i2 + 1], 
                display2[i2 + 2], 
                display2[i2 + 3], 
                display2[i2 + 4] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( display2[i2] ) ; 
            WriteOut( display2[i2 + 1] ) ; 
            WriteOut( display2[i2 + 2] ) ; 
            WriteOut( display2[i2 + 3] ) ; 
            WriteOut( display2[i2 + 4] ) ; 
#endif /* BMDEBUG */
        }

        /***********************************************************************
            Third Pass                                                          
        ***********************************************************************/

        isTableLooped += GetInputValues() ;

#if BMDEBUG
        th_sprintf( szDebug, "%6ld, %2d", ( n_long )inputNum, inverted ) ; 
        DebugOut( szDebug ) ; 
#endif /* BMDEBUG */

        /* Take the new input digit, and slide it into the input string */
        /* ...one nibble at a time, depending on the size of data */    
        
        for( i3 = ((INPUT_CHARS/sizeof(varsize)/2)-1) ; i3 > 0 ; i3-- )
        {        
            inputString3[i3] <<= BITS_PER_DIGIT ; 
            j3 = inputString3[i3 - 1] >> 
                ( sizeof( varsize ) * BITS_PER_BYTE - 4 ) ; 
            inputString3[i3] |= j3 ; 
        }
        /* Don't forget the last nibble */
        inputString3[i3] <<= BITS_PER_DIGIT ;
        /* And pick up the new digit */
        inputString3[i3] |= inputNum ;

        /* Now, copy each digit to the display */    
        for( digit3 = 0 ; digit3 < INPUT_CHARS ; digit3++ )
        {        
            /* Get each digit in the input string */    
            inputNum = inputString3[digit3 / ( sizeof( varsize ) * 2 )] ; 
            j3 = BITS_PER_DIGIT * ( digit3 % ( sizeof( varsize ) * 2 ) ) ; 

            /* Shift down the remainder of bits( if req'd. )*/
            if( j3 > 0 )
            {
                inputNum >>= j3 ; 
            }

            /* Get just one digit to paint */
            inputNum &= DIGIT_MASK ;

            /* Find the digit's bitmap */    
            switch( inputNum )
            {
            /* For the numeric digits... */
            case 0 : 
            case 1 : 
            case 2 : 
            case 3 : 
            case 4 : 
            case 5 : 
            case 6 : 
            case 7 : 
            case 8 : 
            case 9 : 
                charset3 = (varsize*)
                    &digitROM + ( ( inputNum + 1 ) * CHAR_COLUMNS ) ; 
                break ;

            case BLANK : 
                charset3 = (varsize*)&digitROM + BLANK_CHAR ;
                break ;

            default :
                charset3 = (varsize*)&digitROM + ERROR_CHAR ;
                break ;
            }

            /* Paint the digit in the display */    
            for( i3 = 0 ; i3 < CHAR_COLUMNS ; i3++ )
            {        
                j3 = digit3 * CHAR_COLUMNS ; 

                /* First, clear out the column in display */
                display3[i3 + j3] = 0 ;

                if( !inverted )
                {
                    /* Normal orientation, not 'inverted' */
                    if( ( charset3[i3] &BIT_0 ) == 0 )
                    {            
                        /* Not a 'descender' */
                        if( BIT_1 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_1 ;
                            display3[i3 + j3] |= DOT_8 ;
                        }
                        if( BIT_2 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_2 ;
                            display3[i3 + j3] |= DOT_9 ;
                        }
                        if( BIT_3 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_3 ; 
                            display3[i3 + j3] |= DOT_10 ;
                        }
                        if( BIT_4 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_4 ; 
                            display3[i3 + j3] |= DOT_11 ;
                        }
                        if( BIT_5 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_5 ; 
                            display3[i3 + j3] |= DOT_12 ;
                        }
                        if( BIT_6 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_6 ; 
                            display3[i3 + j3] |= DOT_13 ;
                        }
                        if( BIT_7 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_7 ; 
                            display3[i3 + j3] |= DOT_14 ;
                        }
                    } /* End of 'normal' orientation, not a 'descender' */
                    else
                    {
                        /* This is 'normal orientation, and 'descender' */
                        if( BIT_1 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_3 ; 
                            display3[i3 + j3] |= DOT_10 ;
                        }
                        if( BIT_2 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_4 ; 
                            display3[i3 + j3] |= DOT_11 ;
                        }
                        if( BIT_3 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_5 ; 
                            display3[i3 + j3] |= DOT_12 ;
                        }
                        if( BIT_4 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_6 ; 
                            display3[i3 + j3] |= DOT_13 ;
                        }
                        if( BIT_5 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_7 ; 
                            display3[i3 + j3] |= DOT_14 ;
                        }
                        if( BIT_6 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_8 ; 
                            display3[i3 + j3] |= DOT_15 ;
                        }
                        if( BIT_7 &charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_9 ; 
                            display3[i3 + j3] |= DOT_16 ;
                        }
                    }
                } /* End of 'normal' orientation' */
                else
                {
                    /* Else, paint 'inverted', not a 'descender' */
                    if( ( charset3[i3] & BIT_0 ) == 0 )
                    {        
                        if( BIT_1 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_9 ; 
                            display3[i3 + j3] |= DOT_16 ;
                        }
                        if( BIT_2 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_8 ; 
                            display3[i3 + j3] |= DOT_15 ;
                        }
                        if( BIT_3 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_7 ; 
                            display3[i3 + j3] |= DOT_14 ;
                        }
                        if( BIT_4 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_6 ; 
                            display3[i3 + j3] |= DOT_13 ;
                        }
                        if( BIT_5 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_5 ; 
                            display3[i3 + j3] |= DOT_12 ;
                        }
                        if( BIT_6 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_4 ; 
                            display3[i3 + j3] |= DOT_11 ;
                        }
                        if( BIT_7 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_3 ; 
                            display3[i3 + j3] |= DOT_10 ;
                        }
                    } /* End of 'inverted' orientation, not a 'descender' */
                    else
                    {
                        /* Else, it's 'inverted', and 'descender' */
                        if( BIT_1 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_7 ; 
                            display3[i3 + j3] |= DOT_14 ;
                        }
                        if( BIT_2 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_6 ; 
                            display3[i3 + j3] |= DOT_13 ;
                        }
                        if( BIT_3 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_5 ; 
                            display3[i3 + j3] |= DOT_12 ;
                        }
                        if( BIT_4 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_4 ; 
                            display3[i3 + j3] |= DOT_11 ;
                        }
                        if( BIT_5 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_3 ; 
                            display3[i3 + j3] |= DOT_10 ;
                        }
                        if( BIT_6 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_2 ; 
                            display3[i3 + j3] |= DOT_9 ;
                        }
                        if( BIT_7 & charset3[i3] )
                        {        
                            display3[i3 + j3] |= DOT_1 ; 
                            display3[i3 + j3] |= DOT_8 ; 
                        }
                    } /* End of 'inverted' and 'descender' */
                } /* End of 'inverted' painting */
            } /* End of all the 'columns' of character */
            
            /* Now, copy the low-order bits to the high-order bits, 
             * in reverse order */
            for( i3 = 0 ; i3 < CHAR_COLUMNS ; i3++ )
            {        
                if( display3[i3 + j3] & BIT_0 )
                {
                    display3[i3 + j3] |= DOT_32 ; 
                }
                if( display3[i3 + j3] & BIT_1 )
                {
                    display3[i3 + j3] |= DOT_31 ; 
                }
                if( display3[i3 + j3] & BIT_2 )
                {
                    display3[i3 + j3] |= DOT_30 ; 
                }
                if( display3[i3 + j3] & BIT_3 )
                {
                    display3[i3 + j3] |= DOT_29 ; 
                }
                if( display3[i3 + j3] & BIT_4 )
                {
                    display3[i3 + j3] |= DOT_28 ; 
                }
                if( display3[i3 + j3] & BIT_5 )
                {
                    display3[i3 + j3] |= DOT_27 ; 
                }
                if( display3[i3 + j3] & BIT_6 )
                {
                    display3[i3 + j3] |= DOT_26 ; 
                }
                if( display3[i3 + j3] & BIT_7 )
                {
                    display3[i3 + j3] |= DOT_25 ; 
                }
                if( display3[i3 + j3] & BIT_8 )
                {
                    display3[i3 + j3] |= DOT_24 ; 
                }
                if( display3[i3 + j3] & BIT_9 )
                {
                    display3[i3 + j3] |= DOT_23 ; 
                }
                if( display3[i3 + j3] & BIT_10 )
                {
                    display3[i3 + j3] |= DOT_22 ; 
                }
                if( display3[i3 + j3] & BIT_11 )
                {
                    display3[i3 + j3] |= DOT_21 ; 
                }
                if( display3[i3 + j3] & BIT_12 )
                {
                    display3[i3 + j3] |= DOT_20 ; 
                }
                if( display3[i3 + j3] & BIT_13 )
                {
                    display3[i3 + j3] |= DOT_19 ; 
                }
                if( display3[i3 + j3] & BIT_14 )
                {
                    display3[i3 + j3] |= DOT_18 ; 
                }
                if( display3[i3 + j3] & BIT_15 )
                {
                    display3[i3 + j3] |= DOT_17 ; 
                }
            } /* End of low-to-high bit reversal */
        } /* End of all 'digits' */

        for( i3 = 0 ; i3 < (INPUT_CHARS * CHAR_COLUMNS) ; i3 += CHAR_COLUMNS )
        {        
#if BMDEBUG
            th_sprintf( szDebug, 
                "\n            %08lX, %08lX, %08lX, %08lX, %08lX\n", 
                display3[i3], 
                display3[i3 + 1], 
                display3[i3 + 2], 
                display3[i3 + 3], 
                display3[i3 + 4] ) ; 
            DebugOut( szDebug ) ; 
#else
            WriteOut( display3[i3] ) ; 
            WriteOut( display3[i3 + 1] ) ; 
            WriteOut( display3[i3 + 2] ) ; 
            WriteOut( display3[i3 + 3] ) ; 
            WriteOut( display3[i3 + 4] ) ; 
#endif /* BMDEBUG */
        }

#if BMDEBUG
        th_sprintf( szDebug, ", %08lX\n", ( n_ulong )loop_cnt ) ; 
        DebugOut( szDebug ) ; 
#else
#if DATA_SIZE == 0
        i3 = (varsize)( loop_cnt &0x0000FFFF ) ; 
        WriteOut( i3 ) ;
        i3 = (varsize)( loop_cnt >> 16 ) ; 
        WriteOut( i3 ) ; 
#else
        WriteOut( loop_cnt ) ; 
#endif
        i3 = (varsize)0xAAAA ; 
        WriteOut( i3 ) ;
#endif /* BMDEBUG */

 #if BMDEBUG
        if( !th_harness_poll() )
        {
            break ; 
        }
#endif
   }
    return 0;
} /* End of function 't_run_test' */


// n_int GetTestData(n_void)
// {
//     n_int i;
//     n_int j;
// 
//     inpNumber = (varsize *)
//         th_malloc((NUM_TESTS + 1) * sizeof(varsize));
//     inpMode = (int *)
//         th_malloc((NUM_TESTS + 1) * sizeof(n_int));
// 
//     if ((inpNumber == NULL) || (inpMode == NULL))
//     {
//         return false;
//     }
// 
//     /*  Copy the test values from ROM to RAM  */
//     for (i = 0, j = 0; i < NUM_TESTS; i++, j++)
//     {
//         inpNumber[i] = (varsize)inpVariableROM[j++];
//         inpMode[i] = (int)inpVariableROM[j];
//     }
// 
//     return true;
// 
// } /* End of function 'GetTestData' */

/*
 *  Function :  GetInputValues
 *
 *    On each pass of the table lookup, a value must be input for 'inputNum'.
 *    and 'inverted'.  Each time this function is called, the next input value is
 *    pulled from the table in RAM.  The table wraps around, so that input data is
 *    continuous.     A flag is returned TRUE whenever the table wraps around.
 *
 */

static n_int GetInputValues(n_void)
{
    inputNum = inpNumber[input_index*2];
    inverted = inpNumber[input_index*2 + 1];
    return false;
    //inputNum = inpNumber[tableCount];
    //inverted = inpMode[tableCount];

    ///* If you run out of test data, then start table over */
    //if (++tableCount < NUM_TESTS)
    //{
    //    return false;
    //}

    //tableCount = 0;
    //return true;

} /* End of function 'GetInputValues' */