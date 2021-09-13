#ifndef MAMAN14_DATATYPES_H
#define MAMAN14_DATATYPES_H

#include "common.h"
#include "constants.h"
#include "stringExtension.h"

typedef enum {
    db, dw, dh, asciz, entry, external
} OrderType;

typedef enum {
    r_arithmetic_cmd, r_move_command, i_arithmetic_cmd, i_branch_cmd, j_jump_cmd, J_cmd, stop_cmd
} CmdSubtype;

typedef enum {
    r_cmd, i_cmd, j_cmd
} CmdType;

typedef enum {
    register_name,
    command_name,
    directive_name,
    number_of_parameters,
    wrong_parameters,
    wrong_syntax,
    value_out_of_range,
    not_a_printed_symbol,
    label_extern_and_internal,
    missing_comma,
    multiple_commas,
    label_syntax,
    valid,
    label_doesnt_exists,
    missing_param,
    extraneous_param,
    memory_Allocation,
    missing_quotations,
    empty_label,
    label_with_invalid_line
} ErrorType;

typedef enum {
    true,
    false
} Boolean;

/*Data structure representing attributes*/
typedef struct attributes {
    Boolean isCode;
    Boolean isData;
    Boolean isEntry;
    Boolean isExternal;
} Attributes;

/*Data structure representing label */
typedef struct label {
    char *symbol;
    unsigned long value;
    Attributes attr;
    unsigned long *appearances;
    int appearancesLength;
    struct label *next;
} Label;

typedef struct lineParts {
    char *labelName;
    char *cmdName;
    char *params;
} LineParts;

/*Data structure representing error*/
typedef struct error {
    int lineNumber;
    char *message;
    struct error *next;
} Error;

/* Data representing part from command line.*/
typedef enum {
    label,
    commandName,
    parameters
} CommandItem;

/* bit field struct for register commands */
typedef struct rCommand {
    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    unsigned int rd: 5;
    unsigned int funct: 5;
    unsigned int unused: 6;
} RCommand;

/* bit field struct for immediate commands */
typedef struct iCommand {
    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    int immed: 16;
} ICommand;

/* bit field struct for jmp commands */
typedef struct jCommand {
    unsigned int opcode: 6;
    unsigned int reg: 1;
    unsigned int address: 25;
} JCommand;

/* Data structure representing assembly command */
typedef struct command {
    unsigned long address;
    void *bits;
    CmdType type;
    char *line;
    struct command *next;
} Command;

/* Data structure representing data item of order line */
typedef struct data {
    int value: 32;
} Data;

/* enum representing the size of data item. */
typedef enum {
    byte = 1,
    half_word = 2,
    word = 4
} DataSize;

/* Data structure representing data item node in the linked list of data picture. */
typedef struct dataItem {
    unsigned long address;
    Data item;
    DataSize size;
    struct dataItem *next;
} DataItem;

Boolean isEmptyLine(char *line);

void addNewLabel(Label **labels, Label *label);

Label *initLabel(char *symbol, unsigned long value, Attributes *attr);

void printErrorStruct(Error **errors);

Command *initNewCommand(char *commandLine, unsigned long address);

void addNewCommand(Command **commands, Command *command);

Error *initNewError(char *message, int numberLine);

void addNewError(Error **errors, Error *error);

void markLabelAsEntry(Label **labels, char *labelName);

void updateDataLabels(Label **labels, int ICF);

void addLabelAppearance(Label **labels, char *labelName, unsigned long addressAppearance);

void updateLabelAppearances(Command **commands, Label **labels, char *labelName);

void updateExternalAppearancesLabels(Command **commands, Label **labels);

DataItem *initNewDataItem(long value, unsigned long address, DataSize size);

void addNewDataItem(DataItem **dataPicture, DataItem *dataItem);

void resetAttributes(Attributes *attributes);

CmdType getCmdTypeByCommandName(char *commandName);

LineParts *getCommandParts(char *command);

Boolean isCommandInArray(int numOfCommands, char **commands, char *commandName);

Boolean findInArray(char *command, char **arr, int length);

void freeLinePart(LineParts *lineParts);

#endif