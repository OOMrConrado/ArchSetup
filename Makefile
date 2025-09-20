CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = sysmon
SOURCES = main.c sysmon.c cpu_info.c memory_info.c system_info.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c sysmon.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "ArchSetup System Monitor instalado en /usr/local/bin/$(TARGET)"
	@echo "Ahora puedes ejecutar 'ArchSetup' desde cualquier directorio"

uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "ArchSetup System Monitor desinstalado"

# Crear un enlace simbólico para que funcione con el comando "ArchSetup"
setup-command: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/ArchSetup
	sudo chmod +x /usr/local/bin/ArchSetup
	@echo "Comando 'ArchSetup' configurado exitosamente"
	@echo "Ahora puedes ejecutar 'ArchSetup' desde cualquier directorio"

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

test: $(TARGET)
	./$(TARGET) --help
	./$(TARGET) --cpu
	./$(TARGET) --memory

run: $(TARGET)
	./$(TARGET)

watch: $(TARGET)
	./$(TARGET) --watch