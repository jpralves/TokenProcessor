/*
 * Library Name: TokenProcessor
 *
 * Filename: TokenProcessor.h
 * Description: library TokenProcessor interface
 *
 * Version: 1.0.0
 * Author: Joao Alves <jpralves@gmail.com>
 * Required files: TokenProcessor.cpp, TokenProcessor.h
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

#ifndef TOKENPROCESSOR_H
#define TOKENPROCESSOR_H

#include <stddef.h>
#include <stdint.h>

#if (defined(__AVR__))
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include "Arduino.h"
#include "WString.h"
#include "Stream.h"

//#define DEBUGCODE

class TokenProcessor {
  public:
    TokenProcessor(Stream& , int, char , int, const char *, void(**)(TokenProcessor *), void (*)(TokenProcessor *));
    ~TokenProcessor();
    TokenProcessor(const TokenProcessor&);
    TokenProcessor& operator=(const TokenProcessor&);

    char *nextToken();
    void clearBuffer();
    
    char getCommand() { return lastCommand; }
    
    void process();

    Stream *Channel() {
      return stream;
    }
    void processCommand(char *);
    
    uint16_t size() {
      return commandListSize;
    };
    
    void getCommand(int, char &);
  private:
    Stream* const stream;
    int bufferSize;
    int commandListSize = 0;
    const char *commandList;
    void(**callbackFunctions)(TokenProcessor *);

    char *buffer;
    int bufferPos;
    void (*defaultHandler)(TokenProcessor *) = NULL;
    char _delim[2];
    char *token;
    char *save_ptr;
    char lastCommand = 0;
};

#endif