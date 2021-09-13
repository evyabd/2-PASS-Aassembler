assembler: assembler.o constants.o encoding.o parserInput.o validation.o stringExtension.o Datatypes.o files.o
	gcc -ansi -Wall -pedantic assembler.o constants.o encoding.o parserInput.o validation.o stringExtension.o Datatypes.o files.o -o assembler

assembler.o: assembler.c validation.h files.h parserInput.h encoding.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

encoding.o: encoding.c encoding.h parserInput.h
	gcc -c -ansi -Wall -pedantic encoding.c -o encoding.o

files.o: files.c files.h Datatypes.h parserInput.h
	gcc -c -ansi -Wall -pedantic files.c -o files.o

validation.o: validation.c validation.h Datatypes.h parserInput.h constants.h
	gcc -c -ansi -Wall -pedantic validation.c -o validation.o

parserInput.o: parserInput.c parserInput.h stringExtension.h Datatypes.h constants.h
	gcc -c -ansi -Wall -pedantic parserInput.c -o parserInput.o

Datatypes.o: Datatypes.c Datatypes.h stringExtension.h constants.h
	gcc -c -ansi -Wall -pedantic Datatypes.c -o Datatypes.o

constants.o: constants.c constants.h
	gcc -c -ansi -Wall -pedantic constants.c -o constants.o

stringExtension.o: stringExtension.c stringExtension.h common.h
	gcc -c -ansi -Wall -pedantic stringExtension.c -o stringExtension.o
