RIGG(1) - General Commands Manual

# NAME

**rigg** - run indie games

# SYNOPSIS

**rigg**
\[**-v**]
**-e**&nbsp;*engine*
*file*
\[*arguments*]

# DESCRIPTION

**rigg**
serves as an OpenBSD-adapted runtime for indie games based on certain engines
(see
**-e**)
.
There are many exceptions, mainly consisting of games using proprietary middleware.

**rigg**
handles availability of files by using
unveil(2)
to hide bundled files that conflict with OpenBSD runtimes and libraries.
This is a common occurrence with commercial games based on open-source
framworks like FNA, LibGDX, or HashLink.
Besides hiding the conflicting files,
unveil(2)
is also used to enforce a restricted filesystem view of only the
minimum necessary.

The options are as follows:

**-e** *engine*

> Specify the engine to use. Can select from:
> *mono*

**-v**

> enable verbose output.

# SEE ALSO

unveil(2),
mono(1)

OpenBSD 7.3 - September 16, 2023
