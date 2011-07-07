@create "generic message dispatch object" named "generic MCP package",package
@prop package."version_range" {} r
;package.("version_range") = {"1.0", "1.0"}
@prop package."cord_types" {} r
;package.("unique") = 0

@verb package:"set_version_range" this none this rx
@program package:set_version_range
"This is the standard :set_foo verb.  It allows the property to be set if called by this or called with adequate permissions (this's owner or wizardly).";
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  return this.(verb[5..length(verb)]) = args[1];
else
  return E_PERM;
endif
"version: 1.0 Fox Wed Jul  5 17:58:13 1995 EDT";
.

@verb package:"dispatch" this none this rxd __WIZARD__
@program package:dispatch
"Usage:  :dispatch_request(who, authkey, name, arguments)";
"";
connection = caller;
{message, alist} = args;
if (verbname = this:message_name_to_verbname(message))
  set_task_perms(caller_perms());
  this:(verbname)(connection, @this:parse_receive_args(message, alist));
endif
.

@verb package:"match_request" this none this
@program package:match_request
"Usage:  :match_request(request)";
"";
request = args[1];
if ($object_utils:has_verb(this, verbname = "mcp_" + request))
  return verbname;
else
  return 0;
endif
"version: 1.0 Fox Wed Jul  5 17:58:14 1995 EDT";
.

@verb package:"initialize_connection" this none this
@program package:initialize_connection
"Usage:  :initialize_connection()";
"";
{version} = args;
connection = caller;
messages = $list_utils:slice(this.messages_in);
connection:register_handlers(messages);
.

@verb package:"message_name_to_verbname" this none this
@program package:message_name_to_verbname
{message} = args;
if ($object_utils:has_callable_verb(this, vname = "handle_" + message))
  return vname;
else
  return 0;
endif
.

@verb package:"finalize_connection" this none this
@program package:finalize_connection
connection = caller;
return 0;
.

@verb package:"add_cord_type" this none this
@program package:add_cord_type
"Usage:  :add_cord_type(cord_type)";
"";
{cord_type} = args;
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  return this.cord_types = setadd(this.cord_types, cord_type);
else
  raise(E_PERM);
endif
.

@verb package:"remove_cord_type" this none this
@program package:remove_cord_type
"Usage:  :remove_cord_type(cord_type)";
"";
{cord_type} = args;
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  return this.cord_types = setremove(this.cord_types, cord_type);
else
  raise(E_PERM);
endif
.

@verb package:"send_*" this none this
@program package:send_
{connection, @args} = args;
if (caller == this)
  message = verb[6..$];
  `connection:send(message, this:parse_send_args(message, @args)) ! E_VERBNF';
else
  raise(E_PERM);
endif
.

"***finished***

