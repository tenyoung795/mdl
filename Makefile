OBJECTS= symtab.o print_pcode.o matrix.o my_main.o display.o draw.o gmath.o stack.o node.o vector.o shapes.o shading.o objparser.o
CFLAGS= 
LDFLAGS= -lm
CC= gcc

all: parser

parser: lex.yy.c y.tab.c y.tab.h $(OBJECTS)
	gcc -o mdl $(CFLAGS) lex.yy.c y.tab.c $(OBJECTS) $(LDFLAGS)
	rm y.tab.c y.tab.h lex.yy.c *.o *~

lex.yy.c: mdl.l y.tab.h symtab.h
	flex -I mdl.l

y.tab.c: mdl.y parser.h shapes.h display.h shading.h matrix.h misc_headers.h vector.h
	bison -d -y mdl.y

y.tab.h: mdl.y 
	bison -d -y mdl.y

symtab.o: symtab.c ml6.h vector.h parser.h symtab.h matrix.h misc_headers.h
	gcc -c $(CFLAGS) symtab.c

print_pcode.o: print_pcode.c parser.h shading.h symtab.h y.tab.h vector.h
	gcc -c $(CFLAGS) print_pcode.c

matrix.o: matrix.c matrix.h vector.h misc_headers.h
	gcc -c $(CFLAGS) matrix.c

my_main.o: my_main.c misc_headers.h print_pcode.c matrix.h display.h ml6.h draw.h stack.h node.h parser.h symtab.h y.tab.h ml6.h vector.h shading.h objparser.h
	gcc -c $(CFLAGS) my_main.c

display.o: display.c display.h ml6.h
	$(CC) $(CFLAGS) -c display.c

draw.o: draw.c draw.h display.h ml6.h matrix.h gmath.h misc_headers.h shading.h
	$(CC) $(CFLAGS) -c draw.c

gmath.o: gmath.c gmath.h matrix.h vector.h
	$(CC) $(CFLAGS) -c gmath.c 

stack.o: stack.c stack.h matrix.h misc_headers.h
	$(CC) $(CFLAGS) -c stack.c 

node.o: node.c node.h symtab.h misc_headers.h
	$(CC) $(CFLAGS) -c node.c

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector.c

shapes.o: shapes.c shapes.h vector.h
	$(CC) $(CFLAGS) -c shapes.c

shading.o: shading.c shading.h shapes.h ml6.h vector.h draw.h
	$(CC) $(CFLAGS) -c shading.c

objparser.o: objparser.c objparser.h vector.h matrix.h misc_headers.h
	$(CC) $(CFLAGS) -c objparser.c

clean:
	rm y.tab.c y.tab.h lex.yy.c *.o *~

