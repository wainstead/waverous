@create $root_class named "generic MCP 2.1 session",session
@prop session."connection" #-1 r
@prop session."pending_multilines" {} ""
@prop session."packages" {} r
@prop session."authentication_key" E_NONE ""
@prop session."phase" 0 r
@prop session."package_waiters" {} ""
@prop session."message_handlers" {} r
;session.("message_handlers") = {{}, {}}
;session.("unique") = 0
;session.("handlers") = {{}, {}}

@verb session:"set_connection" this none this rx
@program session:set_connection
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  this.connection = args[1];
  this:set_name("session for " + tostr(this.connection));
  return 1;
else
  return E_PERM;
endif
.

@verb session:"multiline_begin" this none this
@program session:multiline_begin
{request, authkey, data_tag, alist} = args;
if (caller != $mcp.parser)
  raise(E_PERM);
elseif ($list_utils:assoc(data_tag, this.pending_multilines))
  "it's not valid to begin two requests with the same data tag, drop it";
  return;
endif
this.pending_multilines = {@this.pending_multilines, {data_tag, authkey, request, alist}};
.

@verb session:"multiline_finish" this none this
@program session:multiline_finish
{who, data_tag} = args;
if ((caller != this) && (caller != $mcp.parser))
  raise(E_PERM);
elseif (!(n = $list_utils:iassoc(data_tag, this.pending_multilines)))
  "drop it";
  return;
else
  {data_tag, authkey, request, alist} = this.pending_multilines[n];
  this.pending_multilines = listdelete(this.pending_multilines, n);
  return {request, authkey, alist};
endif
.

@verb session:"multiline_add_value" this none this
@program session:multiline_add_value
{data_tag, keyword, value} = args;
if (caller != $mcp.parser)
  raise(E_PERM);
elseif (!(n = $list_utils:iassoc(data_tag, this.pending_multilines)))
  "drop it";
  return;
elseif (!(nkey = $list_utils:iassoc(keyword, this.pending_multilines[n][4])))
  "drop it";
  return;
elseif (typeof(this.pending_multilines[n][4][nkey][2]) != LIST)
  "not a multiline, drop it.";
  return;
else
  this.pending_multilines[n][4][nkey][2] = {@this.pending_multilines[n][4][nkey][2], value};
endif
.

@verb session:"do_out_of_band_command" this none this rxd __WIZARD__
@program session:do_out_of_band_command
if (caller != $mcp)
  raise(E_PERM);
