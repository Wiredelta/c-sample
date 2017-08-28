PACKAGE_BUGREPORT				= support@lavabit.com
PACKAGE_URL						= https://lavabit.com

TOPDIR							= $(realpath .)
MFLAGS							=
#MAKEFLAGS						= --output-sync=target --jobs=6
MAKEFLAGS						= --output-sync=target
DEPDIR							= .deps
OBJDIR							= .objs
CDEFINES						= -D_REENTRANT -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -DHAVE_NS_TYPE -DFORTIFY_SOURCE=2 -DMAGMA_PEDANTIC 

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

ifeq ($(VERBOSE),yes)
RUN								=
RM_VERB							= -v
else
RUN								= @
VERBOSE							= no
endif

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
MV								= mv --force $(RM_VERB)
RM								= rm --force $(RM_VERB)
RMRF							= rm --force --recursive $(RM_VERB)
RMDIR							= rmdir --parents --ignore-fail-on-non-empty $(RM_VERB)
MKDIR							= mkdir --parents $(RM_VERB)
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

warning:
ifeq ($(VERBOSE),no)
	@echo 
	@echo 'For a more verbose output' 
	@echo '  make '$(GREEN)'VERBOSE=yes' $(NORMAL)$(TARGETGOAL)
	@echo 
endif

# If verbose mode is disabled, we only output this finished message.
finished:
ifeq ($(VERBOSE),no)
	@echo 'Finished' $(BOLD)$(GREEN)$(TARGETGOAL)$(NORMAL)
endif

