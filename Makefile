CFLAGS ?=	-Wall -Werror -O2 -pipe
MONO_CFLAGS ?=	-I/usr/local/include/mono-2.0
MONO_LDFLAGS ?=	-L/usr/local/lib -lmonosgen-2.0 -Wl,-z,wxneeded -Wl,-z,nobtcfi
CFLAGS +=	${MONO_CFLAGS} ${MONO_LDFLAGS}
CC ?=		clang
TARGET ?=	rigg

SOURCES =	rigg.c mono.c

.SUFFIXES: .c .o

all: rigg

rigg:
	${CC} ${CFLAGS} -o ${TARGET} ${SOURCES}

clean:
	rm -f rigg rigg_mono a.out

readme:
	mandoc -T markdown rigg.1 > README.md

mono:
	clang -I/usr/local/include/mono-2.0 -L /usr/local/lib/ -lmonosgen-2.0 -Wl,-z,wxneeded -Wl,-z,nobtcfi -fPIC -Wall -o rigg_mono mono.c
