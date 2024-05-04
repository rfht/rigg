PROG =		rigg
SRCS =		rigg.c rigg_unveil.c mono.c hl.c
		
BINDIR ?=	/usr/local/bin
MANDIR ?=	/usr/local/man/man
WARNINGS ?=	Yes

HL_CFLAGS ?=	-I/usr/local/include -I/usr/local/include/hl
J17_CFLAGS ?=	-I/usr/local/jdk-17/include
MONO_CFLAGS ?=	-I/usr/local/include/mono-2.0
COPTS +=	${HL_CFLAGS} ${MONO_CFLAGS}

LDFLAGS +=	-L/usr/local/lib -Wl,-z,wxneeded -Wl,-z,nobtcfi
LDADD +=	-lmonosgen-2.0 -lhl -lhl_module

CLEANFILES +=	test/*.exe

readme:
	mandoc -T markdown rigg.1 > README.md

.PHONY: test
test:
	${MAKE} -C test clean regress

uninstall:
	rm -f ${DESTDIR}${BINDIR}/${PROG}
	rm -f ${_MAN_INST}

.include <bsd.prog.mk>
