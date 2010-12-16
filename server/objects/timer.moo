@create $thing named timer:timer
@prop #9999."help_msg" {} rc
;;#9999.("help_msg") = {"This is a simple timer. To use it, type 'set timer
for <duration>'. Here are a few examples:", "", "set timer for 3 minutes", "",
"set timer for 45 seconds", "", "set timer for an hour", "", "The timer will
prompt you for an optional reminder message.", "", "You can time more than one
thing at once. That is, you don't have to wait until the first time is up
before setting the timer for another thing. Reminder messages are especially
helpful when you are timing several things simultaneously.", ""}
@prop #9999."obvious_verbs" {} rc
;;#9999.("obvious_verbs") = {"", "set %<what> for <duration>", "", "give/hand
%<what> to <anyone>", "", "get/take %<what>", "", "drop/throw %<what>", "",
"help %<what>", ""}
;;#9999.("key") = #430
;;#9999.("aliases") = {"timer"}
;;#9999.("description") = "A simple timer, like one you would find in the
kitchen."
;;#9999.("object_size") = {0, 0}
;;#9999.("created_on") = 1002052438
;;#9999.("ansi_color") = "HIWHITE"

@verb #9999:"set" this for any rxd
@program #9999:set
"Usage: set <this> for <duration>";
"Example: set timer for 1 hour";
duration = $time_utils:parse_english_time_interval(iobjstr);
if (typeof(duration) == NUM)
  "We got a good value for duration.";
  message = $command_utils:read("an optional reminder message");
  if (!message)
    be = (iobjstr[$] == "s") ? "are" | "is";
    message = $string_utils:capitalize(((iobjstr + " ") + be) + " up.");
  endif
  message = " Ding! " + message;
  fork (duration)
    player:tell_lines({"", message, ""});
  endfork
  player:tell("The timer starts ticking.");
else
  "Didn't get a good value for duration.";
  player:tell($string_utils:pronoun_sub("Try something like 'set %t for 3
minutes'."));
endif
.


@verb #9999:"examine_verbs" this none this
@program #9999:examine_verbs
what = dobjstr;
vrbs = {};
for vrby in (this.obvious_verbs)
  vrbs = {@vrbs, $string_utils:substitute(vrby, {{"%<what>", what}})};
endfor
return {"Obvious verbs:", @vrbs};
.

@verb #9999:"title" this none this
@program #9999:title
return "^@HIWHITE^@timer^@reset^@";
.

