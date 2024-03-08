RIGG(1) - General Commands Manual

# NAME

**rigg** - run indie games

# SYNOPSIS

**rigg**
\[**-v**]
\[**-u**&nbsp;*strict*&nbsp;|&nbsp;*permissive*&nbsp;|&nbsp;*none*]
*engine*
*file*
\[*game&nbsp;arguments*]

# DESCRIPTION

**rigg**
serves as a thin,
OpenBSD-adapted runtime wrapper for indie games based on certain engines
(see
*engine*).
**rigg**
handles visibility of files by using
unveil(2)
to hide those that conflict with execution on
OpenBSD.
This solves a common problem with commercial games based on open-source
frameworks like FNA, LibGDX, or HashLink.
In addition,
**rigg**
also enforces a minimum-necessary filesystem view by default via
unveil(2).
Some basic engine configuration for
OpenBSD
like dllmap for
mono(1)
is included. More complex configuration like filesystem changes or
game-specific environment variables are out of scope.

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
> is the default and reduces read/write/create/execute permissions to a limited
> set of paths and hides game files that interfere with execution on
> OpenBSD.
> *permissive*
> only hides game files when needed for engine execution, but otherwise doesn't
> restrict filesystem access.
> *none*
> disables all uses of unveil and is generally equivalent to running the
> *engine*'s
> own binary.

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
with a game-specific flag
**-windowed**:

	$ rigg mono Game.exe -windowed

Run HashLink game
*hlboot.dat*
with verbose output:

	$ rigg -v hl hlboot.dat

# SEE ALSO

mono(1),
unveil(2)

# AUTHORS

Thomas Frohwein &lt;[thfr@openbsd.org](mailto:thfr@openbsd.org)&gt;

# CAVEATS

The selection of directories that are
unveil(2)ed
may not cover all potential use cases.
Some games fail to launch or have bugs in
*strict*
mode if they try to
stat(2)
*/home*
or other hidden directories.

OpenBSD 7.5 - March 8, 2024
