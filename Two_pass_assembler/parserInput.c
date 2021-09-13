#include "parserInput.h"

/* Indicate if the line is a comment line.
 *
 * Params:
 * char *line: the line.
 *
 * Returns:
 * Boolean status: true if the line start with ; character, otherwise - return false.
*/
Boolean isCommentLine(char *line) {
  while (isspace((unsigned char) *line)) line++;

  if (*line == ';')
    return true;

  return false;
}

/* Indicate if the command in line is in arr.
 *
 * Params:
 * char *line: the line of the command.
 * char **arr: array of items.
 * int numOfItems: array length.
 *
 * Returns:
 * if the command/order is in array return true, otherwise return false.
*/
Boolean isDirectiveInArray(char *line, char **arr, int length) {
  char *string, *found, *iterator;
  int i;
  line = trimStr(line);
  string = duplicateStr(line);
  iterator = string;
  if (string == NULL) {
    return false;
  }

  found = myStrsep(&iterator, " ");

  if (found[strlen(found) - 1] == ':') {
    found = myStrsep(&iterator, " ");
  }

  if (found == NULL) {
    free(string);
    return false;
  }

  for (i = 0; i < length; i++) {
    if (strcmp(found, *(arr + i)) == 0) {
      free(string);
      return true;
    }
  }

  free(string);
  return false;
}

/* Indicate if the line is an order line.
 *
 * Params:
 * char *line: the line.
 *
 * Returns:
 * Boolean status: true if the command in line is one of orders in the array, otherwise - return false.
*/
Boolean isOrderLine(char *line) {
  char *orders[7] = {".dd", ".dw", ".db", ".dh", ".asciz", ".entry", ".extern"};
  return isDirectiveInArray(line, orders, 7);
}

/* Indicate if the line is a command line.
 *
 * Params:
 * char *line: the line.
 *
 * Returns:
 * Boolean status: if the command in line is one of command in the array, otherwise - return false.
*/
Boolean isCmdLine(char *line) {
  char *commands[27] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
                        "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt",
                        "bgt", "lb", "sb", "lw", "sw", "lh", "sh", "jmp", "la", "call", "stop"};
  return isDirectiveInArray(line, commands, 27);
}

/* Get the type of the line.
 *
 * Params:
 * char *line: the line.
 *
 * Returns:
 * LineType type: the type of the line.
*/
LineType getLineType(char *line) {
  if (!isEmptyLine(line)) {
    return blank_line;
  } else if (!isCommentLine(line)) {
    return comment_line;
  } else if (!isOrderLine(line)) {
    return order_line;
  } else if (!isCmdLine(line)) {
    return cmd_line;
  }
  return invalid_line;
}

/* Get register number by register string param in format of "$<registerNum>".
 *
 * Params:
 * char *param: register represent as param string in the command.
 *
 * Returns:
 * int index: the index of register in the registerTable.
*/
int getRegisterIndexFromParam(char *param) {
  char *index = calloc(strlen(param), sizeof(char));
  int reg;
  if (index == NULL) {
    printf("Error: Allocation Error! \n");
    return -1;
  }
  memcpy(index, &param[1], strlen(param) - 1);
  index[strlen(param) - 1] = '\0';
  reg = atoi(index);
  free(index);
  return reg;
}

/* Get command name and return its index in commands array.
 *
 * Params:
 * char *command : string of command name.
 *
 * Returns:
 * int index: the of command in commands array.
*/
int getIndexOfCommand(char *command) {
  int i = 0;
  for (i = 0; i < 27; i++) {
    if (strcmp(command, commandsTable[i]) == 0) {
      return i;
    }
  }
  return -1;
}

/* Return command's item from array of items by command's index.
 *
 * Params:
 * int length: the length of the array of items.
 * int *arr: array of command's item.
 * char *command: command name.
 *
 * Returns:
 * char *item - the item of the command from the array of items as binary string.
*/
int getItemByCommand(int *arr, char *command) {
  int index = getIndexOfCommand(command);
  return arr[index];
}

/* Get the opcode of command.
 *
 * Params:
 * char *command: command name.
 *
 * Returns:
 * char *opcode : the opcode of the command as binary string.
*/
int getOpcodeByCommand(char *command) {
  return getItemByCommand(opcodes, command);
}

/* Get the funct of command.
 *
 * Params:
 * char *command: command name.
 *
 * Returns:
 * int funct : the funct of the command as binary string.
*/
int getFunctByCommand(char *command) {
  return getItemByCommand(functs, command);
}

/* Get Data size (byte, half-word, or word) by command name.
 *
 * Params:
 * char *cmdName: command name.
 *
 * Returns:
 * DataSize size: the size of data in encoding.
*/
DataSize getDataSizeByOrderName(char *cmdName) {
  if (strcmp(cmdName, ".db") == 0 || strcmp(cmdName, ".asciz") == 0) {
    return byte;
  } else if (strcmp(cmdName, ".dh") == 0) {
    return half_word;
  } else if (strcmp(cmdName, ".dw") == 0) {
    return word;
  }
  return -1;
}

/* Get the number of parameters in command.
 *
 * Params:
 * char *command: the whole command.
 *
 * Returns:
 * int numOfParams: number of params in command.
*/
int getNumOfParamData(char *command) {
  char *params, *line, *cmd, *iterator;
  int numOfParams = 0;
  LineParts *lineParts = getCommandParts(command);
  cmd = lineParts->cmdName;
  params = lineParts->params;

  /* In this command the string come in one chunk with quotations. */
  if (strcmp(cmd, ".asciz") == 0) {
    /* Ignore quotations */
    numOfParams = (int) (strlen(params) - 2);
    freeLinePart(lineParts);
    return numOfParams;
  }

  if (params == NULL) {
    freeLinePart(lineParts);
    return 0;
  }

  line = duplicateStr(params);
  if (line == NULL) {
    freeLinePart(lineParts);
    return -1;
  }

  iterator = line;
  while (myStrsep(&iterator, ",") != NULL) {
    numOfParams++;
  }

  free(line);
  freeLinePart(lineParts);
  return numOfParams;
}

/* Check if label is already exists by its name.
 *
 * Params:
 * Label *labels: label table.
 * char *labelName: name of the label we want to check if exists.
 *
 * Returns:
 * Boolean status: true if label exists, otherwise - false.
*/
Boolean isLabelExists(Label *labels, char *labelName) {
  Label *lastLabel = labels;
  while (lastLabel != NULL) {
    if (strcmp(lastLabel->symbol, labelName) == 0) {
      return true;
    }
    lastLabel = lastLabel->next;
  }
  return false;
}

/* Check if param string is an register ot not.
 *
 * Params:
 * char *param: string represent one slice of param string from params.
 *
 * Returns:
 * Boolean isRegister: if it register return true, otherwise return false.
 */
Boolean isParamRegister(char *param) {
  int reg = getRegisterIndexFromParam(param);
  if (reg < 32 && reg > 0) {
    return true;
  }
  return false;
}


