# Architecture-specific setup, included by Makefile

ifdef ALPHA
ifdef EV6
	CPUTYPE	= 21264
else
	ifdef BWX
		CPUTYPE	= 21164a
	else
		CPUTYPE	= 21164
	endif
endif

LC			= $(shell echo '$1' | tr '[:upper:]' '[:lower:]' )
AS			= alpha-linux-gnu-as
AR			= alpha-linux-gnu-ar
LD			= alpha-linux-gnu-ld
CC			= alpha-linux-gnu-gcc
CPP			= alpha-linux-gnu-cpp
OBJCOPY	= alpha-linux-gnu-objcopy
OBJDUMP	= alpha-linux-gnu-objdump

ASFLAGS		= -m$(call LC,$(CPUTYPE))
CPPFLAGS	= -fdollars-in-identifiers
CCFLAGS		= -c -Os -std=c99 -Wpedantic -Wall -Wextra -Wno-unused-parameter -fno-builtin -ffreestanding
#LDFLAGS		= --orphan-handling=warn -L$(BIOSEMLIB) -L$(OBJDIR) -Telf-coff.ld -z noexecstack -s --no-dynamic-linker -static
LDLIBS		=

.PHONY: setup
setup : 
	@date
	@$(shell ./console_tools.sh)
endif

ifdef VAX
$(error VAX not implemented.)
endif
