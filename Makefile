CC ?=		clang
CFLAGS ?=	-Wall -Werror -O2 -pipe
OBJ ?=		rigg.o mono.o hl.o

MONO_CFLAGS ?=	-I/usr/local/include/mono-2.0
MONO_LDFLAGS ?=	-L/usr/local/lib -lmonosgen-2.0 -Wl,-z,wxneeded -Wl,-z,nobtcfi
HL_CFLAGS ?=	-I/usr/local/include -I/usr/local/include/hl
HL_LDFLAGS ?=	-L/usr/local/lib -lhl -lhl_module -Wl,-z,wxneeded -Wl,-z,nobtcfi
LDFLAGS +=	${HL_LDFLAGS} ${MONO_LDFLAGS}
CFLAGS +=	${HL_CFLAGS} ${MONO_CFLAGS}

all: rigg

rigg: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

.SUFFIXES: .c .o
.c.o:
	${CC} -c ${CFLAGS} $<

rigg.o: rigg.h

mono.o: rigg.h rigg_mono.h rigg_unveil.h

clean:
	rm -f rigg *.o

readme:
	mandoc -T markdown rigg.1 > README.md
