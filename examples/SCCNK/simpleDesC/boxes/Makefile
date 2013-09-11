TARGET  = libboxes.a

BOXOBJS = desBoxes.o

.PHONY: clean

$(TARGET): $(BOXOBJS)
	$(AR) rcs $@ $^

%.o: %.c *.h
	$(CC) $(CCFLAGS) -I$(SNET_INCLUDES) -c $<

clean:
	rm -f $(TARGET) $(BOXOBJS)
