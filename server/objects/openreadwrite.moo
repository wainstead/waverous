@recycle testbot
@create $thing named testbot,t

@describe testbot as "A testing robot."

@verb testbot:test this

@program testbot:test
myfile = "myfile.txt";
player:tell("Opening file " + myfile);
fh = file_open(myfile, "a+tf");
file_writeline(fh, "Boy, Howdy!");
file_close(fh);
player:tell("Closed the file.");
.

drop testbot
