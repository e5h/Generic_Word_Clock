/*==============================================================================
 *==============================================================================
 * NAME:
 *      task_display.c
 *
 * PURPOSE:
 *      One of the major tasks of the application.
 *
 *      The display task is responsible for the RMT peripheral, and managing the
 *      entire context of the "clock" from an abstracted level. This involves
 *      translating system time to words, handling brightness and color changes,
 *      and handling animations on the display.
 *
 * DEPENDENCIES:
 *      task_display.h
 *      lib_messaging.h
 *      cfg_clock.h
 *
 *==============================================================================
 * (C) Andrew Bright 2024, github.com/e5h
 *==============================================================================
 *=============================================================================*/

/*=============================================================================*/
/*][ Feature Switches ][=======================================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ Include Files ][==========================================================*/
/*=============================================================================*/

#include "task_display.h"
#include "lib_messaging.h"
#include "cfg_clock.h"

/*=============================================================================*/
/*][ LOCAL : Constants and Types ][============================================*/
/*=============================================================================*/

const static char* LOG_TAG = "task_display";

/*=============================================================================*/
/*][ LOCAL : Variables ][======================================================*/
/*=============================================================================*/

/* ESP event groups (flags) */
static EventGroupHandle_t h_display_flags;

/* FreeRTOS task handles */
static TaskHandle_t h_display_task;

/* Order and request queues */
static QueueHandle_t h_display_queue;

/*=============================================================================*/
/*][ LOCAL : Function Prototypes ][============================================*/
/*=============================================================================*/

