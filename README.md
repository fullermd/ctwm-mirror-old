# CTWM

## Intro

ctwm is an extension to twm, originally written by Claude Lecommandeur
that support multiple virtual screens, and a lot of other goodies.

You can use and manage up to 32 virtual screens called workspaces.  You
swap from one workspace to another by clicking on a button in an
optionnal panel of buttons (the workspace manager) or by invoking a
function.

You can customize each workspace by choosing different colors, names and
pixmaps for the buttons and background root windows.

Major features include:

* Optional 3D window titles and border (ala Motif).
* Shaped, colored icons.
* Multiple icons for clients based on the icon name.
* Windows can belong to several workspaces.
* A map of your workspaces to move quickly windows between
   different workspaces.
* Animations: icons, root backgrounds and buttons can be animated.
* Pinnable and sticky menus.
* etc...

The sources files were once the twm ones with only workmgr.[ch] added
(written from scratch by Claude Lecommandeur).  There were also some
modifications to some twm files.  This was back in ctwm version 1, by
now, there are a bit more changes and a few more additions.  It's
possible, thought it's doubtful, that twm patches can still be applied...
with great care.

If you find bugs in ctwm, or just want to tell us how much you like it,
please send a report to the mailing list.

There is a manual page, which always needs more work (any volunteers?).
Many useful information bits are only in the CHANGES.md file, so please
read it.


## Configuration

ctwm is build using CMake, which does its best to root around in your
system to find the pieces the build needs.  Occasionally though you might
have to give it some help, or change the defaults of what features are
expected.

In the common case, the included Makefile will do the necessary
invocations, and you won't need to worry about it; just run a normal
`make ; make install` invocation.  If you need to make alterations
though, you may have to invoke cmake manually and set various params on
the command line (cmake also has various GUI configurators, not covered
here).

The following parameters control configuration/installation locations:

CMAKE_INSTALL_PREFIX
:       Where paths are based.  This is a standard cmake var.

ETCDIR
:       Where ctwm will look for a `system.ctwmrc` to fall back to if it
        doesn't find a per-user config.  Nothing is installed here by
        default.
        (default: `$PREFIX/etc`)

BINDIR
:       Where the ctwm binary is installed.
        (default: `$PREFIX/bin`)

DATADIR
:       Where run-time data like image pixmaps are installed.
        (default: `$PREFIX/share/ctwm`)

MANDIR
:       Base directory under which manpage dirs like `man1` and `man2`
        live.
        (default: `$PREFIX/share/man` or `$PREFIX/man` if that doesn't exist)

DOCDIR
:       Where non-manpage docs are installed.
        (default: `$PREFIX/share/doc/ctwm`)

EXAMPLEDIR
:       Where various example files get installed.  These include the
        system.ctwmrc that is compiled into ctwm as a fallback.
        (default: `$PREFIX/share/examples/ctwm`)


The following parameters control the features/external libs that are
available.  The defaults can be changed by passing parameters like
`-DUSE_XYZ=OFF` to the cmake command line.

USE_M4
:       Enables use of m4(1) for preprocessing config files at runtime.
        If your m4 is called something other than `m4` or `gm4`, you may
        need to also set M4_CMD to point at it.
        (**ON** by default)

USE_XPM
:       Enables the use of XPM images.  Disable if libxpm isn't present,
        which is just barely possible on very old systems.
        (**ON** by default)

USE_JPEG
:       Enables the use of jpeg images via libjpeg.  Disable if libjpeg
        isn't present.
        (**ON** by default)

USE_EWMH
:       Enables EWMH support.
        (**ON** by default)

USE_SOUND
:       Build with sound support via librplay.
        (**OFF** by default)


Additional vars you might need to set:

M4_CMD
:       Name of m4 program, if it's not `m4` or `gm4`, or full path to it
        if it's not in your `$PATH`.


## Building

In the simple case, the defaults should work.  Most modern or semi-modern
systems should fall into this.

    funny prompt> make

If you need to add special config, you'll have to pass extra bits to
cmake via an invocation like

    funny prompt> make CMAKE_EXTRAS="-DUSE_XPM=OFF -DM4_CMD=superm4"

Though in more complicated cases it may be simpler to just invoke cmake
directly:

    funny prompt> ( cd build ; cmake -DUSE_XPM=OFF -DM4_CMD=superm4 .. )
    funny prompt> make



## Installation

    funny prompt> make install


## Dev and Support

### Mailing list

There is a mailing list for discussions: <ctwm@ctwm.org>.  Subscribe by
sending a mail with the subject "subscribe ctwm" to
<minimalist@ctwm.org>.

### Repository

ctwm development uses bazaar (see <http://bazaar.canonical.com/>) for
version control.  The code is available on launchpad as `lp:ctwm`.  See
<https://launchpad.net/ctwm> for more details.


## Further information

Additional information can be found from the project webpage, at
<http://www.ctwm.org/>.


{>>
 vim:expandtab:ft=markdown:
<<}