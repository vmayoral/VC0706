CC=g++
LDFLAGS = -lm
CXXFLAGS = -c -Wall -ansi -g
OBJECTS = objdir/camDB.o

all: clean bin/camTimer bin/addTimer bin/exitTimer

bin:
	mkdir bin

objdir:
	mkdir objdir

bin/camTimer: bin $(OBJECTS) objdir/camTimer.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/camTimer.o -o bin/camTimer

objdir/camTimer.o: objdir $(OBJECTS) camTimer.c
	$(CC) $(CXXFLAGS) camTimer.c -o objdir/camTimer.o

bin/exitTimer: bin $(OBJECTS) objdir/exitTimer.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/exitTimer.o -o bin/exitTimer

objdir/exitTimer.o: objdir $(OBJECTS) exitTimer.c
	$(CC) $(CXXFLAGS) exitTimer.c -o objdir/exitTimer.o

bin/addTimer: bin $(OBJECTS) objdir/addTimer.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/addTimer.o -o bin/addTimer

objdir/addTimer.o: objdir $(OBJECTS) addTimer.c
	$(CC) $(CXXFLAGS) addTimer.c -o objdir/addTimer.o

objdir/camDB.o: objdir include/camDB.c
	$(CC) $(CXXFLAGS) include/camDB.c -o objdir/camDB.o

clean:
	rm -rf objdir bin
