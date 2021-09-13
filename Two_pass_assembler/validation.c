#include "validation.h"
#include "string.h"

#define DB_MAX 128
#define DH_MAX 32768
#define DW_MAX "2147483648"
#define DW_MAX_MINUS "-2147483648"


ErrorType validateParameters(Label *labels, CmdSubtype type, char **splitedParams);

ErrorType
runCommandValidation(char *command, Label *labels, CmdSubtype type, char *cmdName, char *label,
                     char *params);

ErrorType runOrderValidation(Label *labels, int paramNum, OrderType type, char **params,
                             char *orderName, char *label, char *paramStr);

/*
 * Check whether the name of the order sent is correct.
 *
 * Params:
 * char *order: the order name.
 *
 * Returns:
 * ErrorType status: valid if the name is correct, directive_name if not.
 */
ErrorType checkOrderName(char *order) {
  if (findInArray(order, directive, 6) == true) {
    return valid;
  }
  return directive_name;
}

/*
 * Check whether the name of the register sent is correct.
 *
 * Params:
 * char *reg: the register name.
 *
 * Returns:
 * ErrorType status: valid if the name is correct, register_name if not.
 */
ErrorType checkRegisterName(char *reg) {
  if (findInArray(reg, registerName, 32) == true) {
    return valid;
  }
  return register_name;
}

/*
 * Check that the name of the command sent is correct.
 *
 * Params:
 * char *commandName: the name of the command.
 *
 * Returns:
 * ErrorType status: valid if the name is correct, command_name if not.
 */
ErrorType checkCommandName(char *commandName) {
  if (findInArray(commandName, commandsTable, 27) == true) {
    return valid;
  }
  return command_name;
}

/*
 * Verify the number of parameters by type of command.
 *
 * Params:
 * char *command: the whole command.
 * CmdSubtype: the type of the command.
 *
 * Returns:
 * ErrorType status: valid if the number of registers is exact, number_of_parameters if ain't.
 */
ErrorType checkParamNum(char *command, CmdSubtype type) {
  int check;
  check = getNumOfParamData(command);
  switch (type) {
    case r_arithmetic_cmd:
    case i_arithmetic_cmd:
    case i_branch_cmd: {
      if (check == 3) {
        return valid;
      } else {
        if (check < 3) {
          return missing_param;
        } else {
          return extraneous_param;
        }
      }
    }
    case r_move_command: {
      if (check == 2) {
        return valid;
      } else {
        if (check < 2) {
          return missing_param;
        } else {
          return extraneous_param;
        }
      }
    }
    case j_jump_cmd:
    case J_cmd: {
      if (check == 1) {
        return valid;
      } else {
        if (check < 1) {
          return missing_param;
        } else {
          return extraneous_param;
        }
      }
    }
    case stop_cmd: {
      if (check == 0) {
        return valid;
      } else
        return extraneous_param;
    }
    default:
      return valid;
  }
}

/*
 * Checks the register of the command, in their order and by definition.
 *
 * Params:
 * char *parameters: the parameters string.
 * CmdSubtype: the type of the command.
 * Label *labels:  labels table.
 *
 * Returns:
 * ErrorType status: valid if the command meets the standard, otherwise the correct error type
 */
ErrorType checkParamStandard(char *parameters, Label *labels, CmdSubtype type) {
  char **params;
  ErrorType status;
  params = strSplit(parameters, ',');
  status = validateParameters(labels, type, params);
  freeStringArray(params);
  return status;
}

