/*
 * Filename: commands.ino
 * Description: Example for library TokenProcessor
 *
 * Version: 1.0.0
 * Author: Joao Alves <jpralves@gmail.com>
 * Required files: -
 * Required Libraries: TokenProcessor
 * Tested on: Arduino Nano, Arduino Uno, ESP8266
 *
 * History:
 * 1.0.0 - 2017-03-14 - Initial Version
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if (defined(__AVR__))
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include "TokenProcessor.h"

// Features:

#define FREERAM 1
#define I2CSCANNER 1
#define EEPROMDUMP 1
#define HELPCMD 1
#define LEDCMDTESTER 1

#ifdef I2CSCANNER
#include <Wire.h>
#endif

#ifdef EEPROMDUMP
#define EEPROM_SIZE 1024
#include <EEPROM.h>
#endif

// ----- MAIN CODE -------

char debug = 0;

const char commands[] = {
#ifdef HELPCMD
      'h', 
#endif
#ifdef DEBUG
      'd', 
#endif
#ifdef LEDCMDTESTER
      '1', '0', 
#endif                   
#ifdef FREERAM                   
      'f',
#endif               
#ifdef I2CSCANNER
      'i',
#endif
#ifdef EEPROMDUMP
      'e',
#endif
};

void setDebug(TokenProcessor *cps);
void getHelp(TokenProcessor *cps);
void LED13_on(TokenProcessor *cps);
void LED13_off(TokenProcessor *cps);
void printFreeRAM(TokenProcessor *cps);
void i2cScanner(TokenProcessor *cps);
void eepromDump(TokenProcessor *cps);

void unknownCommand(TokenProcessor *cps);

typedef void(*callback_t)(TokenProcessor *);

callback_t commandFunctions[] = {
#ifdef HELPCMD
    &getHelp, 
#endif
#ifdef DEBUG
    &setDebug, 
#endif
#ifdef LEDCMDTESTER
    &LED13_on, &LED13_off, 
#endif
#ifdef FREERAM
    &printFreeRAM,
#endif  
#ifdef I2CSCANNER
    &i2cScanner, 
#endif
#ifdef EEPROMDUMP
    &eepromDump,
#endif
};

#ifdef HELPCMD

const char helpStrings_h[] PROGMEM = "- help about commands";
const char helpStrings_d[] PROGMEM = "- sets debug"; 
const char helpStrings_1[] PROGMEM = "- led13 on";
const char helpStrings_0[] PROGMEM = "- led13 off";
const char helpStrings_f[] PROGMEM = "- show free ram";
const char helpStrings_i[] PROGMEM = "- i2c bus scanner";
const char helpStrings_e[] PROGMEM = "- dump EEPROM content";

PGM_P const helpStrings[] PROGMEM = {
#ifdef HELPCMD
    helpStrings_h,
#endif
#ifdef DEBUG  
    helpStrings_d,
#endif
#ifdef LEDCMDTESTER  
    helpStrings_1,
    helpStrings_0,
#endif
#ifdef FREERAM  
    helpStrings_f,
#endif
#ifdef I2CSCANNER
    helpStrings_i,
#endif  
#ifdef EEPROMDUMP
    helpStrings_e,
#endif
};
#endif

TokenProcessor tp(Serial, 128, ' ', sizeof(commands), commands, commandFunctions, unknownCommand);

void setup() {
  Serial.begin(115200);
  while (!Serial);
   Serial.println(F("tpOS 1.0\nCompiled with GCC " __VERSION__ " on " __DATE__ " " __TIME__));
}

void loop() {
  tp.process();
  delay(100);
}

void unknownCommand(TokenProcessor *cps) {
  cps->Channel()->println(F("What?"));
  cps->Channel()->print(F("Command '"));
  cps->Channel()->print(cps->getCommand());
  cps->Channel()->println(F("' not found!"));
}

#ifdef HELPCMD
void getHelp(TokenProcessor *cps) {
  char cmd = '\0';
  char buffer[40];

  for (uint16_t i = 0; i < cps->size(); i++) {
    cps->getCommand(i, cmd);
    cps->Channel()->print(cmd);
    cps->Channel()->print(" ");
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(helpStrings[i])));
    cps->Channel()->print(buffer);
    if (i % 2 == 1) {
      cps->Channel()->println();
    } else {
      for (int16_t j = 0; j < 40 - strlen(buffer); j++)
        cps->Channel()->print(" ");
    }
  }
}
#endif

#ifdef DEBUG
void setDebug(TokenProcessor *cps) {
  debug = !debug;
  if (debug) {
    cps->Channel()->println(F("Debug enabled"));
  }
}
#endif

#ifdef LEDCMDTESTER
void LED13_on(TokenProcessor *cps) {
  if (debug) {
    cps->Channel()->println(F("LED on"));
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void LED13_off(TokenProcessor *cps) {
  if (debug) {
    cps->Channel()->println(F("LED off"));
  }
  digitalWrite(LED_BUILTIN, LOW);
}
#endif

#ifdef FREERAM

#if (defined(__AVR__))
inline int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#else
extern "C" {
#include "user_interface.h"
}
inline uint32_t freeRam () {
return system_get_free_heap_size();
}
#endif

void printFreeRAM(TokenProcessor *cps) {
    cps->Channel()->print(F("RAM "));
    cps->Channel()->print(freeRam());
    cps->Channel()->println(F(" Bytes."));
}
#endif

#ifdef I2CSCANNER
void i2cScanner(TokenProcessor *cps) {
  static bool initialized = false;
  uint8_t error;
  uint16_t nDevices = 0;
 
  if (!initialized) {
    Wire.begin();
    initialized = true;
  }
  cps->Channel()->println(F("Scanning I2C bus ..."));
 
  for(uint8_t address = 0; address < 128; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0) {
      cps->Channel()->print(F("Found at address 0x"));
      nDevices++;
    } else if (error==4) {
      cps->Channel()->print(F("Unknow error at address 0x"));
    }
    if ((error == 0) || (error==4)) {
    if (address<16)
      cps->Channel()->print("0");
    cps->Channel()->print(address,HEX);
    }

    delay(1);
  }
  cps->Channel()->print(F("Found "));
  cps->Channel()->print(nDevices, DEC);
  cps->Channel()->println(F(" device(s)."));
}
#endif

#ifdef EEPROMDUMP
void eepromDump(TokenProcessor *cps) {
  char buffer[16];
  uint8_t value;
  uint16_t address;
  uint8_t trailingSpace = 2;
  uint8_t x,y;
  cps->Channel()->print(F("baseAddr "));
  for(x = 0; x < 2; x++) {
    cps->Channel()->print(" ");
    for(y = 0; y < 25; y++)
      cps->Channel()->print("=");
  }
  for(address = 0; address < EEPROM_SIZE; address++) {
    // read a byte from the current address of the EEPROM
    value = EEPROM.read(address);
 
    // add space between two sets of 8 bytes
    if(address % 8 == 0)
      cps->Channel()->print("  ");
 
    // newline and address for every 16 bytes
    if(address % 16 == 0) {
      //print the buffer
      if(address > 0 && address % 16 == 0)
        printASCII(cps, buffer);

      cps->Channel()->print(F("\n 0x0"));
      printAddress(cps, address); 
      cps->Channel()->print(F(": "));
 
      //clear the buffer for the next data block
      memset(buffer, ' ', sizeof(buffer));
    }
 
    // save the value in temporary storage
    buffer[address%16] = value;
 
    // print the formatted value
    cps->Channel()->print(" ");
    if (value < 0x10)
       cps->Channel()->print("0");
    cps->Channel()->print(value, HEX);
   }
 
  if(address % 16 > 0) {
    if(address % 16 < 9)
      trailingSpace += 2;
 
    trailingSpace += (16 - address % 16) * 3;
  }
  
  for(x = trailingSpace; x > 0; x--)
    cps->Channel()->print(" ");
 
  //last line of data and a new line
  printASCII(cps, buffer);
  cps->Channel()->println();
}

void printAddress(TokenProcessor *cps,  uint16_t addr) {
  if (addr < 0x0010) 
    cps->Channel()->print("0");
  if (addr < 0x0100) 
    cps->Channel()->print("0");
  if (addr < 0x1000) 
    cps->Channel()->print("0");
  cps->Channel()->print(addr, HEX); 
}

void printASCII(TokenProcessor *cps,  char *buffer) {
  for(uint8_t i = 0; i < 16; i++) {
     if (i == 8)
        cps->Channel()->print(" ");
     cps->Channel()->print( (buffer[i] > 31 and buffer[i] < 127)  ?  buffer[i] : '.');
  }
}

#endif

#if (defined(ESP8266))
void reboot() {
  ESP.restart();
}
#else
void reboot() {
  WDTCSR = _BV(WDE);
    while (1); // 16 ms
}
#endif