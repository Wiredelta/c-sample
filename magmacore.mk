#!/usr/bin/make -f

include common.mk

TOPDIR							= $(realpath .)
MFLAGS							=
#MAKEFLAGS						= --output-sync=target --jobs=6
MAKEFLAGS						= --output-sync=target

# Identity of this package.
PACKAGE_NAME					= Magma Core
PACKAGE_TARNAME				= magmacore
PACKAGE_VERSION				= 6.4
PACKAGE_STRING					= $(PACKAGE_NAME) $(PACKAGE_VERSION)
PACKAGE_BUGREPORT				= support@lavabit.com
PACKAGE_URL					= https://lavabit.com

ifeq ($(OS),Windows_NT)
    HOSTTYPE 					:= "Windows"
    LIBPREFIX					:= 
    DYNLIBEXT					:= ".dll"
    STATLIBEXT					:= ".lib"
    EXEEXT 						:= ".exe"
else
    HOSTTYPE					:= $(shell uname -s)
    LIBPREFIX					:= lib
    DYNLIBEXT					:= .so
    STATLIBEXT					:= .a
    EXEEXT						:= 
endif

MAGMA_CORE_SHARED				= $(addsuffix $(DYNLIBEXT), magmacore)
MAGMA_CORE_STATIC				= $(addsuffix $(STATLIBEXT), magmacore)
MAGMA_CORE_CHECK_PROGRAM	= $(addsuffix $(EXEEXT), magmacore.check)

MAGMA_CORE_LSTATIC				= 
MAGMA_CORE_LDYNAMIC			= -lrt -ldl -lpthread -lresolv

