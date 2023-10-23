CC =		cc
PROG =		rigg
SRCS =		rigg.c rigg_unveil.c mono.c hl.c rigg.h rigg_mono.h \
		rigg_unveil.h
		
PREFIX ?=	/usr/local
BINDIR =	${PREFIX}/bin
MANDIR =	${PREFIX}/share/man/man

CFLAGS ?=	-Wall -Werror -O2 -pipe
MONO_CFLAGS ?=	-I/usr/local/include/mono-2.0
HL_CFLAGS ?=	-I/usr/local/include -I/usr/local/include/hl
J17_CFLAGS ?=	-I/usr/local/jdk-17/include
CFLAGS +=	${HL_CFLAGS} ${MONO_CFLAGS}

MONO_LDFLAGS ?=	-L/usr/local/lib -lmonosgen-2.0 -Wl,-z,wxneeded -Wl,-z,nobtcfi
HL_LDFLAGS ?=	-L/usr/local/lib -lhl -lhl_module -Wl,-z,wxneeded -Wl,-z,nobtcfi
LDADD =		${HL_LDFLAGS} ${MONO_LDFLAGS}

readme:
	mandoc -T markdown rigg.1 > README.md

.include <bsd.prog.mk>

uninstall:
	rm -f ${DESTDIR}${BINDIR}/${PROG}
	rm -f ${_MAN_INST}
