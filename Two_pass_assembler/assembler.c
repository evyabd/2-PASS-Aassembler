#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parserInput.h"
#include "encoding.h"
#include "validation.h"
#include "files.h"


#define MAX_LINE_LENGTH 80

char *allocateMemory(size_t length);

int getLine(FILE *stream, char *line, int length);

void
pass1(Label **labels, Error **errors, int *numOfErrors, Command **commands, DataItem **dataPicture, unsigned long *IC,
      unsigned long *DC,
      FILE *stream);

void validateFile(FILE *stream, Error **errors, Label *labels, int *numOfErrors);

void updateDataPictureAddress(DataItem **dataPicture, unsigned long ICF);

void pass2(Command **commands, Label *labels);

void freeCommands(Command **commands);

void freeErrors(Error **errors);

void freeDataPicture(DataItem **dataPicture);

void freeLabels(Label **labels);

int main(int args, char *argv[]) {
  char *filename = NULL;
  FILE *fptr = NULL;
  Label *labels = NULL;
  Error *errors = NULL;
  Command *commands = NULL;
  DataItem *dataPicture = NULL;
  unsigned long IC = 100;
  unsigned long DC = 0;
  unsigned long ICF;
  unsigned long DCF;
  int numOfErrors = 0;
  int assemblerIndex = 1;

  for (assemblerIndex = 1; assemblerIndex < args; assemblerIndex++) {
    /* Ensure you have filename as the first argument. */
    if (argv[assemblerIndex] == NULL) {
      printf("No file specified!");
      continue;
    }

    if (isAsFile(argv[assemblerIndex]) != 0) {
      printf("File is not as file");
      continue;
    }

    filename = allocateMemory(strlen(argv[assemblerIndex]));
    strcpy(filename, argv[assemblerIndex]);

    /* Open the file. */
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
      printf("Cannot open file \n");
      exit(1);
    }

    pass1(&labels, &errors, &numOfErrors, &commands, &dataPicture, &IC, &DC, fptr);
    validateFile(fptr, &errors, labels, &numOfErrors);
    ICF = IC;
    DCF = DC + ICF;

    if (numOfErrors == 0) {
      updateDataPictureAddress(&dataPicture, ICF);
      pass2(&commands, labels);
      updateExternalAppearancesLabels(&commands, &labels);
      createObjectFile(filename, &commands, &dataPicture, ICF, DCF);
      createEntryFile(labels, filename);
      createExternalFile(labels, filename);
    } else {
      printErrorStruct(&errors);
    }
    IC = 100;
    DC = 0;
    ICF = 0;
    DCF = 0;
    numOfErrors = 0;
    freeCommands(&commands);
    freeDataPicture(&dataPicture);
    freeLabels(&labels);
    freeErrors(&errors);
    free(filename);
    fclose(fptr);
  }

  return 0;
}

/* Allocate memory to string.
 *
 * Params:
 * char *str: pointer to the string for allocating it memory.
 * int length: number of char to allocate.
 *
 * Returns:
 * char *str: pointer to the allocated memory.
*/
char *allocateMemory(size_t length) {
  char *str = (char *) calloc(length + 1, sizeof(char));
  if (str == NULL) {
    printf("Error: Allocation error!");
    return NULL;
  }
  return str;
}

/* Get next line from file.
 *
 * Params:
 * FILE *stream: pointer to file (file is already open).
 * char *line: array to copy the line into it.
 * size_t length: max number of read from the line (read until \n but if the line length is more than length, read length chars).
 *
 * Returns:
 * int length: num of char readed in the line.
*/
int getLine(FILE *stream, char *line, int length) {
  int cnt;
  int c = 0;

  if (length == 0)
    return 0;

  for (cnt = 0; c != '\n' && cnt < length - 1; cnt++) {
    c = getc(stream);

    if (c == EOF) {
      if (cnt == 0)
        return -1;
      break;
    }

      line[cnt] = (char) c;
  }
    line[cnt] = '\0';
  return cnt + 1;
}

