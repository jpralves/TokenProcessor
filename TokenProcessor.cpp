/*
 * Library Name: TokenProcessor
 *
 * Filename: TokenProcessor.cpp
 * Description: library TokenProcessor implementation
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

#include "TokenProcessor.h"

TokenProcessor::TokenProcessor(Stream& stream,  int buffer_Size, char delim, int command_List_Size,
                               const char *command_List, void(**callback_Functions)(TokenProcessor *), void(*defaultHandler)(TokenProcessor *)) :
  stream(&stream), bufferSize(buffer_Size), commandListSize(command_List_Size), commandList(command_List), callbackFunctions(callback_Functions), defaultHandler(defaultHandler)
{
  _delim[0] = delim;
  _delim[1] = '\0';
  buffer = (char *)malloc(bufferSize * sizeof(char));
  clearBuffer();
}

TokenProcessor::~TokenProcessor() {
  free(buffer);
}

void TokenProcessor::clearBuffer() {
  memset(buffer, '\0', bufferSize);
//  for (byte i = 0; i < bufferSize; i++) {
//    buffer[i] = '\0';
//  }
  bufferPos = 0;
}

#define BACKSPACE 8

void TokenProcessor::process() {
  char c;
  while (stream->available() > 0) {
    c = stream->read();
    
    stream->print(c);
    if (c == '\n' || c == '\r') {
      processCommand(buffer);
      clearBuffer();
      break;
    }
    if (c == BACKSPACE && bufferPos) {
      bufferPos--;
      buffer[bufferPos] = '\0';
      stream->print(F(" \b"));

    }
    if (isprint(c)) {
      buffer[bufferPos++] = c;
      buffer[bufferPos] = '\0';
      if (bufferPos > bufferSize - 1)
        bufferPos = 0;
    }
  }
}

char *TokenProcessor::nextToken() {
  char *nextToken;
  nextToken = strtok_r(NULL, _delim, &save_ptr);
  return nextToken;
}

void TokenProcessor::processCommand(char *buffer) {
  boolean found = false;

  token = strtok_r(buffer, _delim, &save_ptr);
  if (token == NULL)
    return;
  lastCommand = token[0];
  for (byte i = 0; i < commandListSize; i++) {
    found = (token[0] == commandList[i]);
    if (found && callbackFunctions[i] != NULL) {
      callbackFunctions[i](this);
      break;
    }
  }
  if (!found && defaultHandler != NULL) {
    defaultHandler(this);
  }
}

void TokenProcessor::getCommand(int p, char &cmd) {
  if (p<commandListSize)
    cmd = commandList[p];
}