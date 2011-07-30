@create $root_class named "MCP 2.1",mcp
@prop mcp."parser" __PARSER__ rc
@prop mcp."session" __SESSION__ r
@prop mcp."version" {} rc
;mcp.("version") = {2, 1}
@prop mcp."package" __PACKAGE__ r __WIZARD__
@prop mcp."negotiate" __NEGOTIATE__ r __WIZARD__
@prop mcp."registry" __REGISTRY__ rc
@prop mcp."cord" __MCP-CORD__ r
;mcp.("unique") = 0

@verb mcp:"create_session" this none this
@program mcp:create_session
{connection} = args;
if (caller != this)
  raise(E_PERM);
elseif (typeof(session = $recycler:_create(this.session)) != OBJ)
  raise(session);
endif
session:set_connection(connection);
session:initialize_connection();
return session;
.

@verb mcp:"destroy_session" this none this
@program mcp:destroy_session
{session} = args;
if (!(caller in {this, session}))
  raise(E_PERM);
elseif (!$object_utils:isa(session, this.session))
  raise(E_INVARG);
elseif (session == this.session)
  raise(E_INVARG);
else
  $recycler:_recycle(session);
endif
.

@verb mcp:"initialize_connection" this none this rxd __WIZARD__
@program mcp:initialize_connection
{who} = args;
if (caller != this)
  raise(E_PERM);
endif
return this:create_session(who);
.

@verb mcp:"finalize_connection" this none this rxd __WIZARD__
@program mcp:finalize_connection
{con} = args;
if (caller == con)
  this:destroy_session(con);
endif
.

@verb mcp:"parse_version" this none this rxd __WIZARD__
@program mcp:parse_version
"string version number -> {major, minor}";
{version} = args;
if (m = match(version, "%([0-9]+%)%.%([0-9]+%)"))
  return {tonum(substitute("%1", m)), tonum(substitute("%2", m))};
endif
.

@verb mcp:"compare_version_range" this none this rxd __WIZARD__
@program mcp:compare_version_range
{client, server} = args;
{min1, max1} = client;
{min2, max2} = server;
min1 = (typeof(min1) == STR) ? this:parse_version(min1) | min1;
min2 = (typeof(min2) == STR) ? this:parse_version(min2) | min2;
max1 = (typeof(max1) == STR) ? this:parse_version(max1) | max1;
max2 = (typeof(max2) == STR) ? this:parse_version(max2) | max2;
if (!(((min1 && min2) && max1) && max2))
  return;
else
  if ((this:compare_version(max1, min2) <= 0) && (this:compare_version(max2, min1) <= 0))
    if (this:compare_version(max1, max2) < 0)
      return max2;
    else
      return max1;
    endif
  endif
endif
return 0;
.

@verb mcp:"compare_version" this none this rxd __WIZARD__
@program mcp:compare_version
"-1 if v1 > v2, 0 if v1 = v2, 1 if v1 < v2";
{v1, v2} = args;
if (v1 == v2)
  return 0;
else
  {major1, minor1} = v1;
  {major2, minor2} = v2;
  if (major1 == major2)
    if (minor1 > minor2)
      return -1;
    else
      return 1;
    endif
  elseif (major1 > major2)
    return -1;
  else
    return 1;
  endif
endif
.

@verb mcp:"unparse_version" this none this rxd __WIZARD__
@program mcp:unparse_version
{major, minor} = args;
return tostr(major, ".", minor);
.

@verb mcp:"session_for" this none this rxd __WIZARD__
@program mcp:session_for
{who} = args;
return `who.out_of_band_session ! E_PROPNF => $failed_match';
.

@verb mcp:"user_created user_connected user_reconnected" this none this rxd __WIZARD__
@program mcp:user_created
{who} = args;
if ($list_utils:assoc(caller, listeners()))
  if ($recycler:valid(who.out_of_band_session))
    `who.out_of_band_session:finish() ! ANY';
  endif
  who.out_of_band_session = this:initialize_connection(who);
endif
.

@verb mcp:"user_disconnected user_client_disconnected" this none this rxd __WIZARD__
@program mcp:user_disconnected
{who} = args;
if ($list_utils:assoc(caller, listeners()))
  if ($recycler:valid(who.out_of_band_session))
    `who.out_of_band_session:finish() ! ANY';
    who.out_of_band_session = $nothing;
  endif
else
  raise(E_PERM);
endif
.

@verb mcp:"do_out_of_band_command" this none this rxd __WIZARD__
@program mcp:do_out_of_band_command
if ($list_utils:assoc(caller, listeners()))
  if ($recycler:valid(session = player.out_of_band_session))
    set_task_perms(player);
    return session:do_out_of_band_command(@args);
  endif
endif
.

@verb mcp:"package_name match_package packges" this none this
@program mcp:package_name
return this.registry:(verb)(@args);
.

@verb mcp:"handles_package wait_for_package" this none this
@program mcp:handles_package
{who, @rest} = args;
if (valid(session = this:session_for(who)))
  return session:(verb)(@rest);
endif
.

@verb mcp:"nominate_for_core" this none this rxd __WIZARD__
@program mcp:nominate_for_core
return {@pass(@args), this.package, this.session, this.registry, this.parser, $cord};
.

"***finished***

