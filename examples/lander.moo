@recycle lander

@create $thing named Mars Polar Lander, lander

@describe lander as "You see a three-legged landing probe built by NASA, which set down here on December 3rd, 1999. It has two solar panels and a robotic arm for scooping up soil samples. A small dish antenna points at the sky." 

@verb lander:"pe*t pa*t" this

@program lander:pet
player:tell("You ", verb, " the ", this:title());
player:tell("The ", this:title(), " hums contently");
player.location:announce($name(player), " ", verb, "s ", this:title());
player.location:announce(this:title(), " hums contently");
.

@verb lander:"kn*ock ki*ck" this

@program lander:knock
if (this.state == "Safe Mode")
   this.state = "Search Mode";
   player:tell("You ", verb, " on the hull of NASA's ", this:title(), ".");
   player:tell(this:title(), " ", this.searchmode_msg);
   player:tell(this:title(), "'s antenna searches the sky for Earth.");
   player.location:announce($name(player), " knocks on the hull of NASA's ", this:title(), ".");
   player.location:announce(this:title(), " searches the sky for Earth.");
   fork (this.searchtimer)
      player:tell(this:title(), " cannot contact Earth.");
      player:tell(this:title(), " ", this.safemode_msg);
      player.location:announce(this:title(), " cannot contact Earth.");
      player.location:announce(this:title(), " ", this.safemode_msg);
      this.state = "Safe Mode";
   endfork
else
   player:tell("You ", verb, " the hull of NASA's ", this:title(), ".");
   player:tell(this:title(), " swivels its camera in your direction.");
   player:tell(this:title(), " takes your picture.");
   player.location:announce($name(player), " knocks on the hull of NASA's ", this:title(), ".");
   player.location:announce(this:title(), " swivles its camera in ", $name(player), "'s direction.");
   player.location:announce(this:title(), " takes a picture.");
   this.photo_cache[$list_utils:assoc(player)] = this.photo_cache[$list_utils:assoc(player)] + 1;
endif
.

@verb lander:state this
@program lander:state
player:tell("The ", this:title(), " is in ", this.state, ".");
.

@property lander.state "Safe Mode"
@property lander.searchtimer 60
@property lander.searchmode_msg ""
@property lander.safemode_msg ""
@property lander.photo_cache {}

@searchmode lander is "enters Search Mode."
@safemode lander is "enters Safe Mode and idles."

@verb lander:"g*et t*ake" this
@program lander:get
player:tell(this:title(), " weighs about 615 kg. You cannot lift it.");
.

@verb lander:"th*ought" this with any
@program lander:thought
player:tell(this:title(), " . o O ( ", iobjstr, " )");
player.location:announce(this:title(), " . o O ( ", iobjstr, " )");
.

@verb lander:"do" this with any
@program lander:do
player:tell(this:title(), " ", iobjstr);
player.location:announce(this:title(), " ", iobjstr);
.


@verb lander:sayto this with any
@program lander:sayto
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (!valid(who = $string_utils:match_player(name)))
   player:tell(name, "? Who's that?");
else
   player:tell(this:title(), " [to ", who:title(), "]: ", message);
   player.location:announce(this:title(), " [to ", who:title(), "]: ", message);
endif
.  


@verb lander:ani*mate any any any
@program lander:animate
if (index(iobjstr, "%n"))
   player:tell(strsub(iobjstr, "%n", this:title()));
   player.location:announce_all_but({player}, strsub(iobjstr, "%n", this:title()));
else
   player:tell("You need a %n in the string for this to work.");
endif
.

@verb lander:ping this none none
@program lander:ping
player:tell("You ping the ", this:title());
player:tell(this:title(), " [to ", player:title(), "]: pong!");
player.location:announce(player:title(), " [to ", this:title(), "]: ping");
player.location:announce(this:title(), " [to ", player:title(), "]: pong!");
.

@verb lander:"sez" this with any
@program lander:sez
firstspace = index(iobjstr, " ");
name = iobjstr[1..(firstspace - 1)];
message = iobjstr[(firstspace + 1)..$];
if (valid(who = $string_utils:match_object(name, this.location)))
   player:tell(this:title(), " [to ", who:title(), "]: ", message);
   player.location:announce(this:title(), " [to ", who:title(), "]: ", message);
else
   player:tell(name, "? Who or what is that?");
   player:tell("Usage: sez lander with <username or thing> <message>");
endif
.

@verb lander:"title" this none this
@program lander:title
return "Mars Polar Lander";
.

@property lander.help_msg "The Mars Polar Lander has several fun verbs to pick from. The Lander goes into Search Mode when you knock on its hull (if it's in Safe Mode); if you knock on the hull while it's in Search Mode it takes your picture. You can pet the Lander to make it happy. The 'state' verb will tell you its current mode, though in practice this hasn't been needed much. (I guess that makes it a debugging tool). You can give the lander thought bubbles with the 'thought' verb, i.e. th*ought lander with yup, I'm the Mars Polar Lander, ayup."

drop lander

