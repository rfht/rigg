LIB ?=		mkdir_diag
SRCS ?=		mkdir_diag.c
SHLIB_MAJOR ?=	0
SHLIB_MINOR ?=	0
NOPROFILE ?=	Yes

PREFIX ?=	/usr/local
LIBDIR ?=	${PREFIX}/lib

uninstall:
	rm -f ${DESTDIR}${LIBDIR}/lib${LIB}.a
	rm -f ${DESTDIR}${LIBDIR}/lib${LIB}.so.${SHLIB_MAJOR}.${SHLIB_MINOR}

.include <bsd.lib.mk>
