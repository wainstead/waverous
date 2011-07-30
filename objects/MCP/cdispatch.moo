@create $root_class named "Cord Dispatcher",cdisp
@prop cdisp."root" #220 rc
@prop cdisp."next_id" 1 ""
@prop cdisp."registry" {} r
@prop cdisp."type_root" #91 r
@prop cdisp."registry_ids" {} r
;cdisp.("unique") = 1

@verb cdisp:"open" this none this
@program cdisp:open
"Usage:  :open(connection, type) => cord";
"";
"Open a cord between the calling object and something at the other end of the is_player() object given by `connection'.  Return the stub object representing the cord, or raise or return an error.";
{connection, type} = args;
if (!valid(session = $mcp:session_for(connection)))
  return E_INVARG;
elseif (!session:handles_package($mcp.cord))
  return E_INVARG;
elseif (!$object_utils:isa(type, this.type_root))
  return E_INVARG;
endif
cord = $recycler:_create(type.cord_class);
if (typeof(cord) == ERR)
  return $error:raise_or_return(cord);
endif
cord.type = type;
cord.id = tostr("R", this.next_id);
cord.our_side = caller;
"ask `connection' whether it's willing to put up with a new cord.";
if (!`session:cord_open(cord, caller) ! E_VERBNF => 1')
  $recycler:_recycle(cord);
  raise(E_PERM);
endif
cord.session = session;
cord.connection = connection;
res = $mcp.cord:send_open(session, cord.id, $mcp.cord:type_name(type), {"objnum", tostr(cord)});
if (typeof(res) == ERR)
  $recycler:_recycle(cord);
  return $error:raise_or_return(res);
endif
this.next_id = this.next_id + 1;
this.registry_ids = {tostr(cord.id), @this.registry_ids};
this.registry = {cord, @this.registry};
return cord;
.

@verb cdisp:"lookup" this none this
@program cdisp:lookup
"Usage:  :lookup(id)";
"";
{id} = args;
if (idx = tostr(id) in this.registry_ids)
  return this.registry[idx];
else
  return $nothing;
endif
.

@verb cdisp:"mcp_receive" this none this
@program cdisp:mcp_receive
"Usage:  :mcp_receive(idstr, messagestr, assocs)";
"";
idstr = args[1];
messagestr = args[2];
assocs = args[3];
(caller == $mcp.cord) || $error:raise(E_PERM);
cord = this:lookup(idstr);
"valid(cord) && cord:receive(messagestr, assocs);";
valid(cord) && cord.our_side:("cord__" + messagestr)(cord, @cord.type:parse_receive_args(messagestr, assocs));
.

@verb cdisp:"mcp_closed" this none this
@program cdisp:mcp_closed
"Usage:  :mcp_closed(idstr)";
"";
idstr = args[1];
if (caller != $mcp.cord)
  raise(E_PERM);
endif
cord = this:lookup(idstr);
valid(cord) && cord:closed();
"version: 1.0 Fox Wed Jul  5 17:58:16 1995 EDT";
.

@verb cdisp:"closed" this none this
@program cdisp:closed
"Usage:  :closed(cord)";
"";
cord = args[1];
if ((caller != cord.our_side) && (caller != cord))
  $error:raise(E_PERM);
endif
$recycler:_recycle(cord);
"version: 1.0 Fox Wed Jul  5 17:58:16 1995 EDT";
.

@verb cdisp:"cleanup" this none this rx
@program cdisp:cleanup
"Usage:  :cleanup(cord)";
"";
cord = args[1];
if ((caller != this) && (caller != cord))
  $error:raise(E_PERM);
endif
if (idx = cord in this.registry)
  this.registry = listdelete(this.registry, idx);
  this.registry_ids = listdelete(this.registry_ids, idx);
endif
if (valid(c = cord.our_side))
  c:cord_closed(cord);
endif
if (valid(c = cord.session))
  `c:cord_closed(cord, cord.our_side) ! ANY';
endif
"version: 1.0 Fox Wed Jul  5 17:58:16 1995 EDT";
.

@verb cdisp:"connection" this none this
@program cdisp:connection
"Usage:  :connection(cord)";
"";
cord = args[1];
if (caller != cord.our_side)
  return $error:raise_or_return(E_PERM);
endif
return cord.connection;
.

@verb cdisp:"module_finish_unpack" this none this
@program cdisp:module_finish_unpack
"Usage:  :module_finish_unpack()";
"";
if (!$perm_utils:controls(caller_perms(), this))
  raise(E_PERM);
endif
try
  add_property(#0, "cord", this, {this.owner, "r"});
except (E_INVARG)
  `$cord = this ! ANY';
endtry
.

@verb cdisp:"init_for_module" this none this
@program cdisp:init_for_module
"Usage:  :init_for_module()";
"";
if ((caller != this) && (!$perm_utils:controls(caller_perms(), this)))
  raise(E_PERMS);
endif
this.registry = {};
this.next_id = 1;
this.registry_ids = {};
.

@verb cdisp:"nominate_for_core" this none this
@program cdisp:nominate_for_core
return {@pass(@args), this.root, this.type_root};
.

@verb cdisp:"init_for_core" this none this
@program cdisp:init_for_core
if ($__core_init_phase)
  pass(@args);
  this:init_for_module();
endif
.

"***finished***

