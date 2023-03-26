// ===================================================================================
// Project:   MacroPad Mini for CH551, CH552 and CH554
// Version:   v1.1
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
// - Clock: min. 12 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
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
#include <config.h>                         // user configurations
#include <system.h>                         // system functions
#include <delay.h>                          // delay functions
#include <neo.h>                            // NeoPixel functions
#include <usb_conkbd.h>                     // USB HID consumer keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
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
  __idata uint8_t i;                        // temp variable

  // Enter bootloader if key 1 is pressed
  NEO_init();                               // init NeoPixels
  if(!PIN_read(PIN_KEY1)) {                 // key 1 pressed?
    NEO_latch();                            // make sure pixels are ready
    for(i=9; i; i--) NEO_sendByte(127);     // light up all pixels
    BOOT_now();                             // enter bootloader
  }

  // Setup
  CLK_config();                             // configure system clock
  DLY_ms(5);                                // wait for clock to settle
  KBD_init();                               // init USB HID keyboard
  WDT_start();                              // start watchdog timer

  // Loop
  while(1) {
    // Handle key 1 - command line example (Linux shutdown)
    if(!PIN_read(PIN_KEY1) != key1last) {   // key 1 state changed?
      key1last = !key1last;                 // update last state flag
      if(key1last) {                        // key was pressed?
        neo1 = 127;                         // light up corresponding NeoPixel
        NEO_update();                       // update NeoPixels NOW!
        KBD_press(KBD_KEY_LEFT_GUI);        // press LEFT GUI key
        KBD_type('t');                      // press and release 'T' to open terminal
        DLY_ms(500);                        // wait for terminal to open
        KBD_release(KBD_KEY_LEFT_GUI);      // release LEFT GUI key
        KBD_print("sudo shutdown -h now");  // type command
        KBD_type(KBD_KEY_RETURN);           // press and release RETURN key
      }
      else {                                // key was released?
                                            // nothing to do in this case
      }
    }
    else if(key1last) {                     // key still being pressed?
                                            // nothing to do in this case
    }

    // Handle key 2 - shortcut example (CTRL + ALT + DEL)
    if(!PIN_read(PIN_KEY2) != key2last) {   // key 2 state changed?
      key2last = !key2last;                 // update last state flag
      if(key2last) {                        // key was pressed?
        neo2 = 127;                         // light up corresponding NeoPixel
        NEO_update();                       // update NeoPixels NOW!
        KBD_press(KBD_KEY_LEFT_CTRL);       // press LEFT CTRL key
        KBD_press(KBD_KEY_LEFT_ALT);        // press LEFT ALT key
        KBD_press(KBD_KEY_DELETE);          // press DEL key
      }
      else {                                // key was released?
        KBD_release(KBD_KEY_DELETE);        // release DEL key
        KBD_release(KBD_KEY_LEFT_ALT);      // release LEFT ALT key
        KBD_release(KBD_KEY_LEFT_CTRL);     // release LEFT CTRL key
      }
    }
    else if(key2last) {                     // key still being pressed?
      neo2 = 127;                           // keep NeoPixel on
    }

    // Handle key 3 - consumer key example (volume mute)
    if(!PIN_read(PIN_KEY3) != key3last) {   // key 3 state changed?
      key3last = !key3last;                 // update last state flag
      if(key3last) {                        // key was pressed?
        neo3 = 127;                         // light up corresponding NeoPixel
        CON_press(CON_VOL_MUTE);            // press VOLUME MUTE consumer key
      }
      else {                                // key was released?
        CON_release(CON_VOL_MUTE);          // release VOLUME MUTE consumer key
      }
    }
    else if(key3last) {                     // key still being pressed?
      neo3 = 127;                           // keep NeoPixel on
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