/* Pass 1 of the assembler, read line by line and create labels table, and encode the orders.
 *
 * Params:
 * Label **labels: labels table.
 * Error *error: error array.
 * int *IC: pointer to Instruction Counter.
 * int *DC: pointer to Data Counter.
 * FILE *stream: pointer to file (file already open).
*/
void
pass1(Label **labels, Error **errors, int *numOfErrors, Command **commands, DataItem **dataPicture, unsigned long *IC,
      unsigned long *DC,
      FILE *stream) {
  int read;
  LineType type;
  char *labelName;
  size_t len = 81;
  Label *label;
  char *cmd;
  char *params;
  Command *command;
  Error *error;
  int numOfLines = 0;
  char line[81];
  LineParts *lineParts;
  char *errorMsg = (char *) calloc(100, sizeof(char));
  Attributes *attributes = (Attributes *) calloc(1, sizeof(Attributes));
  if (attributes == NULL || errorMsg == NULL) {
    printf("Allocation Error!");
    exit(1);
  }

  /* Build labels table. */
  while ((read = getLine(stream, line, MAX_LINE_LENGTH + 1)) != -1) {
    if (read > MAX_LINE_LENGTH) {
      numOfLines++;
      error = initNewError("line length is over than 80. \n", numOfLines);
      addNewError(errors, error);
      (*numOfErrors)++;
      read = getLine(stream, line, MAX_LINE_LENGTH + 1);
      continue;
    }
    type = getLineType(line);
    resetAttributes(attributes);
    if (type == invalid_line) {
      numOfLines++;
      continue;
    }
    if (type == blank_line || type == comment_line) {
      numOfLines++;
      continue;
    } else {
      lineParts = getCommandParts(line);
      labelName = lineParts->labelName;
      if (labelName != NULL) {
        if (type == order_line) {
          attributes->isData = true;
          label = initLabel(labelName, *DC, attributes);
        } else if (type == cmd_line) {
          attributes->isCode = true;
          label = initLabel(labelName, *IC, attributes);
        }
        addNewLabel(labels, label);
      }

      if (type == order_line) {
        cmd = lineParts->cmdName;
        if (strcmp(cmd, ".extern") == 0 || strcmp(cmd, ".entry") == 0) {
          numOfLines++;
          freeLinePart(lineParts);
          continue;
        } else if (strcmp(cmd, ".asciz") == 0) {
          encodeAscizOrder(dataPicture, line, DC);
        } else {
          encodeOrder(dataPicture, line, DC);
        }
      } else if (type == cmd_line) {
        command = initNewCommand(line, *IC);
        addNewCommand(commands, command);
        *IC += 4;
      }
      freeLinePart(lineParts);
    }
    numOfLines++;

  }

  /* Back to the start of the file*/
  fseek(stream, 0, SEEK_SET);
  numOfLines = 0;

  /* Read line by line, and mark each label that pass to entry/external command*/
  while ((read = getLine(stream, line, len)) != -1) {
    if (read > MAX_LINE_LENGTH) {
      numOfLines++;
      read = getLine(stream, line, MAX_LINE_LENGTH + 1);
      continue;
    }
    resetAttributes(attributes);
    lineParts = getCommandParts(line);
    cmd = lineParts->cmdName;
    if (cmd == NULL) {
      freeLinePart(lineParts);
      continue;
    }

    params = lineParts->params;
    if (strcmp(cmd, ".extern") == 0) {
      if (isLabelExists(*labels, params) == true) {
        sprintf(errorMsg, "The label: %s, label that exist this file could not be external! \n", params);
        error = initNewError(errorMsg, numOfLines);
        addNewError(errors, error);
        (*numOfErrors)++;
        numOfLines++;
        freeLinePart(lineParts);
        continue;
      }
      attributes->isExternal = true;
      label = initLabel(params, 0, attributes);
      addNewLabel(labels, label);
    } else if (strcmp(cmd, ".entry") == 0) {
      markLabelAsEntry(labels, params);
    }
    numOfLines++;
    freeLinePart(lineParts);
  }

  /* add each data label ICF */
  updateDataLabels(labels, *IC);
  free(attributes);
  free(errorMsg);
}