/* Validate parameters is suitable to command type.
 *
 * Params:
 * Label *labels: labels linked list.
 * CmdSubType type: sub type of cmd.
 * char **params: array string of split parameters.
 *
 * Return:
 * ErrorType status: valid - if parameters suitable to command, otherwise - suitable error.
*/
ErrorType validateParameters(Label *labels, CmdSubtype type, char **params) {
  switch (type) {
    case r_arithmetic_cmd: {
      if ((checkRegisterName(trimStr(*(params))) == valid) &&
          (checkRegisterName(trimStr(*(params + 1))) == valid) &&
          (checkRegisterName(trimStr(*(params + 2))) == valid)) {
        return valid;
      } else {
        return register_name;
      }
    }
    case r_move_command: {
      if ((checkRegisterName(trimStr(*(params))) == valid) &&
          (checkRegisterName(trimStr(*(params + 1))) == valid)) {
        return valid;
      } else
        return register_name;
    }
    case i_arithmetic_cmd: {
      if ((checkRegisterName(trimStr(*(params))) == valid) &&
          (checkParamValue(trimStr(*(params + 1))) == true) &&
          (checkRegisterName(trimStr(*(params + 2))) == valid)) {
        return valid;
      } else if ((checkRegisterName(trimStr(*(params))) != valid) ||
                 (checkRegisterName(trimStr(*(params + 2))) != valid)) {
        return register_name;
      } else if ((checkParamValue(trimStr(*(params + 1))) == false)) {
        return value_out_of_range;
      }
      return wrong_parameters;
    }
    case i_branch_cmd: {
      if ((checkRegisterName(trimStr(*(params))) == valid) &&
          (checkParamValue(trimStr(*(params + 1))) == true) &&
          (isLabelExists(labels, trimStr(*(params + 2))) == true)) {
        return valid;
      } else if ((checkRegisterName(trimStr(*(params))) != valid)) {
        return register_name;
      } else if ((checkParamValue(trimStr(*(params + 1))) != true)) {
        return value_out_of_range;
      } else if ((isLabelExists(labels, trimStr(*(params + 2))) != true)) {
        return label_doesnt_exists;
      }
      return wrong_parameters;
    }
    case j_jump_cmd: {
      if ((checkRegisterName(trimStr(*(params))) == valid) ||
          (isLabelExists(labels, trimStr(*(params))) == true)) {
        return valid;
      } else {
        return wrong_parameters;
      }
    }
    case J_cmd: {
      if (isLabelExists(labels, trimStr(*(params))) == true) {
        return valid;
      } else {
        return label_doesnt_exists;
      }
    }
    default:
      return wrong_parameters;
  }
}

/*
 * Makes sure the commas are written according to the standard use of commas.
 *
 * Params:
 * char* params: string params.
 *
 * Returns:
 * ErrorType status: valid if the command is correct, otherwise - missing comma / multiple comma.
 */
ErrorType checkCommas(char *params) {
  int i;
  char *copy, *param, *iterator;
  /*Check the command word */
  copy = duplicateStr(params);
  iterator = copy;
  while ((param = myStrsep(&iterator, ",")) != NULL) {
    param = trimStr(param);
    /* if found is empty string, it means that args have to , without number. */
    if (strcmp(param, "") == 0) {
      free(copy);
      return multiple_commas;
    }
    for (i = 0; i < strlen(param); i++) {
      if (isspace(param[i])) {
        free(copy);
        return missing_comma;
      }
    }
  }
  free(copy);
  return valid;
}


/*
 * Check if numeric parameter can be represented by 16 bits.
 *
 * Params:
 * char* param: the number in string format.
 *
 * Returns:
 * Boolean status: True if 16 bits can represent the param, false if not.
 */
Boolean checkParamValue(char *param) {
  long check;
  check = atol(param);
  if (-DH_MAX > check || check > (DH_MAX - 1)) {
    return false;
  }
  return true;
}

/*
 * Checks the command by standard.
 *
 * Params:
 * char* command: the whole command.
 * Label *labels: array of all labels.
 *
 * Returns:
 * ErrorType - valid if the command is correct, the specific error if ain't.
 */
ErrorType validateCommand(char *command, Label *labels) {
  ErrorType check;
  CmdSubtype type;
  char *cmdName = NULL, *label, *params = NULL;
  LineParts *lineParts = getCommandParts(command);
  cmdName = lineParts->cmdName;
  label = lineParts->labelName;
  params = lineParts->params;
  type = sortCmd(cmdName);

  if (command[0] == ':') {
    freeLinePart(lineParts);
    return empty_label;
  }

  check = runCommandValidation(command, labels, type, cmdName, label, params);
  freeLinePart(lineParts);
  return check;
}

/* Run all checks for validate commnd.
 *
 * Params:
 * char *command: the whole command.
 * Label *labels: array of all labels.
 * CmSubtype type: the command sub typee.
 * char *cmdName: the order name.
 * char *label: label name if exists.
 * char *params: the whole string of params.
 *
 * Returns:
 * ErrorType check: the correct status of line.
*/
ErrorType
runCommandValidation(char *command, Label *labels, CmdSubtype type, char *cmdName, char *label,
                     char *params) {
  ErrorType check;
  check = checkLabelName(label);
  if (check != valid) {
    return check;
  }

  check = checkCommandName(cmdName);
  if (check != valid) {
    return check;
  }

  if (params != NULL) {
    check = checkCommas(params);
    if (check != valid) {
      return check;
    }

    check = checkParamNum(command, type);
    if (check != valid) {
      return check;
    }

    check = checkParamStandard(params, labels, type);
    if (check != valid) {
      return check;
    }
  }

  return valid;
}

