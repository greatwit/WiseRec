CXXFLAGS += -std=c++11
 
CC 	= gcc
CPP = g++

COMPLIE_INCL = -I./
COMPLIE_LIB  = -L./ -lpthread
CXXFLAGS += $(COMPLIE_INCL)
LDFLAGS += $(COMPLIE_LIB)

OBJECTS = gthreadpool.o TestThreadpool.o
EXE=ThreadpoolTest
 
 
$(EXE):$(OBJECTS)
	$(CPP) -o $(EXE) $(OBJECTS) $(LDFLAGS) 

.PHONY:clean
clean:
	rm $(EXE) $(OBJECTS)
