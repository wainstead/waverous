@recycle clippy

@create $thing named Clippy

@describe Clippy as "A stupid fucking paperclip."

@property clippy.title "Clippy"
@property clippy.color "YELLOW""

@verb clippy:"title" this none this

@program clippy:title
return "^@" + this.color + "^@" + this.title + "^@reset^@";
.

@verb clippy:"sez" this with any
@program clippy:sez
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
   player:tell("Usage: sez clippy with <username> <message>");
else
   player:tell(this:title(), " [to ", who:title(), "]: ", message);
   player.location:announce(this:title(), " [to ", who:title(), "]: ", message);
endif
.  


@verb clippy:"doto" this with any
@program clippy:doto
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
   player:tell("Usage: doto clippy with <username> <message>");
endif

if (newmessage = strsub(message, "%n", who:title()))
   player:tell(this:title(), " ", newmessage);
   player.location:announce(this:title(), " ", newmessage);
else
   player:tell("You need a %n in the message for me to interpolate");
   player:tell("the player's title and name.");
endif
.  

@verb clippy:"dowith" this with any
@program clippy:dowith
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
   player:tell("Usage: dowith clippy with <username> <message>");
   player:tell("where <message> contains a %n to place the clippy's name");
endif

if (newmessage = strsub(message, "%n", this:title()))
   player:tell(who:title(), " ", newmessage);
   player.location:announce(who:title(), " ", newmessage);
else
   player:tell("You need a %n in the message for me to interpolate");
   player:tell("the clippy's title and name.");
endif
.  



@verb clippy:"do" this with any
@program clippy:do
player:tell(this:title(), " ", iobjstr);
player.location:announce(this:title(), " ", iobjstr);
.

@verb clippy:"speak" this with any
@program clippy:speak
player:tell(this:title(), " says, \"", iobjstr, "\"");
player.location:announce(this:title(), " says, \"", iobjstr, "\"");
.

@verb clippy:"th*ought" this with any
@program clippy:thought
player:tell(this:title(), " . o O ( ", iobjstr, " )");
player.location:announce(this:title(), " . o O ( ", iobjstr, " )");
.


@property clippy.help_msg "To see the available commands do: summary clippy"

@verb clippy:"summary" any
@program clippy:summary
player:tell("to make a thought bubble, do: th*ought clippy with <message>");
player:tell("to make the clippy talk to another player do:");
player:tell("    sez clippy with <username> <message>");
player:tell("to make the clippy talk out loud, do:");
player:tell("    speak clippy with <message>");
player:tell("to make the clippy perform an action do:");
player:tell("    do clippy with <message>");
player:tell("to perform an action and insert the player's title:");
player:tell("    doto clippy with <username> <message>");
player:tell("    (but you have to insert a %n somewhere in the message)");
player:tell("    for example, 'doto clippy with w annoys %n'");
player:tell("    will yield:");
player:tell("    'clippy annoys Wainstead' (compare to 'dowith'");
player:tell("to perform an action on the clippy:");
player:tell("    dowith clippy with <username> <message>");
player:tell("    for example, 'dowith clippy with w annoys %n'");
player:tell("    will yield:");
player:tell("    'Wainstead annoys clippy' (compare to 'doto'");

.

drop clippy