/*
 * Checks whether the label's name meets the standard.
 *
 * Params:
 * char *label: label string.
 *
 * Returns:
 * ErrorType status: valid if the label is correct, label_syntax if not.
 */
ErrorType checkLabelName(char *label) {
  int i;

  if (label == NULL) {
    return valid;
  }

  if (strlen(label) > 31) {
    return label_syntax;
  }
  if (!isalpha(label[0])) {
    return label_syntax;
  }
  if (checkRegisterName(label) == valid || checkCommandName(label) == valid || checkOrderName(label) == valid) {
    return label_syntax;
  }
  for (i = 1; i < strlen(label); i++) {
    if (!isalnum(label[i])) {
      return label_syntax;
    }
  }
  return valid;
}

/*
 * Checks if the line was written correctly
 *
 * Params:
 * char* ascizStr: the asciz param from the command.
 *
 * Returns:
 * ErrorType: valid if the line is correct,
 * otherwise - not_a_printed_symbol if there is syntax error or number_of_parameters if string is null.
 */
ErrorType checkAsciz(char *ascizStr) {
  int i;

  if (ascizStr == NULL) {
    return number_of_parameters;
  }
  ascizStr = trimStr(ascizStr);
  if (ascizStr[0] != '"' || ascizStr[strlen(ascizStr) - 1] != '"') {
    return missing_quotations;
  }
  for (i = 0; i < strlen(ascizStr) - 1; i++) {
    if (!isprint(ascizStr[i])) {
      return not_a_printed_symbol;
    }
  }
  return valid;
}

/*
 * Verifies that the parameter value is correct according to the bit allocation
 *
 * Params:
 * char* param: the number parameter.
 * OrderType type: the type of the directive.
 *
 * Returns:
 * ErrorType error: valid if the value is correct, otherwise - value_out_of_range.
 */
ErrorType checkOrderParamValue(char *param, OrderType type) {
  long check;
  check = atol(param);
  switch (type) {
    case dw: {
      if ((strtol(DW_MAX_MINUS, NULL, 10) <= check) && (check < strtol(DW_MAX, NULL, 10)))
        return valid;
      return value_out_of_range;
    }
    case db: {
      if ((-DB_MAX <= check) && (check < (DB_MAX - 1)))
        return valid;
      return value_out_of_range;
    }
    case dh: {
      if ((-DH_MAX <= check) && (check < (DH_MAX - 1)))
        return valid;
      return value_out_of_range;
    }
    default:
      return value_out_of_range;
  }
  return value_out_of_range;
}

/*
 * Split the items of order and run all checks.
 *
 * Params:
 * char* order: the whole order line.
 * Label *labels: array of all labels.
 *
 * Returns:
 * ErrorType status: valid if the command is correct, otherwise - the specific error.
 */
ErrorType validateOrder(char *order, Label *labels) {
  int paramNum;
  OrderType type;
  ErrorType check = valid;
  char *orderName, *label, *params, **paramStr;
  LineParts *lineParts = getCommandParts(order);
  orderName = lineParts->cmdName;
  label = lineParts->labelName;
  params = lineParts->params;
  paramNum = getNumOfParamData(order);
  type = sortOrder(orderName);

  if (order[0] == ':') {
    return empty_label;
  }

  if (params != NULL) {
    paramStr = strSplit(params, ',');
    check = runOrderValidation(labels, paramNum, type, paramStr, orderName, label, params);
    freeStringArray(paramStr);
  }
  freeLinePart(lineParts);
  return check;
}

/* Run all checks for validate order.
 *
 * Params:
 * Label *labels: array of all labels.
 * int paramNum: number of parameters in order.
 * OrderType type: the order type.
 * char **paramStr: string array of params.
 * char *orderName: the order name.
 * char *label: label name if exists.
 * char *params: the whole string of params.
 *
 * Returns:
 * ErrorType check: the correct status of line.
*/
ErrorType runOrderValidation(Label *labels, int paramNum, OrderType type,char **paramStr,
                             char *orderName, char *label, char *params) {
  int i;
  ErrorType check;
  check = checkLabelName(label);
  if (check != valid) {
    return check;
  }

  check = checkOrderName(orderName);
  if (check != valid) {
    return check;
  }

  if (type == asciz) {
    return checkAsciz(params);
  }

  if (type == entry || type == external) {
    if (paramNum != 1) {
      return number_of_parameters;
    }

    if (isLabelExists(labels, params) == false) {
      return label_doesnt_exists;
    }
    return valid;
  }

  check = checkCommas(params);
  if (check != valid) {
    return check;
  }


  for (i = 0; i < paramNum; i++) {
    check = (checkOrderParamValue(trimStr(*(paramStr + i)), type));
    if (check != valid) {
      return check;
    }
  }

  return valid;
}

