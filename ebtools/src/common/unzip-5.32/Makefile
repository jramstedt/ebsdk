CONFIG_DIR=../../../..
include $(CONFIG_DIR)/sdkmake.conf

all:
	make -f unix/Makefile generic

install:
	cp unzip $(EB_TOOLBOX)/unzip

clean:
	make -f unix/Makefile clean
