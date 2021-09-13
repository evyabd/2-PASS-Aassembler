#ifndef TAR2_PARSERINPUT_H
#define TAR2_PARSERINPUT_H

#include "stringExtension.h"
#include "Datatypes.h"
#include "constants.h"


typedef enum {
    blank_line,
    comment_line,
    order_line,
    cmd_line,
    invalid_line
} LineType;



Boolean isCommentLine(char *line);

Boolean isOrderLine(char *line);

Boolean isCmdLine(char *line);

LineType getLineType(char *line);

Boolean isDirectiveInArray(char *line, char **arr, int length);

int getRegisterIndexFromParam(char *param);

int getIndexOfCommand(char *command);

int getItemByCommand(int *arr, char *command);

int getOpcodeByCommand(char *command);

int getFunctByCommand(char *command);

char *readLabel(char *command);

int getNumOfParamData(char *command);

Boolean isLabelExists(Label *labels, char *labelName);

Boolean isParamRegister(char *param);

DataSize getDataSizeByOrderName(char *cmdName);

#endif