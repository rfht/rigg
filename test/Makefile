REGRESS_TARGETS =	mono-visible mono-invisible mono-mkdir-wraptest

CLEANFILES +=	*.exe

.SUFFIXES: .cs .exe
.cs.exe:
	csc $<

# interact with visible part of the unveil(2)'d filesystem
mono-visible: MonoVisible.exe
	rigg mono MonoVisible.exe

# try to interact with invisible part of the unveil(2)'d filesystem
# should fail with ENOENT if unveil(2)'d correctly
mono-invisible: MonoInvisible.exe
	#...
	true

# test mkdir wrapper for privileged unveil(2)'d directories:
# /home, $HOME, $HOME/.local
mono-mkdir-wraptest: MonoMkdirWrapTest.exe
	#...
	true

.include <bsd.regress.mk>