/*
 * By Line type call the correct function to validate the line.
 *
 * Params:
 * char* line: the whole line to check.
 * Label* labels: linked list of labels.
 *
 * Returns:
 * ErrorType status: valid if the line meet its settings, if ain't - the specific error will be returned.
 */
ErrorType checkLine(char *line, Label *labels) {
  char *labelName, *cmdName;
  LineParts *lineParts;
  if (isEmptyLine(line) == true || isCommentLine(line) == true) {
    return valid;
  }
  if (isCmdLine(line) == true) {
    return validateCommand(trimStr(line), labels);
  } else if (isOrderLine(line) == true) {
    return validateOrder(trimStr(line), labels);
  } else {
    if (line[0] == ':') {
      return empty_label;
    }
    lineParts = getCommandParts(line);
    labelName = lineParts->labelName;
    cmdName = lineParts->cmdName;
    if (labelName != NULL && cmdName == NULL) {
      freeLinePart(lineParts);
      return label_with_invalid_line;
    }
  }
  freeLinePart(lineParts);
  return command_name;
}

/*
 * Sorts the commands by general type - R, I, or J.
 *
 * Params:
 * char *commandName: the command name.
 *
 * Returns:
 * CmdSubType type: the type of the command, depending on the content
 */
CmdSubtype sortCmd(char *commandName) {
  if (findInArray(commandName, rACmd, 5) == true) {
    return r_arithmetic_cmd;
  }
  if (findInArray(commandName, rMCmd, 3) == true) {
    return r_move_command;
  }
  if (findInArray(commandName, iACmd, 11) == true) {
    return i_arithmetic_cmd;
  }
  if (findInArray(commandName, iBCmd, 4) == true) {
    return i_branch_cmd;
  }
  if (findInArray(commandName, jJCmd, 1) == true) {
    return j_jump_cmd;
  }
  if (findInArray(commandName, jCmd, 2) == true) {
    return J_cmd;
  }
  if (findInArray(commandName, sCmd, 1) == true) {
    return stop_cmd;
  }
  return stop_cmd;
}

/*
 * Sorts the orders by orderName.
 *
 * Param:
 * char *orderName: the order name.
 *
 * Returns:
 * OrderType type: the type of the line, depending on the content
 */
OrderType sortOrder(char *orderName) {
  if (strcmp(orderName, ".db") == 0) {
    return db;
  } else if (strcmp(orderName, ".dh") == 0) {
    return dh;
  } else if (strcmp(orderName, ".dw") == 0) {
    return dw;
  } else if (strcmp(orderName, ".asciz") == 0) {
    return asciz;
  } else if (strcmp(orderName, ".entry") == 0) {
    return entry;
  } else if (strcmp(orderName, ".extern") == 0) {
    return external;
  }
  return external;
}

/* Return correct message by error type.
 *
 * Params:
 * ErrorType type: type of error.
 *
 * Returns:
 * char *message: suitable message to error type.
*/
char *getMessageErrorType(ErrorType type) {
  switch (type) {
    case valid:
      return "valid command";
    case register_name:
      return "invalid register name.";
    case command_name:
      return "invalid command name.";
    case directive_name:
      return "invalid order name.";
    case number_of_parameters:
      return "command did not get correct number of parameters";
    case wrong_parameters:
      return "one or more from the parameters is not suitable to command.";
    case wrong_syntax:
      return "wrong syntax of the command.";
    case value_out_of_range:
      return "one of the parameters value is out of range.";
    case not_a_printed_symbol:
      return "wrong syntax of asciz: asciz param can have only printed symbols.";
    case label_syntax:
      return "wrong label syntax.";
    case missing_comma:
      return "missing comma.";
    case multiple_commas:
      return "multiple commas.";
    case label_extern_and_internal:
      return "label cannot be both external and internal.";
    case label_doesnt_exists:
      return "label does not exists.";
    case missing_param:
      return "missing parameter.";
    case extraneous_param:
      return "There are too many parameters for this command.";
    case missing_quotations:
      return "Asciz should start and enf with quotation mark.";
    case memory_Allocation:
      return "Allocation Error.";
    case empty_label:
      return "empty label is invalid.";
    case label_with_invalid_line:
      return "label defined in invalid or empty line.";
    default:
      return "unknown error occurred.";
  }
}
