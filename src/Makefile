#
# Makefile in src
#
include ../Make.properties
TARGET=libcgh.a
BIN=main

# subdirs
SUBDIRS=$(foreach dir,$(shell find . -mindepth 1 -type d),$(shell echo $(dir) ' '))


# objs
ALL_OBJS = $(subst .cpp,.o,$(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.cpp)))


all: subdirs $(TARGET)
	mv $(TARGET) $(RELDIR)

bin: $(BIN)
	mv $(BIN) $(RELDIR)
	
$(BIN): $(ALL_OBJS) main.o
	$(CC) $(CPP_FLAGS) -o $@ $^

subdirs:
	@for i in $(SUBDIRS); do \
		echo "===> $$i"; \
		(cd $$i && $(MAKE) -f $(MAKEFILE)) || exit 1; \
		echo "<=== $$i"; \
	done

$(TARGET):$(ALL_OBJS)
	ar -r $@ $^

.PHONY:clean
clean:
	@for i in $(ALL_OBJS); do \
		echo "$$i"; \
		rm $$i; \
		done

