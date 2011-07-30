@recycle parrot

@create $thing named parrot
@describe parrot as "His Manginess, the Parrot."

@property parrot.title "Parrot"
@property parrot.color "higreen""

@verb parrot:"title" this none this

@program parrot:title
return "^@" + this.color + "^@" + this.title + "^@reset^@";
.

@verb parrot:"tell" this none this
@program parrot:tell
"The idea of the parrot was to watch for a particular player in the room to speak, then feed their sentences through a series of translations and post that to a channel. Note that stock LambdaCore doesn't have the channels feature, as LambdaMoo (the community itself) does.";
try 
        c = callers();
        if((c[$][2] == "say") && (c[$][5] == #295)) 
                fork (0)
                        foreign = #736:translate("en_nl", argstr);
                        english = #736:translate("nl_en", foreign);
                        $com:send_to_chan("parrot", "\"" + english + "\"");

                endfork
        endif
except e (ANY) return;
endtry 
.

@chmod parrot:tell +x
drop parrot
