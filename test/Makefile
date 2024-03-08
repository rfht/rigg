# *Visible*:		interact with visible part of the unveil(2)'d filesystem
# *Invisible*:		try to interact with invisible part of the unveil(2)'d filesystem
#			should fail with ENOENT if unveil(2)'d correctly
# *MkdirSpecial*:	Invisible directory, mkdir wrapper should return 0 anyway
REGRESS_TARGETS =	MonoVisible.exe MonoInvisible.exe
CLEANFILES +=		*.exe
CSCFLAGS +=		/nologo

.SUFFIXES: .cs .exe
.cs.exe:
	csc ${CSCFLAGS} $<
	rigg mono $@

.include <bsd.regress.mk>
