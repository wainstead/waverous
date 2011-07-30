@recycle michael

@create $thing named Michael Scott action figure, michael, mike

@describe michael as "A foot tall action figure of Michael Scott wearing a woman's business suit and a pull string hanging out its back."

@verb michael:"p*ull" this

@program michael:pull
"remove messages and use a msg variable."
player:tell("You ", verb, " the string on the back of the ", this:title());
player:tell(this:title(), " says, \"That's what she said!\"");
player.location:announce(player:title(), " ", verb, "s the string on the back of the ", this:title());
player.location:announce(this:title(), " says, \"That's what she said!\"");
.


drop michael

