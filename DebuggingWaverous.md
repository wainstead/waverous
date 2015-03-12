Compile the server with the '-g' flag:

```
make CFLAGS='-g -O0 -m32 -Wall'
```

With optimizations turned off, gdb will step through the code in a way that makes more sense to you... otherwise it has a way of jumping around in a block that is slightly unnerving.


Launch the server in gdb this way:

```
gdb --annotate=3 --args ./moo databases/LambdaCore-12Apr99.db databases/my.db 
```

(Or whichever database you prefer to use). Before you type 'run' type 'break main' to set a breakpoint at the start of main() in server.c.

See also: HackingWaverous