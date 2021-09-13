#include "files.h"


/*
 * Creates a string that represents the bit field.
 *
 * Params:
 * unsigned int item: the bit field.
 * int length: the size of the new string.
 * char *buf: a pointer to the new string.
 *
 * Return:
 * buf - a pointer to the new string.
 */
char *createStrFromBitField(unsigned int item, const int length, char *buf) {
  int len = 0;
  char *str = (char *) calloc(length + 1, sizeof(char));
  if (str == NULL) {
    printf("Error: Allocation Error! \n");
    return NULL;
  }

  /* If command is 0, just continue pad with zeros until str will be in the length of length. */
  while (len < length) {
    if (item & 1) {
      str[length - len - 1] = '1';
    } else {
      str[length - len - 1] = '0';
    }

    item >>= 1;
    len++;
  }

  str[length] = '\0';
  strcpy(buf, str);
  free(str);
  return buf;
}

/*
 * Prints the command table to an ob file, converting  to the hexadecimal representation.
 *
 * Params:
 * Command **commands: a linked list containing all the commands.
 * FILE *fptr: a pointer to the file to be printed.
 *
 * Return:
 * int count: the last memory place that printed.
 */
FILE *writeCommandIntoObjectFile(Command **commands, FILE *fptr) {
  long count = 100;
  long p1, p2, p3, p4;
  Command *head = *commands;
  char *buf, *temp;

  while (head != NULL) {
    /*Allocation of memory for storing string parts */
    buf = (char *) calloc(33, sizeof(char));
    temp = (char *) calloc(33, sizeof(char));
    if (buf == NULL || temp == NULL) {
      printf("Error: Allocation Error! \n");
      return fptr;
    }

    /*Create a string representing the command in 32 characters, based on the byte field */
    if (head->type == r_cmd) {
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->opcode, 6, temp));
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->rs, 5, temp));
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->rt, 5, temp));
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->rd, 5, temp));
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->funct, 5, temp));
      buf = strcat(buf, createStrFromBitField(((RCommand *) (head->bits))->unused, 6, temp));
    } else if (head->type == i_cmd) {
      buf = strcat(buf, (createStrFromBitField(((ICommand *) (head->bits))->opcode, 6, temp)));
      buf = strcat(buf, (createStrFromBitField(((ICommand *) (head->bits))->rs, 5, temp)));
      buf = strcat(buf, (createStrFromBitField(((ICommand *) (head->bits))->rt, 5, temp)));
      buf = strcat(buf, (createStrFromBitField(((ICommand *) (head->bits))->immed, 16, temp)));
    } else if (head->type == j_cmd) {
      buf = strcat(buf, (createStrFromBitField(((JCommand *) (head->bits))->opcode, 6, temp)));
      buf = strcat(buf, (createStrFromBitField(((JCommand *) (head->bits))->reg, 1, temp)));
      buf = strcat(buf, (createStrFromBitField(((JCommand *) (head->bits))->address, 25, temp)));
    }
    /* Dividing the string into four parts, converting to the hexadecimal base and printing */
    p1 = cut(buf, 0, 8);
    p2 = cut(buf, 8, 8);
    p3 = cut(buf, 16, 8);
    p4 = cut(buf, 24, 8);
    fprintf(fptr, "%04ld %02lX %02lX %02lX %02lX \n", head->address, p4, p3, p2, p1);
    count += 4;

    head = head->next;
    free(buf);
    free(temp);
  }
  return fptr;
}

/*
 * Prints the dataPicture to an ob file, converting  to the hexadecimal representation.
 *
 * Params:
 * DataItem **dataPicture : a linked list containing all the directives.
 * FILE *fptr: a pointer to the file to be printed.
 */
void writeOrderIntoObjectFile(DataItem **dataPicture, FILE *fptr) {
  int placeInLIne = 1;
  long p1, p2, p3, p4;
  unsigned long count;
  DataItem *head = *dataPicture;
  char *buf,*temp;
  if(head != NULL){
    count = head->address;
  }
  while (head != NULL) {
    temp = (char *) calloc(33, sizeof(char));
    if (temp == NULL) {
      printf("Error: Allocation Error! \n");
      return;
    }

    /*The function is divided according to the number of sections to be printed from each specific dataPicture
     * This section is for cases where only one section needs to be printed*/
    if (head->size == byte) {
      buf = createStrFromBitField((head->item).value, 8, temp);
      p1 = cut(buf, 0, 8);
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p1);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p1);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p1);
        placeInLIne = 1;
      }
    }
      /*This section is for a case where two sections have to be printed based on the information image */
    else if (head->size == half_word) {
      buf = createStrFromBitField((head->item).value, 16, temp);
      p1 = cut(buf, 0, 8);
      p2 = cut(buf, 8, 8);
      /*Print the p2 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p2);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p2);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p2);
        placeInLIne = 1;
      }
      /*Print the p1 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p1);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p1);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p1);
        placeInLIne = 1;
      }

    }
      /*This section is for a case where four sections have to be printed based on the information image*/
    else {
      buf = createStrFromBitField((head->item).value, 32, temp);
      p1 = cut(buf, 0, 8);
      p2 = cut(buf, 8, 8);
      p3 = cut(buf, 16, 8);
      p4 = cut(buf, 24, 8);
      /*Print the p4 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p4);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p4);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p4);
        placeInLIne = 1;
      }
      /*Print the p3 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p3);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p3);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p3);
        placeInLIne = 1;
      }
      /*Print the p2 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p2);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p2);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p2);
        placeInLIne = 1;
      }
      /*Print the p1 section in its proper place */
      if (placeInLIne == 1) {
        fprintf(fptr, "%04ld %02lX ", count, p1);
        count += 4;
        placeInLIne++;
      } else if (placeInLIne == 2 || placeInLIne == 3) {
        fprintf(fptr, "%02lX ", p1);
        placeInLIne++;
      } else if (placeInLIne == 4) {
        fprintf(fptr, "%02lX \n", p1);
        placeInLIne = 1;
      }
    }
    free(buf);
    head = head->next;
  }
}