FILTERED_CORE_SRCFILES			= 
MAGMA_CORE_SRCDIRS				= $(subst ./,,$(shell find ./src/core -type d -print))
MAGMA_CORE_SRCFILES				= $(filter-out $(FILTERED_CORE_SRCFILES), $(foreach dir, $(MAGMA_CORE_SRCDIRS), $(wildcard $(dir)/*.c)))

MAGMA_CORE_CINCLUDES			= #-I$(TOPDIR)
MAGMA_CORE_CHECK_CINCLUDES	= 
CDEFINES						= -D_REENTRANT -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -DHAVE_NS_TYPE -DFORTIFY_SOURCE=2 -DMAGMA_PEDANTIC 

#MAGMA_CORE_CHECK_STATIC		= $(MAGMA_CORE_STATIC) $(TOPDIR)/lib/local/lib/libcheck.a
MAGMA_CORE_CHECK_STATIC		=
MAGMA_CORE_CHECK_DYNAMIC		= $(MAGMA_CORE_LDYNAMIC) -L$(TOPDIR)/magmacore
MAGMA_CORE_CHECK_SRCDIRS		= $(shell find check/magma/core -type d -print)
MAGMA_CORE_CHECK_SRCFILES		= $(foreach dir, $(MAGMA_CORE_CHECK_SRCDIRS), $(wildcard $(dir)/*.c))

MAGMA_CORE_CINCLUDES				= -Isrc -Isrc/providers -I$(INCDIR) $(addprefix -I,$(MAGMA_CORE_INCLUDE_ABSPATHS))
MAGMA_CORE_CHECK_CINCLUDES		= -Icheck/magma -Ilib/local/include/ $(MAGMA_CINCLUDES) $(addprefix -I,$(MAGMA_CHECK_INCLUDE_ABSPATHS))
MAGMA_CORE_CHECK_DEPFILES			= $(patsubst %.c,$(DEPDIR)/%.d,$(MAGMA_CORE_CHECK_SRCFILES))
MAGMA_CORE_CHECK_OBJFILES		= $(patsubst %.c,$(OBJDIR)/%.o,$(MAGMA_CORE_CHECK_SRCFILES))
MAGMA_CHECK_INCLUDE_ABSPATHS	+= $(foreach target,$(MAGMA_CORE_CHECK_INCDIRS), $(call INCLUDE_DIR_SEARCH,$(target)))

#PACKAGE_DEPENDENCIES			= $(MAGMA_CORE_SHARED) $(MAGMA_CORE_STATIC) $(filter-out $(MAGMA_CORE_STATIC), $(MAGMA_CORE_CHECK_STATIC))

check: config warning $(MAGMA_CORE_CHECK_PROGRAM) finished
	$(RUN)$(TOPDIR)/$(MAGMA_CORE_CHECK_PROGRAM) sandbox/etc/magma.sandbox.config > loggggg.txt

# The Magma Core Unit Test Object Files
$(OBJDIR)/check/magma/core/%.o: check/magma/core/%.c
ifeq ($(VERBOSE),no)
	@echo 'Building' $(YELLOW)$<$(NORMAL)
endif
	@test -d $(DEPDIR)/$(dir $<) || $(MKDIR) $(DEPDIR)/$(dir $<)
	@test -d $(OBJDIR)/$(dir $<) || $(MKDIR) $(OBJDIR)/$(dir $<)
	$(RUN)$(CC) -o '$@' $(CFLAGS) $(CDEFINES) $(CFLAGS.$(<F)) $(CDEFINES.$(<F)) $(MAGMA_CORE_CHECK_CINCLUDES) -MF"$(<:%.c=$(DEPDIR)/%.d)" -MT"$@" "$<"

# Construct the magma core unit test executable.
$(MAGMA_CORE_CHECK_PROGRAM): $(PACKAGE_DEPENDENCIES) $(MAGMA_CORE_CHECK_OBJFILES)  $(MAGMA_CORE_SHARED)
ifeq ($(VERBOSE),no)
	@echo 'Constructing' $(RED)$@$(NORMAL)
endif
	$(RUN)$(LD) $(LDFLAGS) -o '$@' $(MAGMA_CORE_CHECK_OBJFILES) -Wl,--start-group,--whole-archive $(MAGMA_CORE_CHECK_STATIC) -Wl,--no-whole-archive,--end-group $(MAGMA_CORE_CHECK_DYNAMIC)

# Hidden Directory for Dependency Files
DEPDIR							= .deps
MAGMA_CORE_DEPFILES				= $(patsubst %.c,$(DEPDIR)/%.d, $(MAGMA_CORE_SRCFILES))

# Hidden Directory for Object Files
OBJDIR							= .objs
MAGMA_CORE_OBJFILES				= $(patsubst %.c,$(OBJDIR)/%.o, $(MAGMA_CORE_SRCFILES))

# Compiler Parameters
CC								= gcc
CFLAGS							= -std=gnu99 -O0 -fPIC -fmessage-length=0 -ggdb3 -rdynamic -c $(CFLAGS_WARNINGS) -MMD 
CFLAGS_WARNINGS					= -Wall -Werror -Winline -Wformat-security -Warray-bounds #-Wfatal-errors
CFLAGS_PEDANTIC					= -Wextra -Wpacked -Wunreachable-code -Wformat=2

CPP								= g++
CPPFLAGS						= -std=c++0x $(CPPFLAGS_WARNINGS) -Wno-unused-parameter -pthread -g3 
CPPFLAGS_WARNINGS				= -Werror -Wall -Wextra -Wformat=2 -Wwrite-strings -Wno-format-nonliteral #-Wfatal-errors

# Linker Parameters
LD								= gcc
LDFLAGS							= -rdynamic -shared

# Archiver Parameters
AR								= ar
ARFLAGS							= rcs

# Strip Parameters
STRIP							= strip
STRIPFLAGS						= --strip-debug

# GProf Parameters
GPROF							= -pg -finstrument-functions -fprofile-arcs -ftest-coverage

# PProf Parameters
PPROF							= -lprofiler

# Other External programs
MV								= mv --force
RM								= rm --force
RMRF							= rm --force --recursive
RMDIR							= rmdir --parents --ignore-fail-on-non-empty
MKDIR							= mkdir --parents
RANLIB							= ranlib
INSTALL							= install

# Text Coloring
RED								= $$(tput setaf 1)
BLUE							= $$(tput setaf 4)
GREEN							= $$(tput setaf 2)
WHITE							= $$(tput setaf 7)
YELLOW							= $$(tput setaf 3)

# Text Weighting
BOLD							= $$(tput bold)
NORMAL							= $$(tput sgr0)

# The Magma Core Object Files
$(OBJDIR)/%.o: %.c 
ifeq ($(VERBOSE),no)
	@echo 'Building' $(YELLOW)$<$(NORMAL)
endif
	@test -d $(DEPDIR)/$(dir $<) || $(MKDIR) $(DEPDIR)/$(dir $<)
	@test -d $(OBJDIR)/$(dir $<) || $(MKDIR) $(OBJDIR)/$(dir $<)
	$(RUN)$(CC) -o '$@' $(CFLAGS) $(CDEFINES) $(CFLAGS.$(<F)) $(CDEFINES.$(<F)) $(MAGMA_CORE_CINCLUDES) -MF"$(<:%.c=$(DEPDIR)/%.d)" -MT"$@" "$<"

$(MAGMA_CORE_SHARED): $(PACKAGE_DEPENDENCIES) $(MAGMA_CORE_OBJFILES)
ifeq ($(VERBOSE),no)
	@echo 'Constructing' $(RED)$@$(NORMAL)
else
	@echo
endif
	$(RUN)$(LD) $(LDFLAGS) -o '$@' $(MAGMA_CORE_OBJFILES) -Wl,--start-group $(MAGMA_CORE_LDYNAMIC) $(MAGMA_CORE_LSTATIC) -Wl,--end-group

ifeq ($(VERBOSE),yes)
RUN								=
else
RUN								= @
VERBOSE							= no
endif

build-core: $(MAGMA_CORE_SHARED)

clean-impl:
	@$(RM) $(MAGMA_CORE_SHARED)
	@$(RM) $(MAGMA_CORE_DEPFILES)
	@$(RM) $(MAGMA_CORE_OBJFILES)
	@$(RMRF) $(DEPDIR)
	@$(RMRF) $(OBJDIR)

clean: warning clean-impl finished

all: warning build-core check finished

# If we've already generated dependency files, use them to see if a rebuild is required
-include $(MAGMA_CORE_DEPFILES)

.DEFAULT_GOAL := all
.SUFFIXES: .c .cc .cpp .o 
.PHONY: all strip warning config finished check setup build-core clean clean-impl distclean install pprof gprof