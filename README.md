RIGG(1) - General Commands Manual

# NAME

**rigg** - run indie games

# SYNOPSIS

**rigg**
\[**-v**]
\[**-u**&nbsp;*strict*&nbsp;|&nbsp;*permissive*&nbsp;|&nbsp;*none*]
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

**-u**

> Set unveil mode.
> *strict*
> is the default and reduces read/write/create/execute permissions to a limited set of paths and hides game files as needed.
> *permissive*
> only hides game files as needed, but otherwise doesn't restrict filesystem access.
> *none*
> disables all uses of unveil.

**-v**

> Enable verbose output.

# EXIT STATUS

The **rigg** utility exits&#160;0 on success, and&#160;&gt;0 if an error occurs.
After successful launch of the engine, the
*engine*'s
exit status is returned.
See engine-specific documentation.

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

# CAVEATS

The selection of directories that are
unveil(2)'d
may not cover all potential use cases.

OpenBSD 7.5 - February 23, 2024
