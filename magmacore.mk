#!/usr/bin/make -f

include common.mk

PACKAGE_NAME					= Magma Core
PACKAGE_TARNAME					= magmacore
PACKAGE_VERSION					= 6.4
PACKAGE_STRING					= $(PACKAGE_NAME) $(PACKAGE_VERSION)

MAGMA_CORE_SHARED				= $(addsuffix $(DYNLIBEXT), magmacore)
MAGMA_CORE_STATIC				= $(addsuffix $(STATLIBEXT), magmacore)

MAGMA_CORE_LSTATIC				= 
MAGMA_CORE_LDYNAMIC				= -lrt -ldl -lpthread -lresolv

FILTERED_CORE_SRCFILES			= 
MAGMA_CORE_SRCDIRS				= $(subst ./,,$(shell find ./src/core -type d -print))
MAGMA_CORE_SRCFILES				= $(filter-out $(FILTERED_CORE_SRCFILES), $(foreach dir, $(MAGMA_CORE_SRCDIRS), $(wildcard $(dir)/*.c)))

MAGMA_CORE_CINCLUDES			= #-I$(TOPDIR)

MAGMA_CORE_DEPFILES				= $(patsubst %.c,$(DEPDIR)/%.d, $(MAGMA_CORE_SRCFILES))
MAGMA_CORE_OBJFILES				= $(patsubst %.c,$(OBJDIR)/%.o, $(MAGMA_CORE_SRCFILES))

LDFLAGS							= -rdynamic -shared

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
endif
	$(RUN)$(LD) $(LDFLAGS) -o '$@' $(MAGMA_CORE_OBJFILES) -Wl,--start-group $(MAGMA_CORE_LDYNAMIC) $(MAGMA_CORE_LSTATIC) -Wl,--end-group

build-core: $(MAGMA_CORE_SHARED)

clean-impl:
	@$(RM) $(MAGMA_CORE_SHARED)
	@$(RM) $(MAGMA_CORE_DEPFILES)
	@$(RM) $(MAGMA_CORE_OBJFILES)
	@$(RMRF) $(DEPDIR)
	@$(RMRF) $(OBJDIR)

clean: warning clean-impl finished

all: warning build-core finished

# If we've already generated dependency files, use them to see if a rebuild is required
-include $(MAGMA_CORE_DEPFILES)

.DEFAULT_GOAL := all
.SUFFIXES: .c .cc .cpp .o 
.PHONY: all strip warning config finished check setup build-core clean clean-impl distclean install pprof gprof
