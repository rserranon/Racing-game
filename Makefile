# NCURSES-GAME-ENGINE MAKEFILE
# make all        Builds the package + documentation
# make run        Builds and runs the program
# make clean      Cleans results of building process (objects end executable)
# make doc        Generates the documentation with doxygen
# make docclean   Removes the documentation
# make clean-all  Cleans documentation and results of build process



# General Info
PACKAGE = Racing-game

# Build info
EXE         = $(PACKAGE)
CDEBUG      = -O0
CXXFLAGS    = $(CDEBUG) -c -g -Wno-reorder -std=c++11 -Wall -pedantic -Wno-long-long -O0 -ggdb
LDFLAGS     = -lmenu -lncurses
INCLUDESDIR = -I"src/"
LIBSDIR     =

# Project source files
CXXFILES = $(shell find . -type f -name '*.cpp')
OBJECTS  = $(CXXFILES:.cpp=.o)

# Verbose mode check:
#V = 1;
ifdef V
MUTE =
VTAG = -v
else
MUTE = @
endif

compile: $(EXE)
	# Build successful!

# Make targets
all:compile doc
	# Build executable and socumentation successful!


$(EXE): $(OBJECTS)
	# Linking...
	$(MUTE)$(CXX) $(OBJECTS) -o $(EXE) $(LIBSDIR) $(LDFLAGS)

%.o: %.cpp
	# Compiling $<...
	$(MUTE)$(CXX) $(CXXFLAGS) $(CDEBUG) $< -c -MD -o $@  $(INCLUDESDIR)

-include *.d

run: compile
	# Running...
	$(MUTE)./$(EXE)

doc:
	# Generating documentation...
	mkdir doc
	$(MUTE)doxygen Doxyfile

docclean:
	# Cleaning documentation...
	-$(MUTE)rm $(VTAG) -rf doc

clean:
	# Cleaning object files...
	$(MUTE)rm $(VTAG) -f $(OBJECTS)
	$(MUTE)rm $(VTAG) -f $(EXE)
	$(MUTE)rm $(VTAG) -f *.d

clean-all: clean docclean

