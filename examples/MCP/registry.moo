@create $root_class named "MCP package registry",registry
@prop registry."package_names" {} r
@prop registry."packages" {} r
@prop registry."core_package_names" {} r
;registry.("core_package_names") = {"mcp-negotiate", "mcp-cord" }
;registry.("key") = 0
;registry.("unique") = 0

@verb registry:"add_package" this none this
@program registry:add_package
{name, package} = args;
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  if (name in this.package_names)
    raise(E_INVARG, "Another package with that name already exists");
  elseif (package in this.packages)
    raise(E_INVARG, "That package already is registered under a different name.");
  else
    this.package_names = {@this.package_names, name};
    this.packages = {@this.packages, package};
  endif
endif
.

@verb registry:"remove_package" this none this
@program registry:remove_package
{name} = args;
if ((caller == this) || $perm_utils:controls(caller_perms(), this))
  if (idx = name in this.package_names)
    this.package_names = listdelete(this.package_names, idx);
    this.packages = listdelete(this.packages, idx);
  else
    raise(E_INVARG, "Not a defined package");
  endif
endif
.

@verb registry:"match_package" this none this
@program registry:match_package
{name} = args;
if (idx = name in this.package_names)
  return this.packages[idx];
else
  return $failed_match;
endif
.

@verb registry:"package_name" this none this
@program registry:package_name
{package} = args;
if (idx = package in this.packages)
  return this.package_names[idx];
else
  return "";
endif
.

@verb registry:"packages" this none this
@program registry:packages
return $list_utils:make_alist({this.package_names, this.packages});
.

@verb registry:"init_for_module init_for_core" this none this
@program registry:init_for_module
"Usage:  :init_for_module()";
if (((!$__core_init_phase) && (caller != this)) && (!$perm_utils:controls(caller_perms(), this)))
  raise(E_PERM);
endif
for name in (this.package_names)
  if (!(name in this.core_package_names))
    this:remove_package(name);
  endif
endfor
.

@verb registry:"nominate_for_core" this none this rxd __WIZARD__
@program registry:nominate_for_core
corenames = this.core_package_names;
corepackages = {};
for name in (corenames)
  corepackages = {@corepackages, this:match_package(name)};
endfor
return {@pass(@args), @corepackages};
.

@verb registry:"@add-package @remove-package" any to this rd __WIZARD__
@program registry:@add-package
if (!$perm_utils:controls(player, this))
  player:tell("You don't have permission to add or remove MCP 2.1 packages.");
elseif ($command_utils:object_match_failed(dobj, dobjstr))
elseif (!$object_utils:isa(dobj, $mcp.package))
  player:tell(dobj.name, " is not a valid MCP 2.1 package (descendant of ", $mcp.package, ").");
elseif (!$perm_utils:controls(player, dobj))
  player:tell("You don't control ", dobj.name, " in order to add or remove it.");
else
  name = dobj.name;
  package = dobj;
  try
    if (verb == "@add-package")
      this:add_package(name, package);
      player:tell("Added ", package.name, ".");
    else
      this:remove_package(name);
      player:tell("Removed ", package.name, ".");
    endif
  except v (ANY)
    {code, message, value, tb} = v;
    player:tell(code, ": ", message);
  endtry
endif
.

"***finished***