/*=============================================================================*/
/*][ LOCAL : Function Definitions ][===========================================*/
/*=============================================================================*/

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * <<< LOCAL FUNCTION >>>
 * SUMMARY:
 *      ColorWordPixels() - "Color the pixels of a given word"
 *
 * DESCRIPTION:
 *      Takes the index of a word in the clock_config.c words structure, and
 *      loops through the defined pixels to color them all.
 *
 * INPUTS:
 *      wordIndex - the index of the word in the structure
 *      color - the color to print the word in
 *      brightness - the percentage brightness value to print the word in
 *
 * OUTPUTS:
 *      TRUE - successfully printed the word
 *      FALSE - could not print pixels
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
BOOL ColorWordPixels( UINT8 word_index_u8, RGB_COLOR_PCT color_S, UINT8 brightness_u8 )
{
    /* Ensure the index is within bounds */
    if( word_index_u8 >= CLOCK_num_words_u8 )
    {
        return FALSE;
    }

    ESP_LOGI( "ColorWordPixels()", "Printing word: \"%s\"", CLOCK_words_S[ word_index_u8 ].word_str );

    /* For each letter in the word, */
    for( INT8 pixel = 0; pixel < CLOCK_words_S[ word_index_u8 ].word_length_u8; pixel++ )
    {
        /* Color the pixel */
        RGB_LED_SetPixelColor( CLOCK_words_S[ word_index_u8 ].word_pixels_u8[ pixel ], color_S, brightness_u8 );
        ESP_LOGD( "ColorWordPixels()", "Colored pixel #%d", CLOCK_words_S[ word_index_u8 ].word_pixels_u8[ pixel ] );
    }

    return TRUE;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * <<< LOCAL FUNCTION >>>
 * SUMMARY:
 *      DisplayWord() - "Display a given word on the clock face"
 *
 * DESCRIPTION:
 *      Takes the string of a word and checks to see if the word exists in
 *      the collection of words available on the clock face. If the word
 *      exits, it first checks that the type is compatible before finding
 *      the index of the word and calling ColorWordPixels() to
 *      color the associated pixels.
 *
 * INPUTS:
 *      word - the word to display
 *      type - the type of word (prefix, hour, etc)
 *      color - the color to print the word in
 *      brightness - the percentage brightness value to print the word in
 *
 * OUTPUTS:
 *      TRUE - successfully printed the word
 *      FALSE - could not find word
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
BOOL DisplayWord( STRING word_str, CLOCK_WORD_TYPE word_type_E, RGB_COLOR_PCT color_S, UINT8 brightness_u8 )
{
    BOOL success_b = TRUE;

    /* Do not check if null pointer */
    if( word_str == NULL )
    {
        return FALSE;
    }

    /* Check the current buffer for matches against word collection */
    for( INT8 word = 0; word < CLOCK_num_words_u8; word++ )
    {
        /* If a match exists */
        if( ( strcmp( word_str, CLOCK_words_S[ word ].word_str ) == 0 )
         && ( ( CLOCK_words_S[ word ].word_type_E & word_type_E ) != 0 ) )
        {
            success_b = ColorWordPixels( word, color_S, brightness_u8 );
            break;
        }
        /* If a match was not found in all words */
        else if( word == ( CLOCK_num_words_u8 - 1 ) )
        {
            /* Mark failed phrase display */
            ESP_LOGW( "DisplayWord()", "Failed to find: \"%s\"!", word_str );
            return FALSE;
        }
    }

    return success_b;
}

/*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
 * <<< LOCAL FUNCTION >>>
 * SUMMARY:
 *      DisplayPhrase() - "Display a given phrase on the clock face"
 *
 * DESCRIPTION:
 *      Takes a phrase in string format, and separates into substrings which
 *      are then used to search the structure of words in the cfg_clock.c
 *      file. If a match is found, the index of the word in the structure
 *      is passed to another helper function to display it.
 *
 * INPUTS:
 *      phrase - phrase to parse and display
 *      type - the type of words to look for (can be multiple)
 *      color - the color to print the words in
 *      brightness - the percentage brightness value to print the words in
 *
 * OUTPUTS:
 *      TRUE - successfully printed every word in the phrase
 *      FALSE - could not parse and display one or more words
 *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/
BOOL DisplayPhrase( STRING phrase_str, CLOCK_WORD_TYPE word_type_E, RGB_COLOR_PCT color_S, UINT8 brightness_u8 )
{
    BOOL success_b = TRUE;
    CHAR buffer_current_word_c[ MAX_WORD_LENGTH ] = { 0 };
    UINT8 buffer_charindex_u8 = 0;
    UINT8 phrase_charindex_u8 = 0;

    /* Only loop while evaluating string */
    while( phrase_str[ phrase_charindex_u8 ] != '\0' )
    {
        /* If a word inside the phrase is encountered */
        if( phrase_str[ phrase_charindex_u8 ] == ' ' )
        {
            /* Cap the buffered string */
            buffer_current_word_c[ buffer_charindex_u8 ] = '\0';

            /* Attempt to display the word (checks against word collection) */
            success_b &= DisplayWord( buffer_current_word_c, word_type_E, color_S, brightness_u8 );

            /* Reset the "current word" buffer */
            memset( buffer_current_word_c, 0, sizeof( buffer_current_word_c ) );
            buffer_charindex_u8 = 0;
            phrase_charindex_u8++;
        }
        /* If the next character is the end of the phrase */
        else if( phrase_str[ phrase_charindex_u8 + 1 ] == '\0' )
        {
            /* Final update to the buffer */
            buffer_current_word_c[ buffer_charindex_u8++ ] = phrase_str[ phrase_charindex_u8++ ];
            buffer_current_word_c[ buffer_charindex_u8 ] = '\0';

            /* Check the current buffer for matches against word collection */
            success_b &= DisplayWord( buffer_current_word_c, word_type_E, color_S, brightness_u8 );
        }
        else
        {
            /* Add the indexed phrase character to the "current word" buffer */
            buffer_current_word_c[ buffer_charindex_u8++ ] = phrase_str[ phrase_charindex_u8++ ];
        }
    }

    return success_b;
}

BOOL CLOCK_Init( void );
BOOL CLOCK_Tick( void );

BOOL CLOCK_UpdateTime( void )
{
    /* Wipe, and delay to prevent timing glitches */
    RGB_LED_Wipe();
    vTaskDelay( pdMS_TO_TICKS( 10 ) );

    DisplayPhrase( "it is zozz twenty five to", WORD_PREFIX, RGB_LED_default_colors_S[ COLOR_Mint ], 100 );
    DisplayPhrase( "eleven my guy", WORD_HOUR, RGB_LED_default_colors_S[ COLOR_Rose ], 100 );

    RGB_LED_TransmitColors();

    return TRUE;
}

/* Test - cycle the words */
BOOL CLOCK_TestWords( RGB_COLOR_PCT color )
{
    static UINT8 word_index_u8 = 0;
    
    /* Wipe, and delay to prevent timing glitches */
    RGB_LED_Wipe();
    vTaskDelay( pdMS_TO_TICKS( 10 ) );

    ColorWordPixels( word_index_u8, color, 100 );

    if( ++word_index_u8 >= CLOCK_num_words_u8 )
    {
        word_index_u8 = 0;
    }

    RGB_LED_TransmitColors();

    return TRUE;
}