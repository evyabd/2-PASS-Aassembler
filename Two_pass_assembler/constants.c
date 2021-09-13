char *registerName[32] = {
        "$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7",
        "$8", "$9", "$10", "$11", "$12", "$13", "$14",
        "$15", "$16", "$17", "$18", "$19", "$20", "$21",
        "$22", "$23", "$24", "$25", "$26", "$27", "$28",
        "$29", "$30", "$31"};

char *commandsTable[27] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
                             "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt",
                             "bgt", "lb", "sb", "lw", "sw", "lh", "sh", "jmp", "la", "call", "stop"};

int opcodes[27] = {0, 0, 0, 0, 0, 1, 1, 1, 10,
                   11, 12, 13, 14, 15, 16, 17, 18, 19,
                   20, 21, 22, 23, 24, 30, 31, 32, 63};

int functs[8] = {1, 2, 3, 4, 5, 1, 2, 3};

char *directive[6] = {".dh", ".dw", ".db", ".asciz", ".entry", ".extern"};
char *rACmd[5] = {"add", "sub", "and", "or", "nor"};
char *rMCmd[3] = {"move", "mvhi", "mvlo"};
char *iACmd[11] = {"addi", "subi", "andi", "ori", "nori", "lb", "sb", "sw", "lw", "lh", "sh"};
char *iBCmd[4] = {"bne", "beq", "blt", "bgt"};
char *jJCmd[1] = {"jmp"};
char *jCmd[2] = {"la", "call"};
char *sCmd[1] = {"stop"};