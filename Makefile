SOURCES = kovan-recovery.c flash_drive.c image.c
OBJECTS = $(SOURCES:.c=.o)
EXEC = kovan-recovery
MY_CFLAGS += -Wall -Werror -Os -DDANGEROUS

all: $(OBJECTS)
	$(CC) $(LIBS) $(LDFLAGS) $(OBJECTS) $(MY_LIBS) -o $(EXEC)

clean:
	rm -f $(EXEC) $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $(MY_CFLAGS) $< -o $@

