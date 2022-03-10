CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -lpthread

APP = main.out

.PHONY: clean run kill

$(APP): main.o
	$(CC) -o $@ $^

run: $(APP)
	sudo ./$(APP)

kill:
	ps -C $(APP) -o pid= | xargs -- sudo kill

log:
	tail -f /var/log/syslog

%.o: %.c %.h
	$(CC) -o $@ $(CFLAGS) -c $<

%.o: %.c
	$(CC) -o $@ $(CFALGS) -c $^

clean:
	rm -f *.o *.out 2>/dev/null
