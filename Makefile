CC=g++
LDFLAGS = -lm
CXXFLAGS = -c -Wall -ansi -g
OBJECTS = objdir/wiringSerial.o objdir/VC0706.o

all: clean bin/cameraInfo bin/fetch bin/motionDetector bin/setImageSize bin/resetCamera

bin:
	mkdir bin

objdir:
	mkdir objdir

bin/cameraInfo: bin $(OBJECTS) objdir/cameraInfo.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/cameraInfo.o -o bin/cameraInfo

objdir/cameraInfo.o: objdir $(OBJECTS) src/cameraInfo.cpp
	$(CC) $(CXXFLAGS) src/cameraInfo.cpp -o objdir/cameraInfo.o

bin/fetch: bin $(OBJECTS) objdir/fetch.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/fetch.o -o bin/fetch

objdir/fetch.o: objdir $(OBJECTS) src/fetch.cpp
	$(CC) $(CXXFLAGS) src/fetch.cpp -o objdir/fetch.o

bin/motionDetector: bin $(OBJECTS) objdir/motionDetector.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/motionDetector.o -o bin/motionDetector

objdir/motionDetector.o: objdir $(OBJECTS) src/motionDetector.cpp
	$(CC) $(CXXFLAGS) src/motionDetector.cpp -o objdir/motionDetector.o

bin/setImageSize: bin $(OBJECTS) objdir/setImageSize.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/setImageSize.o -o bin/setImageSize

objdir/setImageSize.o: objdir $(OBJECTS) src/setImageSize.cpp
	$(CC) $(CXXFLAGS) src/setImageSize.cpp -o objdir/setImageSize.o

bin/resetCamera: bin $(OBJECTS) objdir/resetCamera.o
	$(CC) $(LDFLAGS) $(OBJECTS) objdir/resetCamera.o -o bin/resetCamera

objdir/resetCamera.o: objdir $(OBJECTS) src/resetCamera.cpp
	$(CC) $(CXXFLAGS) src/resetCamera.cpp -o objdir/resetCamera.o

objdir/VC0706.o: objdir objdir/wiringSerial.o include/VC0706.cpp
	$(CC) $(CXXFLAGS) include/VC0706.cpp -o objdir/VC0706.o

objdir/wiringSerial.o: objdir include/wiringSerial.c
	$(CC) $(CXXFLAGS) include/wiringSerial.c -o objdir/wiringSerial.o

clean:
	rm -rf objdir bin
