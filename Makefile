# Define the compiler
CXX := g++
CXXFLAGS := `root-config --libs --cflags` -I/opt/gsl/include -L/opt/gsl/lib -lgsl -lgslcblas -lm
CXXFLAGS += -DFULL_FILE_PATH=\"$(CURDIR)/\"

SRC = $(wildcard src/[!r]*.cpp)
INC = -Iinc

# Pattern rule to compile any .cpp file in src to an .exe file in bin
bin/test.exe: test/test.cpp
	$(CXX) $< $(SRC) $(CXXFLAGS) $(INC) -o $@

# Clean rule to remove all executables in the bin directory
clean:
	rm -f $(BIN_DIR)/*.exe