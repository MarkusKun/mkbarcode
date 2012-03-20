OBJECTS := ean.o testing.o code128.o barstrings.o
BINARY  := mkbarcode.bin

all: $(BINARY)

$(BINARY) : $(OBJECTS)
	 $(CXX) -o $@ $^ 

doc :
	doxygen doxygen.conf

clean:
	rm -rf $(OBJECTS)
	
