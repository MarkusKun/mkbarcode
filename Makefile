OBJECTS := ean.o testing.o code128.o barstrings.o
BINARY  := mkbarcode.bin

all: $(BINARY)

$(BINARY) : $(OBJECTS)
	 $(CXX) -o $@ $^ 

clean:
	rm -rf $(OBJECTS)
