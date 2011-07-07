@verb system:"user_created user_connected" this none this
@program system:user_created
"Copied from The System Object (#0):user_connected by Slartibartfast (#4242) Sun May 21 18:14:16 1995 PDT";
if (callers())
  return;
endif
$mcp:(verb)(@args);
user = args[1];
set_task_perms(user);
try
  user.location:confunc(user);
  user:confunc();
except id (ANY)
  user:tell("Confunc failed: ", id[2], ".");
  for tb in (id[4])
    user:tell("... called from ", tb[1], ":", tb[2], ", line ", tb[6]);
  endfor
  user:tell("(End of traceback)");
endtry
.

@verb system:"user_disconnected user_client_disconnected" this none this
@program system:user_disconnected
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
`user:disfunc() ! ANY => 0';
`user.location:disfunc(user) ! ANY => 0';
.

@verb system:"user_reconnected" this none this
@program system:user_reconnected
if (callers())
  return;
endif
"JHCore uses the following test...";
"if (user < #0)";
"  return;";
"endif";
$mcp:(verb)(@args);
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
.

@verb system:"do_out_of_band_command doobc" this none this
@program system:do_out_of_band_command
if (callers())
  return E_PERM;
elseif (!valid(player))
  return;
endif
return $mcp:(verb)(@args);
"A cheap and very dirty do_out_of_band verb.  Forwards to verb on player with same name.";
if (!callers())
  try
    if (valid(player) && is_player(player))
      set_task_perms(player);
      player:do_out_of_band_command(@args);
    else
      $login:do_out_of_band_command(@args);
    endif
  except e (ANY)
    raise(e[1]);
  endtry
endif
.


