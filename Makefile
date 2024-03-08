PROG =		rigg
SRCS =		rigg.c rigg_mkdir.c rigg_unveil.c mono.c hl.c
#SRCS =		rigg.c rigg_unveil.c mono.c hl.c
		
PREFIX ?=	/usr/local
BINDIR =	${PREFIX}/bin
MANDIR =	${PREFIX}/share/man/man
WARNINGS ?=	Yes

MONO_CFLAGS ?=	-I/usr/local/include/mono-2.0
HL_CFLAGS ?=	-I/usr/local/include -I/usr/local/include/hl
J17_CFLAGS ?=	-I/usr/local/jdk-17/include
COPTS +=	${HL_CFLAGS} ${MONO_CFLAGS}

LDADD +=	-lmonosgen-2.0 -lhl -lhl_module
LDFLAGS +=	-L/usr/local/lib -Wl,-z,wxneeded -Wl,-z,nobtcfi

readme:
	mandoc -T markdown rigg.1 > README.md

uninstall:
	rm -f ${DESTDIR}${BINDIR}/${PROG}
	rm -f ${_MAN_INST}
.for subdir in ${SUBDIR}
	${MAKE} -C ${subdir} uninstall
.endfor

.include <bsd.prog.mk>
.include <bsd.subdir.mk>
