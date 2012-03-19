OBJECTS := ean.o testing.o
BINARY  := mkbarcode.bin

all: $(BINARY)

$(BINARY) : $(OBJECTS)
	 $(CXX) -o $@ $^ 

clean:
	rm -rf $(OBJECTS)
