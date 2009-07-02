These patches assume a stock LambdaMOO 1.8.1 source tree with 
only the FUP package added (but NOT applied to extensions.c).  The
files it expects are:

ext-FUP.c        
ext-FUP_options.h

They must be applied in order, starting with tORIGINAL-d20040322.patch -
a shell script has been provided to ease in this and expects to be
run in the same directory as configure, and the rest of the source code.

It would not be a bad idea to apply them one by one following the
sequence in the apply_order.sh script to watch for rejections.

The vrandom stuff was never fully tested out or used in Avalon softcode
so I can't really speak for its viability.

The SSL connection host stuff is a hack that should go away once SSL 
support comes about.  It requires a modified stunnel implementation
to pass along the user's connection host via a magic parameter at 
the beginning of the connection conversation.

Everything else is more or less used on a daily basis on on Avalon.
