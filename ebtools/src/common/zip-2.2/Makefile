CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

all:
	make -f unix/Makefile generic

install:
	cp zip $(EB_TOOLBOX)/zip

clean:
	make -f unix/Makefile clean
