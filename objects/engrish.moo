
@create $thing named engrish

@describe Engrish as "Engrish. How you do?"

@property engrish.title "Engrish mangulator"
@property engrish.color "magenta""

@verb engrish:"title" this none this

@program engrish:title
return "^@" + this.color + "^@" + this.title + "^@reset^@";
.

@verb engrish:"do" this with any
@program engrish:do
player:tell(this:title(), " ", iobjstr);
.


@verb engrish:"speak" this with any

@program engrish:speak
"The original purpose of this was to feed English to a translation site, like Google Translate, get back another language, and then send that language back and get it translated back into English. That way you wound up with a completely mangled sentence (i.e. 'engrish.').";
japanese = #736:translate("en|ja", iobjstr);
english = #736:translate("ja|en", japanese);
player:tell(this:title(), " says, \"", japanese, " (", english, ")", "\"");
.

