CC=gcc
CFLAGS= -std=c99 -Wall -Wextra 

SCANNER=src/scanner/scanner
PARSER=src/parser/parser
STRING=src/str/ifj_string
BST=src/bst/bst
DATA=src/data/data
SYMTABLE=src/symtable/symtable
SEMANTIC=src/semantic/semantic
EXPRESSION=src/expressions/expr
GENERATOR=src/generator/generator

OBJFILES = $(SCANNER).o $(PARSER).o $(STRING).o $(BST).o $(DATA).o $(SYMTABLE).o $(SEMANTIC).o $(EXPRESSION).o $(GENERATOR).o main.o
NAME=IFJ22


main: $(NAME)
	
$(NAME) : $(OBJFILES)	
	$(CC) $(CFLAGS) -o $(NAME) $(OBJFILES) 
	rm -f $(OBJFILES) *~

clean:
	rm -f $(NAME) *~


.SILENT: test

test:
	./IFJ22 < test_files/test.php > test.out
	./ic22int test.out
	rm test.out