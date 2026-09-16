CC = cc
CFLAGS = -Wall -Wextra -O2 -lcurl -lcjson

TARGET = ra

all:
	$(CC) $(CFLAGS) main.c file.c api.c display.c -o $(TARGET) 

clean:
	rm -f $(TARGET)

install:
	$(CC) $(CFLAGS) main.c file.c api.c display.c -o $(TARGET) 
	mkdir -p $(HOME)/.local/bin
	cp $(TARGET) $(HOME)/.local/bin/

uninstall:
	rm -f $(HOME)/.local/bin/$(TARGET)
