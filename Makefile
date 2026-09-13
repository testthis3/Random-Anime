CC = cc
CFLAGS = -Wall -Wextra -O2

TARGET = ra

all:
	$(CC) $(CFLAGS) ra.c -o $(TARGET) 

clean:
	rm -f $(TARGET)

install:
	mkdir -p $(HOME)/.local/bin
	cp $(TARGET) $(HOME)/.local/bin/

uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)
