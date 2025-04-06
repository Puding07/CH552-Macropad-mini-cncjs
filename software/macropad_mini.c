// ===================================================================================
// Project:   MacroPad Mini for CH551, CH552 and CH554
// Version:   v1.3
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Firmware example implementation for the MacroPad Mini.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Customize the macro functions in the corresponding section below.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Press a macro key and see what happens.
// - To enter bootloader hold down key 1 while connecting the MacroPad to USB. All
//   NeoPixels will light up white as long as the device is in bootloader mode 
//   (about 10 seconds).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                     // user configurations
#include "src/system.h"                     // system functions
#include "src/delay.h"                      // delay functions
#include "src/neo.h"                        // NeoPixel functions
#include "src/usb_conkbd.h"                 // USB HID consumer keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

#pragma disable_warning 110                 // Keep calm, EVELYN!

int axis = 0; // 0 = X, 1 = Y, 2 = Z
// int jog_distance = 0; // 0 = 1mm, 1 = 10mm, 2 = 20mm, 3 = 3mm, 4 = 40mm, 5 = 50mm etc. 

// ===================================================================================
// Macro Functions which associate Actions with Events (Customize your MacroPad here!)
// ===================================================================================
//
// The list of available USB HID functions can be found in src/usb_conkbd.h

// Key 1 example -> Linux open terminal and run shutdown command
// -------------------------------------------------------------

// ! - Feed Hold
inline void KEY1_PRESSED() {
  KBD_type('!');                          // type '!' character
}

inline void KEY1_RELEASED() {
  // nothing to do
}

inline void KEY1_HOLD() {
  // nothing to do
}


// ctrl + alt + command + h - Homing
inline void KEY2_PRESSED() {
  KBD_press(KBD_KEY_LEFT_CTRL);
  KBD_press(KBD_KEY_LEFT_ALT);
  KBD_press(KBD_KEY_LEFT_GUI);
  KBD_type('h');
  KBD_release(KBD_KEY_LEFT_CTRL);
  KBD_release(KBD_KEY_LEFT_ALT);
  KBD_release(KBD_KEY_LEFT_GUI);
}

inline void KEY2_RELEASED() {
  // nothing to do
}

inline void KEY2_HOLD() {
  // nothing to do
}

// ctrl + alt + command + u - Unlock
inline void KEY3_PRESSED() {
  KBD_press(KBD_KEY_LEFT_CTRL);
  KBD_press(KBD_KEY_LEFT_ALT);
  KBD_press(KBD_KEY_LEFT_GUI);
  KBD_type('u');
  KBD_release(KBD_KEY_LEFT_CTRL);
  KBD_release(KBD_KEY_LEFT_ALT);
  KBD_release(KBD_KEY_LEFT_GUI);
  // CON_press(CON_VOL_MUTE);
}

inline void KEY3_RELEASED() {
  // nothing to do
}

inline void KEY3_HOLD() {
  // nothing to do
}

// ctrl + alt + command + x - Select/Deselect X Axis
// ctrl + alt + command + y - Select/Deselect Y Axis
// ctrl + alt + command + z - Select/Deselect Z Axis
// ctrl + alt + command + a - Select/Deselect A Axis
inline void ENC_SW_PRESSED() {
  KBD_press(KBD_KEY_LEFT_CTRL);
  KBD_press(KBD_KEY_LEFT_ALT);
  KBD_press(KBD_KEY_LEFT_GUI);
  if(axis == 0) {
    axis = 1; // Y Axis
    KBD_type('y');
  } else if(axis == 1) {
    axis = 2; // Z Axis
    KBD_type('z');
  } else if(axis == 2) {
    axis = 0; // X Axis
    KBD_type('x');
  }
  KBD_release(KBD_KEY_LEFT_CTRL);
  KBD_release(KBD_KEY_LEFT_ALT);
  KBD_release(KBD_KEY_LEFT_GUI);
}

inline void ENC_SW_RELEASED() {
  // nothing to do
}

inline void ENC_SW_HOLD() {
  // nothing to do
}

// ctrl + alt + command + ] or f - Jog Forward
inline void ENC_CL() {
  KBD_press(KBD_KEY_LEFT_CTRL);
  KBD_press(KBD_KEY_LEFT_ALT);
  KBD_press(KBD_KEY_LEFT_GUI);
  KBD_type(']');
  KBD_release(KBD_KEY_LEFT_CTRL);
  KBD_release(KBD_KEY_LEFT_ALT);
  KBD_release(KBD_KEY_LEFT_GUI);
}

// ctrl + alt + command + [ or b - Jog Backward
inline void ENC_CCL() {
  KBD_press(KBD_KEY_LEFT_CTRL);
  KBD_press(KBD_KEY_LEFT_ALT);
  KBD_press(KBD_KEY_LEFT_GUI);
  KBD_type('[');
  KBD_release(KBD_KEY_LEFT_CTRL);
  KBD_release(KBD_KEY_LEFT_ALT);
  KBD_release(KBD_KEY_LEFT_GUI);
}

