CC = gcc
DEBUG = 1
TG = 0
TARGET =
TESTCASEO =
TESTCASEH =
CFS =
LINK =

# release: make DEBUG=0
ifeq ($(DEBUG),1)
CFS += -g -DDEBUG -fprofile-arcs -ftest-coverage -O0
else
CFS += -O3
endif

#tunnel: make TG=1
ifeq ($(TG),0)
TARGET = tunneld
else ifeq ($(TG), 1)
TARGET = tunnel
else ifeq ($(TG), 2)
TARGET = CUTest
TESTCASEO = CUTestCases.o
TESTCASEH = CUTestCases.h
LINK += /usr/local/lib/libcunit.a -lgcov
endif

$(TARGET): $(TARGET).o util.o socklib.o protocol.o $(TESTCASEO)
	$(CC) $(CFS) -o $(TARGET) $(TARGET).o util.o socklib.o protocol.o $(TESTCASEO) $(LINK)

$(TARGET).o: $(TARGET).c socklib.h util.h protocol.h $(TESTCASEH)
	$(CC) $(CFS) -c $(TARGET).c

CUTestCases.o: CUTestCases.c CUTestCases.h socklib.h util.h protocol.h
	$(CC) $(CFS) -c CUTestCases.c

util.o: util.c util.h
	$(CC) $(CFS) -c util.c

socklib.o: socklib.c socklib.h
	$(CC) $(CFS) -c socklib.c

protocol.o: protocol.c protocol.h
	$(CC) $(CFS) -c protocol.c

clean:
	rm $(TARGET).o util.o socklib.o protocol.o CUTestCases.o
