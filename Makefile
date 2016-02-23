CXX = g++
CPPFLAGS =        # put pre-processor settings (-I, -D, etc) here
CXXFLAGS =   # put compiler settings here
LDFLAGS =         # put linker settings here
PROGRAM_NAME = shell


shell: shell.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) shell.o

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<


run: 
	./$(PROGRAM_NAME)

#test.cpp: test.h