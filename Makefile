
# flags per la compilazione
#CFLAGS = -std=c89 -Wpedantic
	
CC = gcc
LIBS=libs/
OBJ = $(LIBS)ipc_utilities.o $(LIBS)utilities.o #$(LIBS)dijkstra.o
OBJMAIN = main.o
OBJSOSOURCES=so_sources.o
OBJTAXI=taxi.o
all : main so_sources  clean run
main: $(OBJMAIN) $(OBJ)
	$(CC) $(OBJMAIN) $(OBJ) -o main
so_sources: $(OBJSOSOURCES) $(OBJ)
	$(CC) $(OBJSOSOURCES) $(OBJ) -o so_sources
#taxi: $(OBJTAXI) $(OBJ)
#	$(CC) $(OBJTAXI) $(OBJ) -o taxi
	

clean:
	rm -f *.o 
	rm -f $(LIBS)*.o
	clear
	

# il target run si usa talvolta per eseguire l'applicazione
run: 
	./main
