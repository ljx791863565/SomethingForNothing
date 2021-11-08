release_hdr :=$(shell sh -c './mkreleasehdr.sh')
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OPTIMIZATION ?= -O2

# 暂时没用上
#DEPENDENCY_hiredis linenoise lua

# Default settings
# -pedantic以ANSI/ISO C标准列出的所有警告
# 当GCC在编译不符合ANSI/ISO C语言标准的源代码时，如果在编译指令中加上了-pedantic选项，那么源程序中使用了扩展语法的地方将产生相应的警告信息。
STD=-std=c99 -pedantic
WARN=-Wall
OPT=$(OPTIMIZATION)

PREFIX?=/usr/local
INSTALL_BIN=$(PREFIX)/bin 
INSTALL=install 

#Default allocator
# 可选项有jemalloc 和 tcmalloc 
ifeq ($(uname_S), LINUX)
	MALLOC=libc
endif 

# Override default settings if possible
-include .make-settings

FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS) $(REDIS_CFLAGS)
FINAL_LDFLAGS=$(LDFLAGS) $(REDIS_LDFLAGS) $(DEBUG)
FINAL_LIBS=-lm
DEBUG=-g -ggdb

REDIS_CC=$(QUIET_CC)$(CC) $(FINAL_CFLAGS)
REDIS_LD=$(QUIET_LINK)$(CC) $(FINAL_LDFLAGS)
REDIS_INSTALL=$(QUIET_INSTALL)$(INSTALL)

# color
CCCLOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '	%b %b\n' $(CCCLOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

REDIS_SERVER_NAME=
REDIS_SENTINEL_NAME=
REDIS_CLI_NAME=
REDIS_CLI_OBJ=
REDIS_SERVER_OBJ=zmalloc.o sds.o

all: $(REDIS_SERVER_OBJ)
	@echo ""
	@echo "Hint: To run 'make test' is a good idea ;)"
	@echo ""
.PHONY: all 

# Deps (use make dep to generate this)
include Makefile.dep

dep:
	$(REDIS_CC) -MM *.c > Makefile.dep 
.PHONY: dep

persist-settings: distclean
	echo STD=$(STD) >> .make-settings
	echo WARN=$(WARN) >> .make-settings
	echo OPT=$(OPT) >> .make-settings
	echo MALLOC=$(MALLOC) >> .make-settings
	echo CFLAGS=$(CFLAGS) >> .make-settings
	echo LDFLAGS=$(LDFLAGS) >> .make-settings
	echo REDIS_CFLAGS=$(REDIS_CFLAGS) >> .make-settings
	echo REDIS_LDFLAGS=$(REDIS_LDFLAGS) >> .make-settings
	echo PREV_FINAL_CFLAGS=$(FINAL_CFLAGS) >> .make-settings
	echo PREV_FINAL_LDFLAGS=$(FINAL_LDFLAGS) >> .make-settings
	-(cd ../deps && $(MAKE) $(DEPENDENCY_TARGETS))

.PHONY: persist-settings

# Prerequisites target
.make-prerequisites:
	@touch $@

%.o: %.c .make-prerequisites
	$(REDIS_CC) -c $<

clean:
	rm -rf $(REDIS_SERVER_OBJ)
.PHONY: clean

test:

lcov:
	$(MAKE) gcov
.PHONY: lcov

gcov:
	$(MAKE) REDIS_CFLAGS="-fprofile-arcs -ftest-coverage -DCOVERAGE_TEST" REDIS_LDFLAGS="-fprofile-arcs -ftest-coverage"

noopt:
	$(MAKE) OPTIMIZATION="-O0"

src/help.h:
	@../utils/generate-command-help.rb > help.h

install: all 