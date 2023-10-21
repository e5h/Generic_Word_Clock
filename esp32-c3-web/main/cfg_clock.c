/* Auto-generated source file for clock configuration */

#include "cfg_clock.h"

const CLOCK_WORD CLOCK_words_S[] = {
    /* Type - custom */
    { "my",         2, WORD_CUSTOM, {10, 9} },
    { "happy",      5, WORD_CUSTOM, {92, 87, 72, 67, 52} },
    { "bday",       4, WORD_CUSTOM, {34, 25, 14, 5} },

    /* Type - prefixes */
    { "it",         2, WORD_PREFIX, {90, 89} },
    { "is",         2, WORD_PREFIX, {69, 50} },
    { "a",          1, WORD_PREFIX, {30} },
    { "twenty",     6, WORD_PREFIX, {91, 88, 71, 68, 51, 48} },
    { "half",       4, WORD_PREFIX, {31, 28, 11, 8} },
    { "five",       4, WORD_PREFIX, {32, 27, 12, 7} },
    { "ten",        3, WORD_PREFIX, {93, 86, 73} },
    { "quarter",    7, WORD_PREFIX, {66, 53, 46, 33, 26, 13, 6} },
    { "to",         2, WORD_PREFIX, {94, 85} },
    { "past",       4, WORD_PREFIX, {74, 65, 54, 45} },
    
    /* Type - hours */
    { "one",        3, WORD_HOUR,   {95, 84, 75} },
    { "two",        3, WORD_HOUR,   {24, 15, 4} },
    { "three",      5, WORD_HOUR,   {97, 82, 77, 62, 57} },
    { "four",       4, WORD_HOUR,   {64, 55, 44, 35} },
    { "five",       4, WORD_HOUR,   {96, 83, 76, 63} },
    { "six",        3, WORD_HOUR,   {56, 43, 36} },
    { "seven",      5, WORD_HOUR,   {42, 37, 22, 17, 2} },
    { "eight",      5, WORD_HOUR,   {41, 38, 21, 18, 1} },
    { "nine",       4, WORD_HOUR,   {99, 80, 79, 60} },
    { "ten",        3, WORD_HOUR,   {23, 16, 3} },
    { "eleven",     6, WORD_HOUR,   {60, 59, 40, 39, 20, 19} },
    { "twelve",     6, WORD_HOUR,   {98, 81, 78, 61, 58, 41} }
};

const UINT8 CLOCK_num_words_u8 = ( sizeof( CLOCK_words_S ) / sizeof( CLOCK_WORD ) );

const UINT8 CLOCK_xy_pixel_u8[10][10] = {
    { 99u, 98u, 97u, 96u, 95u, 94u, 93u, 92u, 91u, 90u },
    { 80u, 81u, 82u, 83u, 84u, 85u, 86u, 87u, 88u, 89u },
    { 79u, 78u, 77u, 76u, 75u, 74u, 73u, 72u, 71u, 70u },
    { 60u, 61u, 62u, 63u, 64u, 65u, 66u, 67u, 68u, 69u },
    { 59u, 58u, 57u, 56u, 55u, 54u, 53u, 52u, 51u, 50u },
    { 40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u, 48u, 49u },
    { 39u, 38u, 37u, 36u, 35u, 34u, 33u, 32u, 31u, 30u },
    { 20u, 21u, 22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u },
    { 19u, 18u, 17u, 16u, 15u, 14u, 13u, 12u, 11u, 10u },
    {  0u,  1u,  2u,  3u,  4u,  5u,  6u,  7u,  8u,  9u },
};