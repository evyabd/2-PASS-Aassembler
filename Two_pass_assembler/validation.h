#ifndef MAMAN14_VALIDATION_H
#define MAMAN14_VALIDATION_H


#include "Datatypes.h"
#include "parserInput.h"
#include "constants.h"
#include "stringExtension.h"


ErrorType checkRegisterName(char *reg);

ErrorType checkCommandName(char *commandName);

ErrorType checkParamNum(char *command, CmdSubtype type);

ErrorType checkLabelName(char *label);

ErrorType checkParamStandard(char *command, Label *labels, CmdSubtype type);

ErrorType checkCommas(char *params);

Boolean checkParamValue(char *param);

ErrorType checkOrderParamValue(char *param, OrderType type);

ErrorType validateCommand(char *command, Label *labels);

CmdSubtype sortCmd(char *commandName);

ErrorType checkAsciz(char *ascizStr);

ErrorType validateOrder(char *order, Label *labels);

ErrorType checkOrderName(char *order);

OrderType sortOrder(char *orderName);

ErrorType checkLine(char *line, Label *labels);

char *getMessageErrorType(ErrorType type);

#endif