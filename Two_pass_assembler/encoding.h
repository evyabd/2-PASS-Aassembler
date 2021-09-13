#ifndef MAMAN14_ENCODING_H
#define MAMAN14_ENCODING_H

#include "parserInput.h"

void encodeRCmd(Command *command);

void encodeICmd(Command *command, Label *labels);

void encodeJCmd(Command *command, Label *labels);

long getLabelAddress(Label *labels, char *labelName);

void encodeOrder(DataItem **dataPicture, char *orderLine, unsigned long *address);

void encodeAscizOrder(DataItem **dataPicture, char *orderLine, unsigned long *address);

#endif