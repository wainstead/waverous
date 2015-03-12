Waverous LambdaMOO is a fork of the LambdaMOO server version 1.8.3.

To quote the original project: "LambdaMOO is a network-accessible, multi-user, programmable, interactive system well-suited to the construction of text-based adventure games, conferencing systems, and other collaborative software."

Waverous LambdaMOO:

* has been converted to compile as C++
* adds Expat for XML parsing (kudos to Ken Fox for his file).
* has conditional compilation via choices when you run ./configure (i.e. --without-fup --with-fileio --with-expat --with-forked-checkpoints) 

Future goals include:

* SSL support
* UTF-8
* 64 bit safety
* IPv6 support
* SQLite support (in the form of a MOO object programmers can subclass) 

See also:

    CompilingWaverous
    HackingWaverous
    DebuggingWaverous 

Elizabeth Hess has written a book about using LambdaMOO, Yib's Guide to MOOing. The complete text is available as a PDF from her site: www.yibco.com 
