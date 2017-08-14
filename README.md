## Waverous LambdaMOO is a fork of the LambdaMOO server version 1.8.3.

To quote the original project: "LambdaMOO is a network-accessible, multi-user, programmable, interactive system well-suited to the construction of text-based adventure games, conferencing systems, and other collaborative software."

Waverous LambdaMOO:

* has been ported from C to C++
* adds Expat for XML parsing (kudos to Ken Fox for his file)
* has conditional compilation via choices when you run ./configure (i.e. --without-fup --with-fileio --with-expat --with-forked-checkpoints)
* Has an extensively updated set of automake and autoconf files

## Waverous changes were incorporated into Stunt.io

In 2013 I duplicated the C++ porting work I did for Waverous for [Todd Sundsted's clone of the LambdaMOO server](https://github.com/toddsundsted/stunt). At that point I stopped work on this project.

## A free book on using LambdaMOO

Elizabeth Hess has written a book about using LambdaMOO, "Yib's Guide to MOOing." The complete text is available as a PDF from her site: www.yibco.com 
