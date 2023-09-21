RIGG(1) - General Commands Manual

# NAME

**rigg** - run indie games

# SYNOPSIS

**rigg**
\[**-v**]
*engine*
*file*
\[*arguments*]

# DESCRIPTION

**rigg**
serves as an
OpenBSD-adapted runtime for indie games based on certain engines
(see
*engine*)
.
There are many exceptions, mainly consisting of games using proprietary
middleware.

**rigg**
handles availability of files by using
unveil(2)
to hide bundled files that conflict with
OpenBSD
runtimes and libraries.
This is a common occurrence with commercial games based on open-source
frameworks like FNA, LibGDX, or HashLink.
Besides hiding the conflicting files,
**rigg**
also enforces a restricted filesystem view of the minimum necessary for
the engine via
unveil(2).

The arguments are as follows:

*engine*

> Specify the engine to use.
> Can select from:
> *mono*,
> *hl*.

*file*

> The file to launch with the engine, e.g.
> *Game.exe*
> (mono)
> or
> *hlboot.dat*
> (hl).

**-v**

> enable verbose output.

# EXAMPLES

Run Mono game
*Game.exe*
with
**-windowed**
flag:

	$ rigg mono Game.exe -windowed

Run HashLink game
*hlboot.dat*
with verbose output:

	$ rigg -v hl hlboot.dat -windowed

# SEE ALSO

mono(1),
unveil(2)

# AUTHORS

Thomas Frohwein &lt;[thfr@openbsd.org](mailto:thfr@openbsd.org)&gt;

OpenBSD 7.3 - September 21, 2023
