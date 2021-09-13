#include "Datatypes.h"

/* Add new label to labels array.
 *
 * Params:
 * Label **labels: pointer to labels linked list.
 * Label *label: label to add the array.
*/
void addNewLabel(Label **labels, Label *label) {
  if (*labels == NULL) {
    *labels = label;
  } else {
    Label *lastLabel = *labels;
    while (lastLabel->next != NULL) {
      lastLabel = lastLabel->next;
    }
    lastLabel->next = label;
  }

}

/* Initialize new label.
 *
 * Params:
 * char *symbol: the symbol of the label.
 * int value: the value of the label.
 * Attributes attr: the attributes of the label.
 *
 * Returns:
 * Label *label: the new label with the parameters to init.
*/
Label *initLabel(char *symbol, unsigned long value, Attributes *attr) {
  Label *newLabel = calloc(1, sizeof(Label));
  if (newLabel == NULL) {
    return NULL;
  }

  newLabel->symbol = (char *) calloc(strlen(symbol) + 1, sizeof(char));
  if (newLabel->symbol == NULL) {
    return NULL;
  }
  strcpy(newLabel->symbol, symbol);
  newLabel->value = value;
  newLabel->attr = *attr;
  newLabel->appearances = NULL;
  newLabel->appearancesLength = 0;
  newLabel->next = NULL;
  return newLabel;
}

/*
 * Prints all the errors to the stderr file.
 *
 * Params:
 * Error **error: pointer to error's linked list.
 */
void printErrorStruct(Error **errors) {
  Error *lastError = *errors;
  while (lastError != NULL) {
    fprintf(stderr, "Error! in line: %d: %s \n", lastError->lineNumber, lastError->message);
    lastError = lastError->next;
  }
}

/* Initialize new Command node for new line of command.
 *
 * Params:
 * char *commandLine: the whole line of the command.
 * unsigned long address: the address of the command.
 *
 * Returns:
 * Command command: node of the correct type of command.
*/
Command *initNewCommand(char *commandLine, unsigned long address) {
  LineParts *lineParts;
  Command *command;
  CmdType commandType;

  lineParts = getCommandParts(commandLine);
  command = (Command *) calloc(1, sizeof(Command));

  if (command == NULL) {
    printf("Error: Allocation Error! \n");
    return NULL;
  }

  command->line = (char *) calloc(strlen(commandLine) + 1, sizeof(char));
  commandType = getCmdTypeByCommandName(lineParts->cmdName);

  command->address = address;
  strcpy(command->line, commandLine);
  command->type = commandType;
  command->next = NULL;
  freeLinePart(lineParts);
  return command;
}

/* Add new command node to the end of the commands linked list.
 *
 * Params:
 * Command **commands: pointer to linked list of commands.
 * Command *command: pointer to the command node that want to add.
*/
void addNewCommand(Command **commands, Command *command) {
  if (*commands == NULL) {
    *commands = command;
  } else {
    Command *lastCommand = *commands;
    while (lastCommand->next != NULL) {
      lastCommand = lastCommand->next;
    }
    lastCommand->next = command;
  }
}

/* Initialize new Data Item node.
 *
 * Params:
 * long value: the value of the data.
 * unsigned long address: the address of this data in data picture.
 * DataSize size: how much bits required.
 *
 * Returns:
 * DataItem *item: pointer to the new data item node.
*/
DataItem *initNewDataItem(long value, unsigned long address, DataSize size) {
  DataItem *dataItem = (DataItem *) calloc(1, sizeof(DataItem));
  if (dataItem == NULL) {
    printf("Error: Allocation Error! \n");
  }

  (dataItem->item).value = value;
  dataItem->address = address;
  dataItem->next = NULL;
  dataItem->size = size;
  return dataItem;
}

/* Insert new data item node to the end of data pictur.
 *
 * Params:
 * DataItem **dataPicture: pointer to linked list of data picture.
 * DataItem *dataItem: pointer to the new node.
*/
void addNewDataItem(DataItem **dataPicture, DataItem *dataItem) {
  if (*dataPicture == NULL) {
    *dataPicture = dataItem;
  } else {
    DataItem *lastDataItem = *dataPicture;
    while (lastDataItem->next != NULL) {
      lastDataItem = lastDataItem->next;
    }
    lastDataItem->next = dataItem;
  }
}

