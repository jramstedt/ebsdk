default:	all

CONFIG_DIR=..
include $(CONFIG_DIR)/sdkmake.conf
include rules.gnu


EB_SUBDIRS=h srom palcode lib stubs ieee ether ladebug dbm demo dhry

all:	$(CHECK_CONFIG)
	@echo EB_BASE is ${EB_BASE}
	@echo TOP is ${TOP}
	for target in ${EB_TARGETS} ; do  \
            echo "======== Building target $$target ========" ; \
            for subdir in ${EB_SUBDIRS} ; do  \
                if [ -d ./$$subdir ] ; then  \
                    echo "######## Building $$subdir for target $$target ########" ; \
                    (cd ./$$subdir ; $(EB_MAKE) MAKEFILE=Makefile TARGET=$$target ) ; \
                fi ; done ; done



config:
	echo "Configuring EBSDK for your setup"
	./configure

clean:	$(CHECK_CONFIG)
	rm -f *~ \#*\# core
	for target in ${EB_TARGETS} ; do  \
            echo "======== Cleaning target $$target ========" ; \
            for subdir in ${EB_SUBDIRS} ; do  \
                if [ -d ./$$subdir ] ; then  \
                    echo "######## Cleaning $$subdir for target $$target ########" ; \
                    echo "(cd ./$$subdir ; $(EB_MAKE) MAKEFILE=Makefile TARGET=$$target clean )" ; \
                    (cd ./$$subdir ; $(EB_MAKE) MAKEFILE=Makefile TARGET=$$target clean ) ; \
                fi ; \
	    done ; \
	    rm -f obj/$$target/* ; \
	    rm -f $(EB_BOOTDIR)/$$target/* ; \
	done


