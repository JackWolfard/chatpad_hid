#define USB_SERIAL Serial
#define CHATPAD_SERIAL Serial1
#define REPEAT_KEY_INIT_DELAY 500 // ms
#define REPEAT_KEYS_PER_SECOND 4
#define REPEAT_KEY_DELAY ((int) ((float) 1 / (float) REPEAT_KEYS_PER_SECOND * 1000)) // ms
#define REFRESH_CHATPAD_TIME 800 // ms

char initialize_chatpad_buffer[5] = {0x87, 0x02, 0x8c, 0x1f, 0xcc};
char refresh_chatpad_buffer[5] = {0x87, 0x02, 0x8c, 0x1b, 0xd0};
char chatpad_buffer[8];
unsigned long current_time;
unsigned long refresh_time;
unsigned long repeat_time;

struct pressed_key {
  char keycode;
  unsigned long time_pressed;
  char repeating;
  char modifier;
};

struct pressed_key pressed_keys[2];

int handle_read(char *chatpad_buffer, int buffer_size);
int handle_keypress(struct pressed_key *pressed_key, char modifier, char keycode, unsigned long current_time,
                    void (*report_keypress)(struct pressed_key *));
int handle_repeat_keys(struct pressed_key *pressed_keys, int pressed_keys_size, unsigned long current_time,
                       void (*report_keypress)(struct pressed_key *));
void report_keypress_serial(struct pressed_key *pressed_key);

void setup() {
  USB_SERIAL.begin(9600);
  while (!USB_SERIAL);
  USB_SERIAL.println("Arduino Xbox 360 Chatpad Demo");
  CHATPAD_SERIAL.begin(19200, SERIAL_8N1);
  CHATPAD_SERIAL.write(initialize_chatpad_buffer, 5);
  USB_SERIAL.println("Chatpad initialized");
  refresh_time = millis();
  repeat_time = refresh_time;
}

void loop() {
  // read current time
  current_time = millis();
  
  // check if chatpad serial port needs to be kept alive
  if (current_time - refresh_time > REFRESH_CHATPAD_TIME) {
    USB_SERIAL.println("Refreshing Chatpad");
    refresh_time = current_time;
    CHATPAD_SERIAL.write(refresh_chatpad_buffer, 5);
  }

  // check if 8 bytes can be read
  if (CHATPAD_SERIAL.available() >= 8) {
    USB_SERIAL.println("Reading from Chatpad");
    CHATPAD_SERIAL.readBytes(chatpad_buffer, 8);
    USB_SERIAL.write(chatpad_buffer, 8);

    // see how many character keys are pressed
    int characters_present = handle_read(chatpad_buffer, 8);

    // handle the key press
    if (characters_present >= 0) {
      // bug: note that when both keys are repeating and the first one is released, the second will be shifted over
      // to the other location in the pressed_keys meaning that it won't repeat anymore...
      handle_keypress(pressed_keys, chatpad_buffer[3], chatpad_buffer[4], current_time, report_keypress_serial);
      handle_keypress(&pressed_keys[1], chatpad_buffer[3], chatpad_buffer[5], current_time, report_keypress_serial);
    }
  }

  // check if currently pressed keys are repeating
  if (current_time - repeat_time > REPEAT_KEY_DELAY) {
    repeat_time = current_time;
    handle_repeat_keys(pressed_keys, 2, current_time, report_keypress_serial);
  }
}

/**
 * Takes the raw input from Chatpad and checks for a key press report 
 * 
 * @param chatpad_buffer the buffer to read from
 * @param buffer_size the size of the buffer
 * @return -1 for error or ignorable message, otherwise number of characters 0-2 present in buffer
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
 * Takes the confirmed keypress input from Chatpad and checks if the key has already been reported as being pressed
 * 
 * @param pressed_key the metadata tracking this pressed key
 * @param modifier the state of the modifier keys
 * @param keycode the keycode reported by the Chatpad
 * @param current_time the current time
 * @return 1 if the pressed_key was updated, 0 if not
 */
int handle_keypress(struct pressed_key *pressed_key, char modifier, char keycode, unsigned long current_time,
                    void (*report_keypress)(struct pressed_key *)) {
  if (keycode && keycode != pressed_key->keycode && modifier != pressed_key->modifier) {
    pressed_key->keycode = keycode;
    pressed_key->repeating = false;
    pressed_key->time_pressed = current_time;
    pressed_key->modifier = modifier;
    report_keypress(pressed_key);
    return 1;
  }
  return 0;
}

/**
 * Checks for keys which need to be repeated
 * 
 * @param pressed_keys an array of pressed key metadata
 * @param pressed_keys_size the size of the array
 * @param current_time the current time
 * @return -1 for failure, 0 for success
 */
int handle_repeat_keys(struct pressed_key *pressed_keys, int pressed_keys_size, unsigned long current_time,
                       void (*report_keypress)(struct pressed_key *)) {
  if (pressed_keys_size <= 0) {
    return -1;
  }
  for (struct pressed_key *pressed_key = pressed_keys; pressed_key < pressed_keys + pressed_keys_size;
       pressed_key++) {
    if (pressed_key->keycode) {
      if (pressed_key->repeating) {
        pressed_key->time_pressed = current_time;
        report_keypress(pressed_key);
      } else if (current_time - pressed_key->time_pressed > REPEAT_KEY_INIT_DELAY) {
        pressed_key->repeating = true;
        pressed_key->time_pressed = current_time;
        report_keypress(pressed_key);
      }
    }
  }
  return 0;
}

/**
 * Report the pressed key to the USB Serial
 * 
 * @param pressed_key the key that was pressed
 */
void report_keypress_serial(struct pressed_key *pressed_key) {
  USB_SERIAL.print("key press: ");
  USB_SERIAL.print(pressed_key->modifier, BIN);
  USB_SERIAL.print(" ");
  USB_SERIAL.print(pressed_key->keycode, HEX);
  USB_SERIAL.println();
}
