@create "generic MCP package" named mcp-cord,cord
@prop cord."next_id" 1 r
@prop cord."cords" {} r
;cord.("cord_types") = {}
;cord:set_messages_in({{"open", {"_id", "_type"}}, {"", {"_id", "_message"}}, {"closed", {"_id"}}})
;cord:set_messages_out({{"open", {"_id", "_type"}}, {"", {"_id", "_message"}}, {"closed", {"_id"}}})
;cord.("unique") = 0

@verb cord:"next_id" this none this
@program cord:next_id
if (caller == this)
  return tostr("I", this.next_id = this.next_id + 1);
else
  raise(E_PERM);
endif
.

@verb cord:"cord_send" this none this rxd __WIZARD__
@program cord:cord_send
{message, alist} = args;
cord = caller;
session = cord.session;
if (cord in $cord.registry)
  return this:send_(session, tostr(cord.id), message, @alist);
else
  raise(E_PERM);
endif
.

@verb cord:"cord_closed" this none this rxd __WIZARD__
@program cord:cord_closed
cord = caller;
session = cord.session;
this:send_closed(session, tostr(cord.id));
.

@verb cord:"handle_" this none this
@program cord:handle_
{session, id, message, @assocs} = args;
if (caller == this)
  $cord:mcp_receive(id, message, assocs);
endif
.

@verb cord:"handle_closed" this none this
@program cord:handle_closed
{session, id, @rest} = args;
if (caller == this)
  $cord:mcp_closed(id);
endif
.

@verb cord:"find_type" this none this
@program cord:find_type
{name} = args;
for i in ($object_utils:leaves($cord.type_root))
  if (name == (($mcp.registry:package_name(i.parent_package) + "-") + i.cord_name))
    return i;
  endif
endfor
return $failed_Match;
.

@verb cord:"send_open" this none this
@program cord:send_open
if (caller == $cord)
  return pass(@args);
endif
.

@verb cord:"finalize_connection" this none this rxd __WIZARD__
@program cord:finalize_connection
session = caller;
len = length($cord.registry_ids);
for i in [0..len - 1]
  idx = len - i;
  cord = $cord.registry[idx];
  if (cord.session == session)
    $recycler:_recycle(cord);
  endif
endfor
.

@verb cord:"type_name" this none this
@program cord:type_name
{cord_type} = args;
parent = $mcp:package_name(cord_type.parent_package);
if (suffix = cord_type.cord_name)
  return (parent + "-") + suffix;
else
  return parent;
endif
.

"***finished***