/*
 * Divides a string into one sub-strings from begin to end and preserves their hexadecimal value.
 *
 * Params:
 * const char *buf: a pointer to the full string.
 * int begin: the index to start the copy.
 * int end: how many chars to copy.
 *
 * Return:
 * long p1: number that preserves the value of the sub-string.
 */
long cut(const char *buf, int begin, int end) {
  long p;
  char *part = (char *)calloc(9, sizeof(char));
  if (part == NULL) {
    printf("Error: Allocation Error! \n");
    return -1;
  }
  strncpy(part, buf + begin, end);
  p = strtol(part, NULL, 2);
  free(part);
  return p;
}

/*
 * Creates the ob file.
 *
 * Params:
 * char *filename: the input file name.
 * Command **commands: the commands linked list.
 * DataItem **dataPicture: the dataPicture linked list.
 * unsigned long ICF: the memory length of the Orders image.
 * unsigned long IDF: the memory length of the dataPicture image.
 */
void
createObjectFile(char *filename, Command **commands, DataItem **dataPicture, unsigned long ICF, unsigned long IDF) {
  char *name = changeFileName(filename, ".ob");
  FILE *fp = fopen(name, "w");

  fprintf(fp, "\t \t %ld %ld \n", ICF - 100, IDF - ICF);
  fp = writeCommandIntoObjectFile(commands, fp);
  writeOrderIntoObjectFile(dataPicture, fp);
  free(name);
  fclose(fp);
}

/*
 * Creates the entry file.
 *
 * Params:
 * Label *labels: a pointer to a linked list of all labels.
 * char *filename: the name of the input file.
 */
void createEntryFile(Label *labels, char *filename) {
  int count = 0;
  Label *head = labels;
  FILE *fp;
  char *name;

  name = changeFileName(filename, ".ent");
  fp = fopen(name, "w");
  while (head) {
    if (head->attr.isEntry == true) {
      fprintf(fp, "%s %04ld \n", head->symbol, head->value);
      count +=1;
    }
    head = head->next;
  }
  if(count == 0){
    remove(name);
  }
  free(name);
  fclose(fp);
}

/*
 * creates the extern file.
 *
 * Params:
 * Label *labels: a pointer to a linked list of all labels.
 * char *filename: the name of the input file.
 */
void createExternalFile(Label *labels, char *filename) {
  Label *head = labels;
  FILE *fp;
  int i, count = 0;
  char *name;

  name = changeFileName(filename, ".ext");
  fp = fopen(name, "w");
  while (head) {
    if (head->attr.isExternal == true) {
      for (i = 0; i < head->appearancesLength; i++) {
        fprintf(fp, "%s %04lu \n", head->symbol, head->appearances[i]);
      }
      count +=1;
    }
    head = head->next;
  }
  if(count == 0){
    remove(name);
  }
  free(name);
  fclose(fp);
}

/*
 * Replaces the end of the function name in the 'ext' string.
 *
 * Params:
 * char *name: the name of the file.
 * char *ext: the file extension.
 *
 * Return:
 * char *filname - the new name of the file.
 */
char *changeFileName(char *name, char *ext) {
  size_t fileNameLen = strlen(name) - 3 + strlen(ext);
  char *fileName = (char *) calloc(fileNameLen + 1, sizeof(char));
  if (name == NULL) {
    printf("Error: Allocation Error! \n");
    return NULL;
  }
  fileName = strncpy(fileName, name, strlen(name) - 3);
  fileName[strlen(name) - 3] = '\0';
  strcat(fileName, ext);
  return fileName;
}

/* Make sure the extension of filename is ".as".
 *
 * Params:
 * char *filename: file name.
 *
 * Returns:
 * Boolean isAsFile: true if file from type of as, otherwise - return false.
*/
Boolean isAsFile(char *filename){
  size_t length = strlen(filename);
  const char *extension = &filename[length - 3];

  if(strcmp(extension, ".as") == 0){
    return true;
  }
  return false;
}