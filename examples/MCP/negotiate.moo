@create "generic MCP package" named "mcp-negotiate",negotiate
;negotiate:set_version_range({"1.0", "2.0"})
;negotiate:set_messages_in({{"can", {"package", "min-version", "max-version"}}, {"end", {}}})
;negotiate:set_messages_out({{"can", {"package", "min-version", "max-version"}}, {"end", {}}})
;negotiate.("unique") = 0

@verb negotiate:"do_negotiation" this none this
@program negotiate:do_negotiation
connection = caller;
for keyval in ($mcp.registry:packages())
  {name, package} = keyval;
  this:send_can(connection, name, @package.version_range);
endfor
this:send_end(connection);
.

@verb negotiate:"handle_can" this none this
@program negotiate:handle_can
if (caller == this)
  {connection, package, minv, maxv, @rest} = args;
  if (valid(pkg = $mcp.registry:match_package(package)))
    if (version = $mcp:compare_version_range({minv, maxv}, pkg.version_range))
      connection:add_package(pkg, version);
    endif
  endif
endif
.

@verb negotiate:"handle_end" this none this
@program negotiate:handle_end
if (caller == this)
  {connection, @rest} = args;
  connection:end_negotiation();
endif
.

"***finished***

