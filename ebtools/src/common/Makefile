SUBDIRS = common bit pvc list strip hex32 ic4mat srec srom repeat objtest \
	makerom uload sysgen rcsv hfcomp mini_dbg hexpad ship \
	unzip-5.32 zip-2.2

CONFIG_DIR=../../..
include $(CONFIG_DIR)/sdkmake.conf

# MFL = EB_BASE=$(EB_BASE) EB_TOOLBOX=$(EB_TOOLBOX)

all:	$(CHECK_CONFIG)
	for dir in $(SUBDIRS) ; do \
	    echo "----- Building in $$dir -----" ; \
	    cd $$dir ; $(EB_MAKE) $(MFL) ; cd .. ; \
	done

install: $(CHECK_CONFIG)
	for dir in $(SUBDIRS) ; do \
	    echo "----- Installing in $$dir -----" ; \
	    cd $$dir ; $(EB_MAKE) $(MFL) install ; cd .. ; \
	done

clean: $(CHECK_CONFIG)
	for dir in $(SUBDIRS) ; do \
	    echo "----- Cleaning in $$dir -----" ; \
	    cd $$dir ; $(EB_MAKE) $(MFL) clean ; cd .. ; \
	done

depend: $(CHECK_CONFIG)
	for dir in $(SUBDIRS) ; do \
	    echo "----- Genrating dependencies in $$dir -----" ; \
	    cd $$dir ; $(EB_MAKE) $(MFL) depend ; cd .. ; \
	done

check:
	@echo EB_BASE is $(EB_BASE)
	@echo EB_TOOLBOX is $(EB_TOOLBOX)
	@echo MFL is $(MFL)

