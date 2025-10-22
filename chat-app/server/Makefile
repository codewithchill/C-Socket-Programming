# _*_ Makefile _*_

# *Compiler
CC=gcc
CFLAGS = -Wall -O2

# *Directories
PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
OBJDIR=$(PROJECT_ROOT)/build/obj
BINDIR=$(PROJECT_ROOT)/build/bin
SRCDIR=$(PROJECT_ROOT)/src

.PHONY: dirs all clean run

all: $(BINDIR)/server

dirs:
	mkdir -p $(OBJDIR) $(BINDIR)

$(BINDIR)/server: $(OBJDIR)/server.o $(OBJDIR)/server-setup.o $(OBJDIR)/accept-conn.o
	$(CC) $(CFLAGS) -o $@ $^

# server.o: server.c
# 	$(CC) $(CFLAGS) -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c include/server.h | dirs
	$(CC) $(CFLAGS) -o $@ -c $< 

run: all
	$(BINDIR)/server

clean:
	rm -rf $(BINDIR) $(OBJDIR)