# PKSV: Pokémon Script Viewer & Editor

Note: Rewrite in progress, a lot will change. File formats and command names
are absolutely not set in stone.

## What does it do?

This is a script editor for the Pokémon games, specifically FRLG and RSE. In
this rewritten version, it is possible to make it compatible with any other
game with a scripting engine, or even things like Python bytecode if you're
feeling hacky, all by distributing a data file rather than recompiling the
whole software.

## How do I build it?

You can make a native version with `make compat`. If you're on Windows or
cross-compiling to Windows with the `make-mingw` script, you can just make the
default target and it will probably all build okay.

## Why is it called PKSV?

The original title was "PokeScript Viewer", and it was abbreviated from that.
The intent was to position it as the "next generation" of script editing
following in the footsteps of the PokeScript/Rubicon tool that came before it.
I don't remember what bugs I found in it, but it was definitely a project born
partly in anger.

As luck would have it, another tool called XSE was released at almost exactly
the same time with exactly the same purpose, which mostly eclipsed this project
in popularity. I recall a brief GUI-focused feature arms-race for a few months
but it wasn't enough to claw back any popularity.

Some of the features unique to this program at the time (as far as I am aware)
included Pokémon GSC script editing, and there was a short-lived attempt to
also add support for Pokémon DPP scripts, but I wasn't personally interested
enough in that version to see it through.

## The GUI doesn't work?!

Yep, this is an in-progress rewrite.

Try commit f282fe51b5f9a3478c122e194bdd41c74d9ea1a3 for something approximating
the pre-hiatus version of this software. You will only really get a sensible
build from it on Windows, so MinGW and WINE are the name of the game here,
unless you happen to run a Windows system.
