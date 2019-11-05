#ifndef CHATPAD_HEADER
#define CHATPAD_HEADER

#define CC_ROWS 56
#define CC_COLS 5
#define CC_ROW(Y) (((((Y) & 0xF0) - 0x10) >> 1) | (((Y) & 0x0F) - 1))
#define CC_COL(X) (__builtin_ctz(X) + 1)
#define IS_POWER_OF_2(X) ((X) > 0 && (((X) - 1) & (X)) == 0)

#define KEY_RETURN 0xb0
#define KEY_LEFT_ARROW 0xd8
#define KEY_RIGHT_ARROW 0xd7
#define KEY_BACKSPACE 0xb2

extern const char chatpad_characters[CC_ROWS][CC_COLS];

// compression for index in table
// index = (((keycode & 0xF0) - 0x10) >> 1) | ((keycode & 0x0F) - 1)
// formula taken from @vitormhenrique (no license)
#endif
