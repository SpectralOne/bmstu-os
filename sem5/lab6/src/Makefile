# Компилятор
CC := gcc
MAIN_APP := app.exe
.PHONY : clean all

# Опции компиляции
CFLAGS := -std=c99 -Wall -Werror -pedantic -pipe

# Файловые зависимости
HEADERS := include
RELEASE_FILES := $(wildcard *.c)

all : $(MAIN_APP)

# Main app
$(MAIN_APP) : $(patsubst %.c,%.o, $(RELEASE_FILES))
	$(CC) $(CFLAGS) -I $(HEADERS) $^ -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -I $(HEADERS) -c -MD $< 

include $(wildcard *.d)

clean :
	rm -f *.d *.o *.exe
