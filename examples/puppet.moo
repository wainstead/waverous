@recycle puppet

@create $thing named Puppet

@describe Puppet as "A generic thingie you can rename and animate. Try 'test puppet' to see what it's currently set to; try 'summary puppet' to see all options."

@property puppet.title "puppet"
@property puppet.color "RED"
@property puppet.log {}

@verb puppet:"title" this none this

@program puppet:title
return "^@" + this.color + "^@" + this.title + "^@reset^@";
.

@verb puppet:"retitle" this to any
@program puppet:retitle
this.title = iobjstr;
this.aliases = {"puppet", @$string_utils:explode(iobjstr)};
player:tell("now using: ",  this:title());
.

@verb puppet:"test" this
@program puppet:test
player:tell("Title is now set to: '", this:title(), "'");
player:tell("Color is now set to: '", this.color, "'");
.

@verb puppet:"sez" this with any
@program puppet:sez
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (valid(who = $string_utils:match_object(name, this.location)))
   player:tell(this:title(), " [to ", who:title(), "]: ", message);
   player.location:announce(this:title(), " [to ", who:title(), "]: ", message);
else
   player:tell(name, "? Who or what is that?");
   player:tell("Usage: sez puppet with <username or thing> <message>");
endif
.


@verb puppet:"doto" this with any
@program puppet:doto
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
   player:tell("Usage: doto puppet with <username> <message>");
endif

if (newmessage = strsub(message, "%n", who:title()))
   player:tell(this:title(), " ", newmessage);
   player.location:announce(this:title(), " ", newmessage);
else
   player:tell("You need a %n in the message for me to interpolate");
   player:tell("the player's title and name.");
endif
.  

@verb puppet:"dowith" this with any
@program puppet:dowith
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
   player:tell("Usage: dowith puppet with <username> <message>");
   player:tell("where <message> contains a %n to place the puppet's name");
endif

if (newmessage = strsub(message, "%n", this:title()))
   player:tell(who:title(), " ", newmessage);
   player.location:announce(who:title(), " ", newmessage);
else
   player:tell("You need a %n in the message for me to interpolate");
   player:tell("the puppet's title and name.");
endif
.  



@verb puppet:"do" this with any
@program puppet:do
player:tell(this:title(), " ", iobjstr);
player.location:announce(this:title(), " ", iobjstr);
.

@verb puppet:"speak" this with any
@program puppet:speak
player:tell(this:title(), " says, \"", iobjstr, "\"");
player.location:announce(this:title(), " says, \"", iobjstr, "\"");
.

@verb puppet:"th*ought" this with any
@program puppet:thought
player:tell(this:title(), " . o O ( ", iobjstr, " )");
player.location:announce(this:title(), " . o O ( ", iobjstr, " )");
.


@verb puppet:"recolor" this with any
@program puppet:recolor
this.color = iobjstr;
player:tell("Color is now: ", this.color);
player:tell("result will be: '", this:title(), "'");
.

@verb puppet:"writelog" this any any
@program puppet:writelog
this.log = listappend(this.log, player.name + " " + argstr);
.

@verb puppet:"log" any
@program puppet:log
for entry in (this.log)
    player:tell(entry);
endfor
.

@verb puppet:"alii" any
@program puppet:alii
for alias in (this.aliases)
     player:tell(alias);
endfor
.

@property puppet.help_msg "To see the available commands do: summary puppet"

@verb puppet:"summary" any
@program puppet:summary
player:tell("to rename the puppet do: retitle puppet to <whatever>");
player:tell("to recolor the puppet do: recolor puppet with <ansi color>");
player:tell("to test the puppet do: test puppet");
player:tell("to make a thought bubble, do: th*ought puppet with <message>");
player:tell("to make the puppet talk to another player do:");
player:tell("    sez puppet with <username> <message>");
player:tell("to make the puppet talk out loud, do:");
player:tell("    speak puppet with <message>");
player:tell("to make the puppet perform an action do:");
player:tell("    do puppet with <message>");
player:tell("to perform an action and insert the player's title:");
player:tell("    doto puppet with <username> <message>");
player:tell("    (but you have to insert a %n somewhere in the message)");
player:tell("    for example, 'doto puppet with w kisses %n'");
player:tell("    will yield:");
player:tell("    'puppet kisses Wainstead' (compare to 'dowith'");
player:tell("to perform an action on the puppet:");
player:tell("    dowith puppet with <username> <message>");
player:tell("    for example, 'dowith puppet with w kisses %n'");
player:tell("    will yield:");
player:tell("    'Wainstead kisses puppet' (compare to 'doto'");

.

drop puppet
