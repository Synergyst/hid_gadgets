# hid_gadgets
HID Gadgets for the Raspberry Pi 0

# Compiling:
- HID keyboard wrapper: gcc -pedantic hid_gadget_wrapper.c -std=gnu99 -o hid_keyboard_gadget_wrapper
- HID keyboard gadget: gcc ./hid_keyboard_gadget.c -std=gnu99 -o hid_keyboard_gadget

# Running the program(s):
- HID keyboard wrapper: ./hid_keyboard_gadget_wrapper <char/string>

# TODO:
- Fix naming referred within the source to fit the naming for each release file..
- Accept keyboard modifiers within the wrapper utility.
