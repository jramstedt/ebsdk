# platform_support.make

# It is off in a separate file to provide an "escape" for doing wierd
# things for a platform.  The platform_<platform>.make file can omit the
# include of this file and use its own rules for building files.  Only
# useful if there is a naming conflict, and should be discouraged.

.SUFFIXES :
.SUFFIXES : .exe .slis .obj .lis .c .h .b32 .req .s .S .sdl

STUB_SRCS	= $(wildcard stub_*.c)
%.S : platform.S

$(OBJ)%.o : %.c
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(INCLUDES) $(OUTPUT_OPTION) -c $^

$(OBJ)%.o : %.S
	$(AS) $(ASFLAGS) -o $@ $^

$(OBJ)stubs.a : $(addprefix $(OBJ), $(STUB_SRCS:.c=.o))
	$(DELETE) $@
	$(AR) rcs $@ $^

do_x86 : 
	$(info $^ -> $@)