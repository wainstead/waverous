" Verbs on JHCore's system object contain hooks to the $mcp suite.
" The latest JHCore's verbs look like this:

@verb system:"do_out_of_band_command" this none this
@program system:do_out_of_band_command
if (callers())
  return E_PERM;
elseif (!valid(player))
  return;
endif
return $mcp:(verb)(@args);
.

@verb system:"user_created user_connected" this none this
@program system:user_created
user = args[1];
if (callers())
  raise(E_PERM);
endif
if (args[1] < #0)
  return;
endif
$mcp:(verb)(@args);
fork (0)
  `user:confunc() ! E_VERBNF';
endfork
fork (0)
  $login_watcher:hello(user);
endfork
`user.location:confunc(user) ! E_VERBNF';
`user:anyconfunc() ! E_VERBNF';
.



@verb system:"user_disconnected user_client_disconnected" this none this
@program system:user_disconnected
if (callers())
  return;
endif
user = args[1];
if (user < #0)
  return $network:connection_closed(user);
elseif ($login:is_tester(user))
  $login:tester_disconnecting(user);
endif
$mcp:(verb)(@args);
"...";
"... record disconnect time...";
user.last_disconnect_time = time();
user.total_connect_time = user.total_connect_time + (user.last_disconnect_time - user.last_connect_time);
if (user.started_keeping_total == 0)
  user.started_keeping_total = user.last_connect_time;
endif
"...";
fork (0)
  `user.location:disfunc(user) ! E_INVIND, E_VERBNF';
endfork
fork (0)
  $login_watcher:goodbye(user);
endfork
`user:disfunc() ! E_VERBNF';
.

@verb system:"user_reconnected" this none this
@program system:user_reconnected
user = args[1];
if (user < #0)
  return;
endif
$mcp:(verb)(@args);
fork (0)
  `user.location:reconfunc(user) ! E_INVIND, E_VERBNF';
endfork
fork (0)
  `user:reconfunc() ! E_VERBNF';
endfork
`user:anyconfunc() ! E_VERBNF';
.

