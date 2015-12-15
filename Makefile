EXECUTABLE = powerstep
SOURCES    = $(wildcard src/*.c)
OBJECTS    = $(patsubst %,%,${SOURCES:.c=.o})
CXX        = gcc
CXXFLAGS   = -Wall -g -c -o
LDFLAGS    =
LDLIBS     = lreadline
BEAGLE_USER = root
BEAGLE_IP  = 192.168.0.7
BEAGLE_DIR = ~/powerstep
#-------------------------------------------------------------------------------

#make executable
all: $(EXECUTABLE) 

%.o: %.c
	$(CXX) $(CXXFLAGS) $@ $< -MMD -MF ./$@.d

#link Objects
$(EXECUTABLE) : $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS) -$(LDLIBS)

#make clean
clean:
	rm -f src/*.o
	rm -f src/*.o.d
	rm -f $(EXECUTABLE)

#make valgrind
valgrind:
	valgrind --tool=memcheck --leak-check=full ./$(EXECUTABLE)
	
#make beagle
beagle:
	scp -r . $(BEAGLE_USER)@$(BEAGLE_IP):$(BEAGLE_DIR)
