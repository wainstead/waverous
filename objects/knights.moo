@recycle knights

@create $thing named Huge Army Of Knights, knights

@describe knights as "Off in the distance, a huge army of knights in armor, some on horses, most standing in a horizontal line, holding spears and shields and banners."

@verb knights:"sh*out" this
@program knights:shout
message = "From afar, " + this:title() + " shout: ^@RED^@^@BACKWHITE^@GET ON WITH IT!^@reset^@";
player:tell(message);
player.location:announce(message);
.

@verb knights:title tnt
@program knights:title
return "^@HIWHITE^@a Huge Army of Knights^@reset^@";
.

drop knights