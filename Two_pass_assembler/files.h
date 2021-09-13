#ifndef MAMAN14_FILES_H
#define MAMAN14_FILES_H

#include "Datatypes.h"
#include "parserInput.h"


char *createStrFromBitField(unsigned int item, const int length, char *buf);

FILE *writeCommandIntoObjectFile(Command **commands, FILE *fptr);

void writeOrderIntoObjectFile(DataItem **dataPicture, FILE *fptr);

void createObjectFile(char *filename, Command **commands, DataItem **dataPicture, unsigned long ICF, unsigned long IDF);

void createEntryFile(Label *labels, char *filename);

long cut(const char *buf, int begin, int end);

void createExternalFile(Label *labels, char* filename);

char *changeFileName(char* name, char *ext);

Boolean isAsFile(char *filename);


#endif