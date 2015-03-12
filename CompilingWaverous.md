Clone the Git repository and run configure:

`git clone https://yourusername@code.google.com/p/waverous/ `

`cd waverous/server`

`./configure`

Waverous will compile with the default system C++ compiler, typically gcc (via g++).

`make CFLAGS='-g -m32 -Wall'`

For the moment it may or may not compile... as I write it compiles on OSX and Linux, but not NetBSD. I will keep bugs ("issues") up to date on compilation status. You need -m32 on 64 bit Darwin systems (basically, any Mac after 2007 or so) to avoid segmentation faults.

**Running waverous**

Waverous works like any LambdaMOO. The copy you downloaded includes a version of the Lambdamoo database, LambdaCore-12Apr99.db. For instructions on starting and stopping the server, read the README file; the basic flow is this:
`gunzip ./databases/LambdaCore-12Apr99.db.gz` (you only need to do this once)
`$ ./mooctl ./databases/LambdaCore-12Apr99`

Then you can telnet to port 7777 to connect:

`$ telnet localhost 7777`

You will see the prompt to login to your new moo server; type "connect wizard" to start. For a better client-side experience you might want to check out TinyFugue, a full screen client for many text-based worlds.

http://tinyfugue.sourceforge.net/

If you are running OS X you can install TinyFugue via [MacPorts](http://www.macports.org/):

`port install tf`

Administering a LambdaMOO is beyond the scope of this document; there are many resources on the net though:

http://the-b.org/moo/new-archwiz-faq.html

For a list of my moo links see http://www.delicious.com/wainstead/moo