/* Pass 2 - encoded each command in the linked list.
 *
 * Params:
 * Command **commands: linked list of commands.
 * Label *label: array of labels.
*/
void pass2(Command **commands, Label *labels) {
  Command *head = *commands;

  while (head != NULL) {
    if (head->type == r_cmd) {
      encodeRCmd(head);
    } else if (head->type == i_cmd) {
      encodeICmd(head, labels);
    } else if (head->type == j_cmd) {
      encodeJCmd(head, labels);
    }
    head = head->next;
  }
}

/* Update the addresses of data picture by adding ICF.
 *
 * Params:
 * DataItem **dataPicture: pointer to data picture linked list.
 * unsigned long ICF: value of the last address in command picture.
*/
void updateDataPictureAddress(DataItem **dataPicture, unsigned long ICF) {
  DataItem *head = *dataPicture;
  while (head != NULL) {
    head->address += ICF;
    head = head->next;
  }
}

/* Read line by line from file, and if it is commandLine / orderLine validate the content.
 *
 * Params:
 * FILE *stream: pointer to file, the file is already open.
 * Error **error: pointer to errors table.
 * Label **label: pointer to labels table.
 * int numOfLabels: labels table length.
 * int *numOfErrors: pointer to errors table length.
*/
void validateFile(FILE *stream, Error **errors, Label *labels, int *numOfErrors) {
  int numOfLines = 0;
  int read;
  ErrorType errorType;
  Error *error;
  char *line = (char *) calloc(MAX_LINE_LENGTH + 1, sizeof(char));

  if (line == NULL) {
    printf("allocation Error! \n");
    return;
  }
  fseek(stream, 0, SEEK_SET);

  while ((read = getLine(stream, line, MAX_LINE_LENGTH + 1)) != -1) {
    if (read > MAX_LINE_LENGTH) {
      numOfLines++;
      read = getLine(stream, line, MAX_LINE_LENGTH + 1);
      continue;
    }
    numOfLines++;
    errorType = checkLine(line, labels);
    if (errorType != valid) {
      error = initNewError(getMessageErrorType(errorType), numOfLines);
      addNewError(errors, error);
      (*numOfErrors)++;
    }
  }
  free(line);
}


/* Free all command nodes from commands linked list.
 *
 * Params:
 * Command **commands: pointer to commands linked list.
*/
void freeCommands(Command **commands) {
  Command *currentCommand;

  if (commands == NULL) {
    return;
  }

  while (*commands) {
    currentCommand = *commands;
    *commands = (*commands)->next;
    free(currentCommand->bits);
    free(currentCommand->line);
    free(currentCommand);
  }
}

/* Free all errors nodes from errors linked list.
 *
 * Params:
 * Error **errors: pointer to errors linked list.
*/
void freeErrors(Error **errors) {
  Error *currentError;

  if (errors == NULL) {
    return;
  }

  while (*errors) {
    currentError = *errors;
    *errors = (*errors)->next;
    free(currentError->message);
    free(currentError);
  }
}

/* Free all data items nodes from data picture linked list.
 *
 * Params:
 * DataItem **dataPicture: pointer to data picture linked list.
*/
void freeDataPicture(DataItem **dataPicture) {
  DataItem *currentDataItem;

  if (dataPicture == NULL) {
    return;
  }

  while (*dataPicture) {
    currentDataItem = *dataPicture;
    *dataPicture = (*dataPicture)->next;
    free(currentDataItem);
  }
}

/* Free all labels nodes from labels linked list.
 *
 * Params:
 * Label **labels: pointer to labels linked list.
*/
void freeLabels(Label **labels) {
  Label *currentLabel;

  if (labels == NULL) {
    return;
  }

  while (*labels) {
    currentLabel = *labels;
    *labels = (*labels)->next;
    free(currentLabel->appearances);
    free(currentLabel->symbol);
    free(currentLabel);
  }
}
