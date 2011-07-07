"For enCore
"
"    :user_reconnected sure looks to have a bug, user:confunc is
"    never called if you're not a guest.  perhaps they have no
"    concept of :reconfunc or similar, that might explain it.

@program #0:user_reconnected
if (callers())
  return;
endif
if ($object_utils:isa(user = args[1], $guest))
  "from $guest:boot";
  oldloc = user.location;
  move(user, $nothing);
  "..force enterfunc to be called so that the newbie gets a room description.";
  move(user, user.home);
  user:do_reset();
  if ($object_utils:isa(oldloc, $room))
    oldloc:announce("In the distance you hear someone's alarm clock going off.");
    if (oldloc != user.location)
      oldloc:announce(user.name, " wavers and vanishes into insubstantial mist.");
    else
      oldloc:announce(user.name, " undergoes a wrenching personality shift.");
    endif
  endif
  set_task_perms(user);
  `user:confunc() ! ANY';
endif
$mcp:(verb)(@args);
.

@program #0:user_disconnected
if (callers())
  return;
endif
if (args[1] < #0)
  "not logged in user.  probably should do something clever here involving Carrot's no-spam hack.  --yduJ";
  return;
endif
$mcp:(verb)(@args);
user = args[1];
user.last_disconnect_time = time();
set_task_perms(user);
fork (0)
  $login_watcher:goodbye(user);
endfork
`user:disfunc() ! ANY => 0';
`user.location:disfunc(user) ! ANY => 0';
"Last modified Sun Jul 20 13:23:16 1997 CDT by Wizard (#2).";
.

@program #0:user_created
if (callers())
  return;
endif
user = args[1];
try
  "Added to provide guests with the opportunity to select their own names, Jan";
  if ((user in children($guest)) && $real_guest_names)
    user:set_guest_name(user);
  endif
  set_task_perms(user);
  user.location:confunc(user);
  user:confunc();
  fork (0)
    $login_watcher:hello(user);
  endfork
except id (ANY)
  user:tell("Confunc failed: ", id[2], ".");
  for tb in (id[4])
    user:tell("... called from ", tb[1], ":", tb[2], ", line ", tb[6]);
  endfor
  user:tell("(End of traceback)");
endtry
$mcp:(verb)(@args);
"Last modified Wed Apr 15 13:13:47 1998 CDT by Wizard (#2).";
.

@program #0:do_out_of_band_command
if (callers())
  return E_PERM;
elseif (!valid(player))
  return;
else
  set_task_perms(player);
endif
if ((length(args) > 1) && (args[1] == "#$#MacMOOSE"))
  oob_verb = args[2];
  tokens = $macmoose_utils:simple_tokenize(argstr);
  oob_args = ((len = length(tokens)) > 2) ? tokens[3..len] | {};
  $macmoose_utils:(oob_verb)(@oob_args);
endif
return $mcp:(verb)(@args);
.
