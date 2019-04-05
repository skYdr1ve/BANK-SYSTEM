build: directories bin/banking_system
	
run: build
	bin/banking_system

directories:	
	mkdir -p bin build

build/Client.o: src/Client.c
	gcc -std=c99 -c -MD src/Client.c -o build/Client.o

build/Operator.o: src/Operator.c
	gcc -std=c99 -c -MD src/Operator.c -o build/Operator.o

build/Administrator.o: src/Administrator.c
	gcc -std=c99 -c -MD src/Administrator.c -o build/Administrator.o

build/TP_lab5.o: src/TP_lab5.c
	gcc -std=c99 -c -MD src/TP_lab5.c -o build/TP_lab5.o

build/sqlite3.o: src/sqlite3.c
	gcc -std=c99 -c -MD src/sqlite3.c -o build/sqlite3.o	

bin/banking_system: build/Client.o build/Operator.o build/Administrator.o build/TP_lab5.o build/sqlite3.o 
	gcc -std=c99 build/Client.o build/Operator.o build/Administrator.o build/TP_lab5.o build/sqlite3.o  -o bin/banking_system -lsqlite3

include $(wildcard *.d) 

clean: 
	rm -rf build/*.d build/*.o bin/banking_system
