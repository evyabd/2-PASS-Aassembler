#include "encoding.h"

/*
 * Encoded R Format bit field of Command node in linked list.
 *
 * Params:
 * Command *command: command node.
*/
void encodeRCmd(Command *command) {
  char *str, *param, *iterator;
  RCommand *bits;
  int rs = 0, rt = 0, rd = 0;

  /* Get param and commandName from Whole command. */
  LineParts *lineParts = getCommandParts(command->line);

  /* Get Opcode and Funct by commandName. */
  int opcode = getOpcodeByCommand(lineParts->cmdName);
  int funct = getFunctByCommand(lineParts->cmdName);

  /* Separate each register and convert it to integer number */
  iterator = duplicateStr(lineParts->params);
  str = iterator;
  param = trimStr(myStrsep(&iterator, ","));

  /* Check if it an arithmetic command or copy command */
  if (opcode == 0) {
    rs = getRegisterIndexFromParam(param);
    param = trimStr(myStrsep(&iterator, ","));
    rt = getRegisterIndexFromParam(param);
    iterator = trimStr(iterator);
    rd = getRegisterIndexFromParam(iterator);
  } else if (opcode == 1) {
    rs = getRegisterIndexFromParam(param);
    iterator = trimStr(iterator);
    rd = getRegisterIndexFromParam(iterator);
  }

  /* Encode command bits. */
  bits = (RCommand *) calloc(1, sizeof(RCommand));
  bits->opcode = opcode;
  bits->funct = funct;
  bits->unused = 0;
  bits->rs = rs;
  bits->rt = rt;
  bits->rd = rd;
  command->bits = bits;

  free(str);
  freeLinePart(lineParts);
}

/*
 * Encoded I Format bit field of Command node in linked list.
 *
 * Params:
 * Command *command: command node.
 * Labels *labels: labels linked list.
*/
void encodeICmd(Command *command, Label *labels) {
  char *iterator, *param, *str;
  int rs, rt;
  long immed;
  long targetAddress;

  /* Get param and commandName from Whole command. */
  LineParts *lineParts = getCommandParts(command->line);
  char *conditionJmpCmd[4] = {"beq", "bne", "blt", "bgt"};

  /* Get Opcode by commandName. */
  int opcode = getOpcodeByCommand(lineParts->cmdName);

  /* Encode command bits. */
  ICommand *bits = (ICommand *) calloc(1, sizeof(ICommand));
  bits->opcode = opcode;

  /* Separate each param and convert it to number */
  iterator = duplicateStr(lineParts->params);
  str = iterator;
  param = trimStr(myStrsep(&iterator, ","));

  rs = getRegisterIndexFromParam(param);

  if (isCommandInArray(4, conditionJmpCmd, lineParts->cmdName) == true) {
    param = trimStr(myStrsep(&iterator, ","));
    rt = getRegisterIndexFromParam(param);
    iterator = trimStr(iterator);
    if (getLabelAddress(labels, iterator) == -1) {
      printf("Labels doesnt exists! \n");
      return;
    }
    targetAddress = getLabelAddress(labels, iterator);
    if (targetAddress - command->address < 0) {
      immed = (long) (command->address - targetAddress);
    } else {
      immed = (long) (targetAddress - command->address);
    }
  } else {
    param = trimStr(myStrsep(&iterator, ","));
    immed = atol(param);
    iterator = trimStr(iterator);
    rt = getRegisterIndexFromParam(iterator);
  }

  bits->rs = rs;
  bits->rt = rt;
  bits->immed = immed;
  command->bits = bits;
  free(str);
  freeLinePart(lineParts);
}

