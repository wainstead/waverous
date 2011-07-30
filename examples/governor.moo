@recycle gov

@create $thing named Governor Schwarzenegger Doll, gov, arnold, ahnold,schw

@describe gov as "Ahnold."

@verb gov:"p*ull" this

@program gov:pull
player:tell("You ", verb, " the string on the back of the ", this:title());
player:tell(this:title(), " stiffly says, \"Vote for me if you want to live.\"");
player.location:announce(player:title(), " ", verb, "s the string on the back of the ", this:title());
player.location:announce(this:title(), " stiffly says, \"Vote for me if you want to live.\"");
.

@verb gov:"speak" this with any
@program gov:speak
player:tell(this:title(), " says, \"", iobjstr, "\"");
player.location:announce(this:title(), " says, \"", iobjstr, "\"");
.


drop gov

;player:my_match_object("gov").ansi_color="hiwhite"

