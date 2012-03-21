all : all_bins

include Makefile.Paths

# OBJ_DEBUG
include $(PATH_TO_DEBUGOUT)Makefile.inc

# OBJ_PIXELBILD and OBJ_BMPREADER
include $(PATH_TO_BILDSPIELE)Makefile.inc

OBJECTS := ean.o testing.o code128.o barstrings.o $(OBJ_DEBUGOUT) $(OBJ_PIXELBILD) $(OBJ_BMPREADER)
BINARY  := mkbarcode.bin

CXXFLAGS += -I$(PATH_TO_DEBUGOUT) -I$(PATH_TO_BILDSPIELE)

all_bins: $(BINARY)

$(BINARY) : $(OBJECTS)
	 $(CXX) -o $@ $^ 

doc :
	doxygen doxygen.conf

clean:
	rm -rf $(OBJECTS)
	