/*
 * Encoded J Format bit field of Command node in linked list.
 *
 * Params:
 * Command *command: command node.
 * Labels *labels: labels linked list.
*/
void encodeJCmd(Command *command, Label *labels) {
  int reg = 0;
  long address = 0;
  int opcode;
  LineParts *lineParts = getCommandParts(command->line);
  /* Get param and commandName from Whole command. */
  char *params = lineParts->params;
  char *cmdName = lineParts->cmdName;

  JCommand *bits = (JCommand *) calloc(1, sizeof(JCommand));

  if (strcmp(cmdName, "stop") == 0) {
    opcode = getOpcodeByCommand(cmdName);
  } else {
    opcode = getOpcodeByCommand(cmdName);
    params = trimStr(params);
    if (isParamRegister(params) == 0) {
      address = getRegisterIndexFromParam(params);
      reg = 1;
    } else {
      if (getLabelAddress(labels, params) == -1) {
        printf("Label does not exists! \n");
        return;
      } else {
        address = getLabelAddress(labels, params);
      }
    }
  }

  bits->opcode = opcode;
  bits->reg = reg;
  bits->address = address;
  command->bits = bits;
  freeLinePart(lineParts);
}

/* Search label by its name in labels array and return its address.
 * if label not found - return -1.
 *
 * Params:
 * Label *labels: labels linked list.
 * char *labelName: the name of the label to search.
 *
 * Returns:
 * long labelAddress: the address of the label (if founds)
*/
long getLabelAddress(Label *labels, char *labelName) {
  Label *lastLabel = labels;
  while (lastLabel != NULL) {
    if (strcmp(labelName, lastLabel->symbol) == 0) {
      return lastLabel->value;
    }
    lastLabel = lastLabel->next;
  }
  return -1;
}

/* Insert data nodes to data picture.
 *
 * Params:
 * DataItem **dataPicture: linked list of all data items.
 * char *orderLine: the line of the order.
 * unsigned long address: the address of the next node in dataPicture.
*/
void encodeOrder(DataItem **dataPicture, char *orderLine, unsigned long *address) {
  char *str, *param, *iterator;
  LineParts *lineParts = getCommandParts(orderLine);
  char *params = lineParts->params ;
  char *cmdName = lineParts->cmdName;
  DataItem *currentItem;
  DataSize itemType = getDataSizeByOrderName(cmdName);
  Data *item = (Data *) calloc(1, sizeof(Data));

  if (item == NULL) {
    freeLinePart(lineParts);
    printf("Error: Allocation Error! \n");
  }

  if (itemType == -1) {
    free(item);
    freeLinePart(lineParts);
    return;
  }

  /* split parameters. */
  str = duplicateStr(params);
  iterator = str;
  while ((param = myStrsep(&iterator, ",")) != NULL) {
    param = trimStr(param);
    item->value = atol(param);
    currentItem = initNewDataItem(atol(param), *address, itemType);
    addNewDataItem(dataPicture, currentItem);
    (*address) += itemType;
  }
  free(str);
  freeLinePart(lineParts);
  free(item);
}

/* Insert data node to data picture for command asciz.
 * because the parameters to encode are coming as string - there is need another implementation.
 *
 * Params:
 * DataItem **dataPicture: linked list of all data items.
 * char *orderLine: the line of the order.
 * unsigned long address: the address of the next node in dataPicture.
*/
void encodeAscizOrder(DataItem **dataPicture, char *orderLine, unsigned long *address) {
  int i = 0;
  LineParts *lineParts = getCommandParts(orderLine);
  char *params = lineParts->params;
  char *stringToEncode = (char *) calloc(strlen(params) - 1, sizeof(char));
  Data *item = (Data *) calloc(1, sizeof(Data));
  DataItem *currentItem = NULL;

  if (stringToEncode == NULL || item == NULL) {
    printf("Error: Allocation Error! \n");
  }

  memcpy(stringToEncode, &params[1], strlen(params) - 2);
  stringToEncode[strlen(params) - 2] = '\0';

  for (i = 0; i <= strlen(stringToEncode); i++) {
    currentItem = initNewDataItem((long) stringToEncode[i], *address, byte);
    addNewDataItem(dataPicture, currentItem);
    (*address) += 1;
  }
  free(stringToEncode);
  free(item);
  freeLinePart(lineParts);
}