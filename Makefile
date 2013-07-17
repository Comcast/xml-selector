CC=gcc

XML_CONFIG=xml2-config
XML_CFLAGS=`${XML_CONFIG} --cflags`
CFLAGS=-I. ${XML_CFLAGS} -fPIC

XML_LDFLAGS=`${XML_CONFIG} --libs`
LDFLAGS=${XML_LDFLAGS}

DIRS=build

OBJECTS=build/nodelist.o

.DEFAULT: all

.PHONY: all dirs clean

all: dirs ${OBJECTS}

dirs:
	@for d in ${DIRS}; do if [ ! -d $$d ]; then mkdir $$d; fi; done

clean:
	@rm -rf build

build/%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<
