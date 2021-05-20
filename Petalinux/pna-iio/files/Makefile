APP = pna-iio

# Add any other object files to this list below
APP_OBJS = pna-interface.o pna-prepare.o pna-gpio.o pna-base.o pna-capture.o pna-help.o pna-siggen.o pna-iio.o

PKG_CONFIG ?= pkg-config

NLLIBNAME := libiio libxml-2.0

LIBS += $(shell $(PKG_CONFIG) --libs $(NLLIBNAME))
CFLAGS += $(shell $(PKG_CONFIG) --cflags $(NLLIBNAME))
CFLAGS += -lm

all: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LIBS) -lm

clean:
	-rm -f $(APP) *.elf *.gdb *.o

# .c.o:
#    ${CC} ${CFLAGS} -c $<