else
  set_task_perms(caller_perms());
  if (message = $mcp.parser:parse(argstr, @args))
    if (`player.MCP_snoop ! ANY')
      player:tell("C->S: ", argstr);
    endif
    this:dispatch(@message);
  endif
endif
.

@verb session:"finish" this none this
@program session:finish
if (caller == $mcp)
  this:_signal_package_waiter(E_INVARG);
  for package in ($list_utils:slice(this.packages))
    fork (0)
      package:finalize_connection();
    endfork
  endfor
  return $mcp:finalize_connection(this);
endif
.

@verb session:"initialize_connection" this none this
@program session:initialize_connection
if (caller != $mcp)
  raise(E_PERM);
else
  this:send("mcp", {{"version", "2.1"}, {"to", "2.1"}});
endif
.

@verb session:"set_packages set_authentication_key set_phase" this none this
@program session:set_packages
"This is the standard :set_foo verb.  It allows the property to be set if called by this or called with adequate permissions (this's owner or wizardly).";
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  return this.(verb[5..length(verb)]) = args[1];
else
  return E_PERM;
endif
.

@verb session:"add_package" this none this
@program session:add_package
{package, version} = args;
if (caller in {$mcp.negotiate, this})
  if (n = $list_utils:iassoc(package, this.packages))
    packages = this.packages;
    packages[n][2] = version;
    this:set_packages(packages);
  else
    this:set_packages({@this.packages, {package, version}});
  endif
  package:initialize_connection(version);
  this:_signal_package_waiter(package, version);
endif
.

@verb session:"handles_package" this none this
@program session:handles_package
{package} = args;
if (assoc = $list_utils:assoc(package, this.packages))
  return assoc[2];
endif
.

@verb session:"register_handlers" this none this
@program session:register_handlers
{messages} = args;
package = caller;
{plist, mlist} = this.message_handlers;
prefix = this:package_name(package);
for message in (messages)
  message = this:message_fullname(prefix, message);
  if (idx = message in mlist)
    if (plist[idx] != package)
      raise(E_INVARG);
    endif
  else
    plist = {@plist, package};
    mlist = {@mlist, message};
  endif
endfor
this.message_handlers = {plist, mlist};
.

@verb session:"dispatch" this none this rxd __WIZARD__
@program session:dispatch
{message, authkey, alist} = args;
if (caller == this)
  if ((!this.phase) && (message == "mcp"))
    authkey = $list_utils:assoc("authentication-key", alist);
    minv = $list_utils:assoc("version", alist);
    maxv = $list_utils:Assoc("to", alist);
    if (((authkey && minv) && maxv) && $mcp:compare_version_range({minv[2], maxv[2]}, {$mcp.version, $mcp.version}))
      this:set_authentication_key(authkey[2]);
      this:add_package($mcp.negotiate, $mcp.negotiate.version_range[1]);
    else
      "woop woop break somehow";
      return;
    endif
    this:set_phase(1);
    $mcp.negotiate:do_negotiation();
  elseif (this.phase)
    if ((this.authentication_key != E_NONE) && (authkey != this.authentication_key))
      return;
    endif
    package = this:find_handler(message);
    if (typeof(package) == OBJ)
      set_task_perms(caller_perms());
      package:dispatch(this:strip_prefix(this:package_name(package), message), alist);
    endif
    "figure out which package to dispatch to";
    "do dispatch";
  endif
endif
.

@verb session:"send" this none this rxd __WIZARD__
@program session:send
{message, alist} = args;
who = caller_perms();
if (caller == this)
  prefix = "";
elseif ($list_utils:assoc(caller, this.packages))
  package = caller;
  message = this:message_fullname(this:package_name(package), message);
else
  raise(E_PERM);
endif
con = this.connection;
snoop = `this.connection.MCP_snoop ! E_PROPNF => 0';
for line in ($mcp.parser:unparse(message, this.authentication_key, alist))
  notify(con, line);
  if (snoop)
    notify(con, "S->C: " + line);
  endif
endfor
.

@verb session:"find_handler" this none this
@program session:find_handler
{message} = args;
if (assoc = $list_utils:passoc(message, this.message_handlers[2], this.message_handlers[1]))
  return assoc[2];
endif
.

@verb session:"connection" this none this
@program session:connection
return this.connection;
.

@verb session:"package_name" this none this
@program session:package_name
{package} = args;
return $mcp.registry:package_name(package);
.

@verb session:"message_fullname" this none this
@program session:message_fullname
{prefix, message} = args;
if (message)
  message = (prefix + "-") + message;
else
  message = prefix;
endif
return message;
.

@verb session:"strip_prefix" this none this
@program session:strip_prefix
{prefix, message} = args;
if (index(message, prefix + "-") == 1)
  return message[length(prefix) + 2..$];
elseif (index(message, prefix) == 1)
  return message[length(prefix) + 1..$];
elseif (message == prefix)
  return "";
else
  return message;
endif
.

@verb session:"end_negotiation" this none this
@program session:end_negotiation
if (caller == $mcp.negotiate)
  this:_signal_package_waiter(0);
endif
.

@verb session:"_add_package_waiter" this none this
@program session:_add_package_waiter
{package, timeout} = args;
if (caller == this)
  this.package_waiters = {@this.package_waiters, {package, task_id()}};
  if (timeout < 0)
    r = suspend();
  else
    r = suspend(timeout);
  endif
  this.package_waiters = setremove(this.package_waiters, {package, task_id()});
  return r;
endif
.

@verb session:"_signal_package_waiter" this none this
@program session:_signal_package_waiter
{?package = $nothing, value} = args;
if (caller == this)
  all = package == $nothing;
  for keyval in (this.package_waiters)
    {pkg, tid} = keyval;
    if (all || (pkg == package))
      `resume(tid, value) ! ANY';
    endif
  endfor
endif
.

@verb session:"wait_for_package" this none this
@program session:wait_for_package
{package, ?timeout} = args;
timeout = `timeout ! E_VARNF => -1';
if (v = this:handles_package(package))
  return v;
else
  return this:_add_package_waiter(package, timeout);
endif
.

"***finished***

