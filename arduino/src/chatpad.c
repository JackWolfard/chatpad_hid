#include "chatpad.h"

/* { unmodified, shifted, green, orange, people } */
const char chatpad_characters[CC_ROWS][CC_COLS] = {
    { '7', '&', '7', '7', '7' },          /* 11 */
    { '6', '^', '6', '6', '6' },          /* 12 */
    { '5', '%', '5', '5', '5' },          /* 13 */
    { '4', '$', '4', '4', '4' },          /* 14 */
    { '3', '#', '3', '3', '3' },          /* 15 */
    { '2', '@', '2', '2', '2' },          /* 16 */
    { '1', '!', '1', '1', '1' },          /* 17 */
    { 0, 0, 0, 0, 0 },                    /* 18 unused */

    { 'u', 'U', '&', 'ú', '7' },          /* 21 */
    { 'y', 'Y', '^', 'ý', '7' },          /* 22 */
    { 't', 'T', '%', 'þ', '7' },          /* 23 */
    { 'r', 'R', '#', '$', '7' },          /* 24 */
    { 'e', 'E', '€', 'é', '7' },          /* 25 */ // missing euro
    { 'w', 'W', '@', 'å', '7' },          /* 26 */
    { 'q', 'Q', '!', '¡', '7' },          /* 27 */
    { 0, 0, 0, 0, 0 },                    /* 28 unused */

    { 'j', 'J', '\'', '"', '7' },         /* 31 */
    { 'h', 'H', '/', '\\', '7' },         /* 32 */
    { 'g', 'G', '¨', '¥', '7' },          /* 33 */ // doesn't work
    { 'f', 'F', '}', '£', '7' },          /* 34 */
    { 'd', 'D', '{', 'ð', '7' },          /* 35 */
    { 's', 'S', 'š', 'ß', '7' },          /* 36 */ // missing Sharp S
    { 'a', 'A', '~', 'á', '7' },          /* 37 */
    { 0, 0, 0, 0, 0 },                    /* 38 unused */

    { 'n', 'N', '<', 'ñ', '7' },          /* 41 */
    { 'b', 'B', '|', '+', '7' },          /* 42 */
    { 'v', 'V', '-', '_', '7' },          /* 43 */
    { 'c', 'C', '»', 'ç', '7' },          /* 44 */
    { 'x', 'X', '«', 'œ', '7' },          /* 45 */ // missing oe
    { 'z', 'Z', '`', 'æ', '7' },          /* 46 */
    { 0, 0, 0, 0, 0 },                    /* 47 unused */
    { 0, 0, 0, 0, 0 },                    /* 48 unused */

    { KEY_RIGHT_ARROW, KEY_RIGHT_ARROW,
      KEY_RIGHT_ARROW, KEY_RIGHT_ARROW,
      KEY_RIGHT_ARROW },                  /* 51 */
    { 'm', 'M', '>', 'µ', '7' },          /* 52 */
    { '.', '<', '?', '¿', '7' },          /* 53 */
    { ' ', ' ', ' ', ' ', ' ' },          /* 54 */
    { KEY_LEFT_ARROW, KEY_LEFT_ARROW,
      KEY_LEFT_ARROW, KEY_LEFT_ARROW,
      KEY_LEFT_ARROW },                   /* 55 */
    { 0, 0, 0, 0, 0 },                    /* 56 unused */
    { 0, 0, 0, 0, 0 },                    /* 57 unused */
    { 0, 0, 0, 0, 0 },                    /* 58 unused */

    { 0, 0, 0, 0, 0 },                    /* 61 unused */
    { ',', '<', ':', ';', '7' },          /* 62 */
    { KEY_RETURN, KEY_RETURN,
      KEY_RETURN, KEY_RETURN,
      KEY_RETURN },                       /* 63 */
    { 'p', 'P', ')', '=', '7' },          /* 64 */
    { '0', ')', '0', '0', '0' },          /* 65 */
    { '9', '(', '9', '9', '9' },          /* 66 */
    { '8', '*', '8', '8', '8' },          /* 67 */
    { 0, 0, 0, 0, 0 },                    /* 68 unused */

    { KEY_BACKSPACE, KEY_BACKSPACE,
      KEY_BACKSPACE, KEY_BACKSPACE,
      KEY_BACKSPACE },                    /* 71 */
    { 'l', 'L', ']', 'Ø', '7' },          /* 72 */
    { 0, 0, 0, 0, 0 },                    /* 73 unused */
    { 0, 0, 0, 0, 0 },                    /* 74 unused */
    { 'o', 'O', '(', 'ó', '7' },          /* 75 */
    { 'i', 'I', '*', 'í', '7' },          /* 76 */
    { 'k', 'K', '[', '7', '7' },          /* 77 */ // missing halo
    { 0, 0, 0, 0, 0 },                    /* 78 unused */
};