// ===================================================================================
// NeoPixel Functions
// ===================================================================================

// NeoPixel variables
__idata uint8_t neo1 = 127;                 // brightness of NeoPixel 1
__idata uint8_t neo2 = 127;                 // brightness of NeoPixel 2
__idata uint8_t neo3 = 127;                 // brightness of NeoPixel 3

// Update NeoPixels
void NEO_update(void) {
  EA = 0;                                   // disable interrupts
  NEO_writeColor(neo1, 0, 0);               // NeoPixel 1 lights up red
  NEO_writeColor(0, neo2, 0);               // NeoPixel 2 lights up green
  NEO_writeColor(0, 0, neo3);               // NeoPixel 3 lights up blue
  EA = 1;                                   // enable interrupts
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Variables
  __bit key1last = 0;                       // last state of key 1
  __bit key2last = 0;                       // last state of key 2
  __bit key3last = 0;                       // last state of key 3
  __bit enc_sw_last = 0;                    // last state of key 3
  __idata uint8_t i;                        // temp variable

  // Setup
  NEO_init();                               // init NeoPixels
  CLK_config();                             // configure system clock
  DLY_ms(10);                               // wait for clock to settle

  // Enter bootloader if key 1 is pressed
  if(!PIN_read(PIN_KEY1)) {                 // key 1 pressed?
    for(i=9; i; i--) NEO_sendByte(NEO_MAX);     // light up all pixels
    BOOT_now();                             // enter bootloader
  }

  // Init USB keyboard
  KBD_init();                               // init USB HID keyboard
  WDT_start();                              // start watchdog timer

  // Loop
  while(1) {
    // Handle key 1 - command line example (Linux shutdown)
    if(!PIN_read(PIN_KEY1) != key1last) {   // key 1 state changed?
      key1last = !key1last;                 // update last state flag
      if(key1last) {                        // key was pressed?
        neo1 = NEO_MAX;                         // light up corresponding NeoPixel
        NEO_update();                       // update NeoPixels NOW!
        KEY1_PRESSED();                     // take proper action
      }
      else {                                // key was released?
        KEY1_RELEASED();                    // take proper action
      }
    }
    else if(key1last) {                     // key still being pressed?
      neo1 = NEO_MAX;                           // keep NeoPixel on
      KEY1_HOLD();                          // take proper action
    }

    // Handle key 2 - shortcut example (CTRL + ALT + DEL)
    if(!PIN_read(PIN_KEY2) != key2last) {   // key 2 state changed?
      key2last = !key2last;                 // update last state flag
      if(key2last) {                        // key was pressed?
        neo2 = NEO_MAX;                         // light up corresponding NeoPixel
        NEO_update();                       // update NeoPixels NOW!
        KEY2_PRESSED();                     // take proper action
      }
      else {                                // key was released?
        KEY2_RELEASED();                    // take proper action
      }
    }
    else if(key2last) {                     // key still being pressed?
      neo2 = NEO_MAX;                           // keep NeoPixel on
      KEY2_HOLD();                          // take proper action
    }

    // Handle key 3 - consumer key example (volume mute)
    if(!PIN_read(PIN_KEY3) != key3last) {   // key 3 state changed?
      key3last = !key3last;                 // update last state flag
      if(key3last) {                        // key was pressed?
        neo3 = NEO_MAX;                         // light up corresponding NeoPixel
        NEO_update();                       // update NeoPixels NOW!
        KEY3_PRESSED();                     // take proper action
      }
      else {                                // key was released?
        KEY3_RELEASED();                    // take proper action
      }
    }
    else if(key3last) {                     // key still being pressed?
      neo3 = NEO_MAX;                           // keep NeoPixel on
      KEY3_HOLD();                          // take proper action
    }
    
    // Handle key 3 - consumer key example (volume mute)
    if(!PIN_read(PIN_ENC_SW) != enc_sw_last) {   // key 3 state changed?
      enc_sw_last = !enc_sw_last;                 // update last state flag
      if(enc_sw_last) {                        // key was pressed?
        // skip neo
        ENC_SW_PRESSED();                     // take proper action
      }
      else {                                // key was released?
        ENC_SW_RELEASED();                    // take proper action
      }
    }
    else if(enc_sw_last) {                     // key still being pressed?
      ENC_SW_HOLD();                          // take proper action
    }

    // Handle encoder A/B - rotary encoder example (volume up/down)
    if (!PIN_read(PIN_ENC_A)) { // encoder turned?
      if (PIN_read(PIN_ENC_B)) {
        ENC_CL(); // clockwise?
      }
      else {
        ENC_CCL(); // counter-clockwise?
      }
      while (!PIN_read(PIN_ENC_A))
        ; // wait until next detent
    }


    // Update NeoPixels
    NEO_update();
    if(neo1) neo1--;                        // fade down NeoPixel
    if(neo2) neo2--;                        // fade down NeoPixel
    if(neo3) neo3--;                        // fade down NeoPixel
    DLY_ms(5);                              // latch and debounce
    WDT_reset();                            // reset watchdog
  }
}
