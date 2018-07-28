#---------------------------------------
# FLAGS
#---------------------------------------
include Make.properties
#---------------------------------------
# FILES
#---------------------------------------
SRCDIR=src

SUBDIRS=$(SRCDIR)
#---------------------------------------
# RULES
#---------------------------------------
all:
ifeq ($(shell find . -maxdepth 1 -name release),)
	mkdir release
endif
	@for i in $(SUBDIRS); do \
		echo "===> $$i"; \
		(cd $$i && $(MAKE) -f $(MAKEFILE)) || exit 1; \
		echo "<=== $$i"; \
	done
bin:
	@for i in $(SUBDIRS); do \
		echo "===> $$i"; \
		(cd $$i && $(MAKE) -f $(MAKEFILE) bin) || exit 1; \
		echo "<=== $$i"; \
	done

clean:
	rm src/main.o
	@for i in $(SUBDIRS); do \
		echo "===> $$i"; \
		(cd $$i && $(MAKE) clean -f $(MAKEFILE)) || exit 1; \
		echo "<=== $$i"; \
	done
#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------
