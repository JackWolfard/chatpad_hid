#include <Keyboard.h>
#include "src/chatpad.h"

#define USB_SERIAL Serial
#define CHATPAD_SERIAL Serial1
#define REFRESH_CHATPAD_TIME 1000 // ms
#define PRESSED_KEYS_BUFFER_SIZE 2

char initialize_chatpad_buffer[5] = {0x87, 0x02, 0x8c, 0x1f, 0xcc};
char refresh_chatpad_buffer[5] = {0x87, 0x02, 0x8c, 0x1b, 0xd0};
char chatpad_buffer[8];
unsigned long current_time;
unsigned long refresh_time;

struct pressed_key {
  char keycode;
  char modifier;
};

struct pressed_key pressed_keys[PRESSED_KEYS_BUFFER_SIZE];

int seek_to_header(void);
int handle_read(char *chatpad_buffer, int buffer_size);
int handle_keypress(struct pressed_key *pressed_keys, int pressed_keys_buffer_size, char modifier, char keycode, int index);
void report_press(char modifier, char keycode);
void report_release(char modifier, char keycode);
char translate_key(char modifier, char keycode);

void setup() {
//  USB_SERIAL.begin(9600);
//  while (!USB_SERIAL);
//  USB_SERIAL.println("Chatpad starting");
//  USB_SERIAL.println(chatpad_characters[1][0]);
  Keyboard.begin();
  CHATPAD_SERIAL.begin(19200, SERIAL_8N1);
  CHATPAD_SERIAL.write(initialize_chatpad_buffer, 5);
  refresh_time = millis();
  Keyboard.write(184);
}

void loop() {
  // read current time
  current_time = millis();

  // check if chatpad serial port needs to be kept alive
  if (current_time - refresh_time > REFRESH_CHATPAD_TIME) {
    refresh_time = current_time;
    CHATPAD_SERIAL.write(refresh_chatpad_buffer, 5);
  }

  // check if 8 bytes can be read
  if (CHATPAD_SERIAL.available() >= 8 && seek_to_header()) {
    CHATPAD_SERIAL.readBytes(chatpad_buffer, 8);

    // see how many character keys are pressed
    int characters_present = handle_read(chatpad_buffer, 8);

    // handle the key press
    if (characters_present >= 0) {
      handle_keypress(pressed_keys, PRESSED_KEYS_BUFFER_SIZE, chatpad_buffer[3], chatpad_buffer[4], 0);
      handle_keypress(pressed_keys, PRESSED_KEYS_BUFFER_SIZE, chatpad_buffer[3], chatpad_buffer[5], 1);
    }
  }
}

/**
   When called will seek for a header data in serial to realign the program with the Chatpad's 8 byte
   messages.

   @return 0 if not able to find header data or < 8 bytes to read, 1 for at header data and >= 8 bytes to be read
*/
int seek_to_header() {
  int available_data = CHATPAD_SERIAL.available();
  char peek_header = CHATPAD_SERIAL.peek();
  while (available_data >= 8 && !(peek_header == 0xb4 || peek_header == 0xa5)) {
    CHATPAD_SERIAL.read();
    available_data--;
    peek_header = CHATPAD_SERIAL.peek();
  }
  return available_data >= 8;
}

/**
   Takes the raw input from Chatpad and checks for a key press report

   @param chatpad_buffer the buffer to read from
   @param buffer_size the size of the buffer
   @return -1 for error or ignorable message, otherwise number of characters 0-2 present in buffer
*/
int handle_read(char *chatpad_buffer, int buffer_size) {
  if (buffer_size < 8) {
    return -1;
  }
  if (chatpad_buffer[0] == 0xb4 && chatpad_buffer[1] == 0xc5) {
    // confirm the checksum
    char checksum = 0x79; // know that we start w/ (0xb4 + 0xc5) % 256
    for (int i = 2; i < 7; i++) {
      checksum += chatpad_buffer[i];
    }
    checksum = ~checksum + 1;
    if (checksum == chatpad_buffer[7]) {
      int characters_present = 0;
      for (int i = 4; i < 6; i++) {
        if (chatpad_buffer[i]) {
          characters_present++;
        }
      }
      return characters_present;
    }
  }
  return -1;
}

/**
   Takes the confirmed keypress input from Chatpad and checks if the key has already been reported as being pressed

   @param pressed_keys the metadata for pressed keys
   @param pressed_keys_buffer_size the number of keys of metadata
   @param modifier the state of the modifier keys
   @param keycode the keycode reported by the Chatpad
   @param index the index in metadata to evict if keycode not found
   @return 1 if the pressed_key was updated, 0 if not
*/
int handle_keypress(struct pressed_key *pressed_keys, int pressed_keys_buffer_size, char modifier, char keycode, int index) {
  int found_index = 0;
  int found = 0;
  if (keycode || (!IS_POWER_OF_2(modifier) && modifier)) {
    for (int i = 0; i < pressed_keys_buffer_size && !found; i++) {
      if (keycode == pressed_keys[i].keycode && modifier == pressed_keys[i].modifier) {
        found = 1;
        found_index = i;
      }
    }
  }
  if (!found) {
    struct pressed_key *pressed_key = &pressed_keys[index];
    report_release(pressed_key->modifier, pressed_key->keycode);
    pressed_key->keycode = keycode;
    pressed_key->modifier = modifier;
    report_press(modifier, keycode);
  } else if (found_index != index) {
    struct pressed_key *pressed_key = &pressed_keys[index];
    report_release(pressed_key->modifier, pressed_key->keycode);
    pressed_key->keycode = keycode;
    pressed_key->modifier = modifier;
    pressed_key = &pressed_keys[found_index];
    pressed_key->keycode = 0x0;
    pressed_key->modifier = 0x0;
  }
}

void report_press(char modifier, char keycode) {
  char character = translate_key(modifier, keycode);
  if (character) {
    Keyboard.press(character);
  }
}

void report_release(char modifier, char keycode) {
  char character = translate_key(modifier, keycode);
  if (character) {
    Keyboard.release(character);
  }
}

char translate_key(char modifier, char keycode) {
  if (keycode) {
    int row_index = CC_ROW(keycode);
    int col_index = CC_COL(modifier);
    return chatpad_characters[row_index][col_index];
  } else if (!IS_POWER_OF_2(modifier) && modifier) {
    switch (modifier) {
      case 0b0101: {
        return KEY_CAPS_LOCK;
        break;
      }
    }
  }
}
