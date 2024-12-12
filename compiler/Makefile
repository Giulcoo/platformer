FILENAME = platform
SOURCE_FILES := output/lex.yy.c helper/ast.c helper/compiler.c helper/editor.c helper/error.c
INPUT ?= example
VERBOSE ?= 1

all: $(FILENAME)

dir:
	@mkdir -p output

$(FILENAME).tab.c $(FILENAME).tab.h:	$(FILENAME).y
	bison -t -v -d $(FILENAME).y -o output/$(FILENAME).tab.c

lex.yy.c: $(FILENAME).l $(FILENAME).tab.h
	flex -o output/lex.yy.c $(FILENAME).l 

$(FILENAME): dir lex.yy.c $(FILENAME).tab.c $(FILENAME).tab.h
	gcc -o output/$(FILENAME) output/$(FILENAME).tab.c ${SOURCE_FILES}

clean:
	rm output/*

run:
	@output/$(FILENAME) $(INPUT).txt $(VERBOSE)