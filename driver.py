import serial
import threading
import sys
import pathlib
import json
import math
from time import time

def init_port(port):
  data = [b"\x87", b"\x02", b"\x8c", b"\x1f", b"\xcc"]
  for datum in data:
    port.write(datum)

def keep_port_alive(*args):
  port = args[0]
  data = [b"\x87", b"\x02", b"\x8c", b"\x1b", b"\xd0"]
  for datum in data:
    port.write(datum)

def handle_read(data, keymap):
  if data.startswith(bytes.fromhex("b4c5")):
    # check the checksum for corruption
    checksum = ~(sum(data[:-1]) % 256) + 2 ** 8 + 1
    if checksum == data[-1]:
      characters_read = []
      # check for 2nd character
      if data[4]:
        character = translate_data(data[3], hex(data[4]), keymap)
        if character:
          characters_read.append(character)
      if data[5]:
        character = translate_data(data[3], hex(data[5]), keymap)
        if character:
          characters_read.append(character)
      return characters_read
  return False

def translate_data(modifier, character, keymap):
  index = int(math.log2(modifier)) + 1 if modifier else 0
  if modifier & 0x8:
    # could make this an emoji modifier later ;)
    # this was the people button
    index = 0
  if character in keymap:
    if type(keymap[character]) is str:
      return keymap[character]
    return keymap[character][index]

def handle_pressed_keys(pressed_keys, keys):
  current_time = time()
  new_pressed_keys = {}
  # keys present
  for key in keys:
    if key in pressed_keys:
      new_pressed_keys[key] = pressed_keys[key]
    else:
      report_key_press(key)
      new_pressed_keys[key] = (current_time, False)
  pressed_keys.clear()
  pressed_keys.update(new_pressed_keys)

def handle_repeat_keys(pressed_keys, repeat_delay):
  current_time = time()
  for key in pressed_keys:
    time_pressed, repeating = pressed_keys[key]
    if not repeating and current_time - time_pressed > repeat_delay:
      repeating = True
      report_key_press(key)
      time_pressed = current_time
    elif repeating:
      report_key_press(key)
      time_pressed = current_time
    pressed_keys[key] = (time_pressed, repeating)

def report_key_press(key):
  if key not in ["Enter", "Backspace", "Left", "Right"]:
    print(key, end="", flush=True)
  elif key == "Enter":
    print()

class RepeatingTimer(threading.Thread):
  def __init__(self, timeout, callback, *callback_args, **callback_kwargs):
    threading.Thread.__init__(self)
    self.callback_args = callback_args
    self.callback_kwargs = callback_kwargs
    self.callback = callback
    self.timeout = timeout
    self.stop_event = threading.Event()

  def run(self):
    while not self.stop_event.wait(self.timeout):
      self.callback(*self.callback_args, **self.callback_kwargs)

  def stop(self):
    self.stop_event.set()

if __name__ == "__main__":
  keymap_path = pathlib.Path("key_map.json")
  if not keymap_path.exists():
    print("key_map.json does not exist. cannot translate keys.")
    exit()
  with keymap_path.open() as keymap_file:
    data = keymap_file.read()
  keymap = json.loads(data)
  print("loaded keymap")
  port = serial.Serial("/dev/serial0", baudrate=19200,
                       bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                       stopbits=serial.STOPBITS_ONE)
  print("port opened")
  init_port(port)
  keep_port_alive_timer = RepeatingTimer(1.0, keep_port_alive, port)
  keep_port_alive_timer.start()
  print("timer started")
  pressed_keys = {}
  repeat_delay = 0.5
  repeat_per_second = 4
  repeat_again_delay = 1 / repeat_per_second
  repeat_keys_timer = RepeatingTimer(repeat_again_delay, handle_repeat_keys, pressed_keys, repeat_delay)
  repeat_keys_timer.start()
  while True:
    characters_read = handle_read(port.read(8), keymap)
    if type(characters_read) is list:
      handle_pressed_keys(pressed_keys, characters_read)
  repeat_keys_timer.stop()
  keep_port_alive_timer.stop()
  port.close()
