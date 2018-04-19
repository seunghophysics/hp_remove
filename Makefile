CC = g++
CXXFLAGS = `root-config --cflags`
LDLIBS = `root-config --libs`

OBJ = main_hp.o lib_hp.o

%.o: %.c
	$(CC) -c $< $(CFLAGS) 

hp_remove: $(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -f $(OBJ) hp_remove
