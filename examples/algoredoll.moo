@recycle alg

@create $thing named Al Gore action figure, algore, alg, gore

@describe alg as "A foot tall action figure of Al Gore with a dour look on its face and a pull string hanging out its back."

@verb alg:"p*ull" this

@program alg:pull
player:tell("You ", verb, " the string on the back of the ", this:title());
player:tell(this:title(), " haltingly says, \"You... are... hearing... me... talk.\"");
player.location:announce(player:title(), " ", verb, "s the string on the back of the ", this:title());
player.location:announce(this:title(), " haltingly says, \"You... are... hearing... me... talk.\"");
.


drop alg


;gore.ansi_color="cyan"
