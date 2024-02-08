CC = cc
SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
EXEC = pass-gen
MANPAGE = $(EXEC).1
LIBS = -lssl -lcrypto
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic
RELEASE = -O3 -DNDEBUG
DEBUG = -O0 -g
INSTALL_DIR = /usr/local/bin
MAN_DIR = /usr/local/share/man/man1

all: debug

release: CFLAGS += $(RELEASE)
release: $(EXEC)

debug: CFLAGS += $(DEBUG)
debug: $(EXEC)

$(EXEC) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@echo "----BUILD_DONE----"

clean:
	rm -f $(OBJS) $(EXEC)
	@echo "----BUILD_CLEANED----"

install: clean release
	install -m 755 $(EXEC) $(INSTALL_DIR)
	install -m 644 $(MANPAGE) $(MAN_DIR)
	@echo "----INSTALL_DONE----"

uninstall:
	rm -f $(INSTALL_DIR)/$(EXEC)
	rm -f $(MAN_DIR)/$(MANPAGE)
	@echo "----UNINSTALL_DONE----"

.PHONY: all release debug clean install uninstall