/* Initialize new error.
 *
 * Params:
 * char *message: the message for print error.
 * int numberLine: the number of the line when error occurred.
 *
 * Returns:
 * Label *label: the new label with the parameters to init.
*/
Error *initNewError(char *message, int numberLine) {
  Error *error = (Error *) calloc(1, sizeof(Error));
  error->message = (char *) calloc(strlen(message) + 1, sizeof(char));

  if (error == NULL || error->message == NULL) {
    printf("Error: Allocation Error! \n");
  }

  strcpy(error->message, message);
  error->lineNumber = numberLine;
  error->next = NULL;

  return error;
}

/* Add new error to errors array.
 *
 * Params:
 * Label *errors: pointer to errors linked list.
 * Label *error: error to add the array.
*/
void addNewError(Error **errors, Error *error) {
  if (*errors == NULL) {
    *errors = error;
  } else {
    Error *lastError = *errors;
    while (lastError->next != NULL) {
      lastError = lastError->next;
    }
    lastError->next = error;
  }
}

/* Mark the label with labelName as Entry.
 *
 * Params:
 * Label **label: pointer to linked lise of labels.
 * char *labelName: the name of the label we want to mark.
*/
void markLabelAsEntry(Label **labels, char *labelName) {
  Label *lastLabel = *labels;
  while (lastLabel != NULL) {
    if (strcmp(lastLabel->symbol, labelName) == 0) {
      lastLabel->attr.isEntry = true;
      return;
    }
    lastLabel = lastLabel->next;
  }
}

/* Update data labels by adding ICF.
 *
 * Params:
 * Label **label: pointer to linked lise of labels.
 * int ICF: the starting address of data segment.
*/
void updateDataLabels(Label **labels, int ICF) {
  Label *lastLabel = *labels;
  while (lastLabel != NULL) {
    if (lastLabel->attr.isData == true) {
      lastLabel->value += ICF;

    }
    lastLabel = lastLabel->next;
  }
  return;
}

/* Add to label with name of labelName appearance.
 *
 * Params:
 * Label **label: pointer to linked lise of labels.
 * char *labelName: the name of the label we want to mark.
 * unsigned long addressAppearance: the address of appearance of label.
*/
void addLabelAppearance(Label **labels, char *labelName, unsigned long addressAppearance) {
  Label *lastLabel = *labels;
  while (lastLabel != NULL) {
    if (strcmp(lastLabel->symbol, labelName) == 0) {
      if (lastLabel->appearancesLength == 0) {
        lastLabel->appearances = (unsigned long *) calloc(1, sizeof(int));
      } else {
        lastLabel->appearances = (unsigned long *) realloc(lastLabel->appearances,
                                                           (lastLabel->appearancesLength + 1) * sizeof(int));
      }

      if (lastLabel->appearances == NULL) {
        printf("Error: Allocation Error! \n");
        return;
      }

      lastLabel->appearances[lastLabel->appearancesLength] = addressAppearance;
      lastLabel->appearancesLength++;
      return;
    }
    lastLabel = lastLabel->next;
  }
}

/* Pass commands list, check if labelName is appear in command from type branch/jmp -
 * if it appears- add this appearance to label.
 *
 * Params:
 * Command **command: pointer to linked list of command.
 * Label **labels: pointer to linked list of labels.
 * char *labelName: the name of label we want to update its appearances.
*/
void updateLabelAppearances(Command **commands, Label **labels, char *labelName) {
  char *cmdName, **paramsArr;
  Command *lastCommand = *commands;
  LineParts *lineParts;

  while (lastCommand != NULL) {
    lineParts = getCommandParts((char *) lastCommand->line);
    cmdName = lineParts->cmdName;
    if (lineParts->params != NULL) {
      paramsArr = strSplit(lineParts->params, ',');
      if (findInArray(cmdName, jCmd, 2) == true) {
        if (strcmp(*(paramsArr), labelName) == 0) {
          addLabelAppearance(labels, labelName, lastCommand->address);
        }

      } else if (findInArray(cmdName, jJCmd, 1) == true) {
        if (strcmp(*(paramsArr), labelName) == 0) {
          addLabelAppearance(labels, labelName, lastCommand->address);
        }
      } else if (findInArray(cmdName, iBCmd, 4) == true) {
        if (strcmp(*(paramsArr + 2), labelName) == 0) {
          addLabelAppearance(labels, labelName, lastCommand->address);
        }
      }
      freeStringArray(paramsArr);
    }
    freeLinePart(lineParts);
    lastCommand = lastCommand->next;
  }
}

/* Pass on labels linked list, and update appearances for every label that marked as external.
 *
 * Params:
 * Command **command: pointer to linked list of command.
 * Label **labels: pointer to linked list of labels.
*/
void updateExternalAppearancesLabels(Command **commands, Label **labels) {
  Label *lastLabel = *labels;
  while (lastLabel != NULL) {
    if (lastLabel->attr.isExternal == true) {
      updateLabelAppearances(commands, labels, lastLabel->symbol);
    }
    lastLabel = lastLabel->next;
  }
}

/* Reset Attributes to default (all of them false).
 *
 * Params:
 * Attributes *attributes: attributes we want to reset.
*/
void resetAttributes(Attributes *attributes) {
  attributes->isCode = false;
  attributes->isEntry = false;
  attributes->isExternal = false;
  attributes->isData = false;
}

/* Get the type of command (R, I Or J) by command name.
 *
 * Params:
 * char *commandName: the command name.
 *
 * Returns:
 * CmdType type: the type of the command.
*/
CmdType getCmdTypeByCommandName(char *commandName) {
  char *r_commands[8] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"};
  char *i_commands[15] = {"addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt",
                          "bgt", "lb", "sb", "sw", "lw", "lh", "sh"};
  char *j_commands[4] = {"jmp", "la", "call", "stop"};

  if (isCommandInArray(8, r_commands, commandName) == true) {
    return r_cmd;
  } else if (isCommandInArray(15, i_commands, commandName) == true) {
    return i_cmd;
  } else if (isCommandInArray(4, j_commands, commandName) == true) {
    return j_cmd;
  }
  return -1;
}

/* Get Item (label / command / params) from command.
 *
 * Params:
 * char *command: the whole command.
 * CommandItem item: enum represent which part from command we want to read.
 *
 * Returns:
 * char *item: substring from command represent item(commandName/ label/ params).
*/
LineParts *getCommandParts(char *command) {
  char *string = NULL, *found = NULL, *iterator = NULL;
  size_t found_len;
  LineParts *lineParts;

  if (isEmptyLine(command) == true) {
    return NULL;
  }

  lineParts = (LineParts *) calloc(1, sizeof(LineParts));

  if (lineParts == NULL) {
    return NULL;
  }

  command = trimStr(command);
  string = duplicateStr(command);

  if (string == NULL) {
    freeLinePart(lineParts);
    return NULL;
  }


  iterator = string;
  /* Fill the part of the command. */
  found = trimStr(myStrsep(&iterator, " "));
  found_len = strlen(found);
  if (found_len == 0) {
    printf("\"%c\" %d \n", found[found_len - 1], found_len);
  }

  if (found[found_len - 1] == ':') {
    lineParts->labelName = (char *) calloc(found_len, sizeof(char));
    if (lineParts->labelName == NULL) {
      printf("Error: Allocation error \n");
      return NULL;
    }
    memcpy(lineParts->labelName, found, found_len - 1);
    lineParts->labelName[found_len - 1] = '\0';
    found = trimStr(myStrsep(&iterator, " "));
  }

  lineParts->cmdName = duplicateStr(found);
  found = trimStr(myStrsep(&iterator, "\r\n"));
  lineParts->params = duplicateStr(found);
  free(string);
  return lineParts;
}

/* Check if commandName is exists in the array.
 *
 * Params:
 * int numOFCommand: num of string commands.
 * char commands[numOfCommands][5]: commands array.
 * char *commandName: the command name.
 *
 * Returns:
 * Boolean status: true - if command exists in array, otherwise - false.
*/
Boolean isCommandInArray(int numOfCommands, char **commands, char *commandName) {
  int i = 0;
  for (i = 0; i < numOfCommands; i++) {
    if (strcmp(commandName, *(commands + i)) == 0) {
      return true;
    }
  }
  return false;
}

/*
 * Check if the name is in the array.
 *
 * Params:
 * char *command: the command name.
 * int first_length: The length of the outer array.
 * int second_length: The length of the inner array.
 * char arr: The array in which we look for a word.
 *
 * Returns:
 * Boolean status: True if the word was found, otherwise - false.
 */
Boolean findInArray(char *command, char **arr, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (strcmp(command, *(arr + i)) == 0) {
      return true;
    }
  }
  return false;
}

/* Indicate if the line is an Empty line.
 *
 * Params:
 * char *line: the line.
 *
 * Returns:
 * Boolean status: true if the line contains only blank chars, otherwise - return false.
*/
Boolean isEmptyLine(char *line) {
  while (isspace((unsigned char) *line)) line++;

  if (*line == 0)
    return true;

  return false;
}

void freeLinePart(LineParts *lineParts) {
  if (lineParts == NULL) {
    return;
  }

  if (lineParts->labelName != NULL) {
    free(lineParts->labelName);
  }

  if (lineParts->cmdName != NULL) {
    free(lineParts->cmdName);
  }

  if (lineParts->params != NULL) {
    free(lineParts->params);
  }

  free(lineParts);
}