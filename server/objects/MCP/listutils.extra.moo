" JHCore uses a pair of new verbs on $list_utils.  You can use these
" definitions to upgrade your own $list_utils, but it's just as
" correct to place these versb on $mcp (say) and make all callers
" refer to $mcp:<verb> instead of $list_utils:<verb>

@verb list_utils:"passoc" this none this
@program list_utils:passoc
"passoc(key,list1,list2)";
"passoc() behaves rather similarly to assoc, with the exception that it's intended for";
"parallel lists.  given a key from list1, it returns a list containing the key and the";
"corresponding item from list2 (\"corresponding\", in the case of parallel lists, means";
"having the same index.)";
indx = args[1] in args[2];
if (indx)
  return {args[1], args[3][indx]};
else
  return {};
endif
.

@verb list_utils:"max_length" this none this rx
@program list_utils:max_length
":max_length(strings-or-lists[, default])";
"Return the maximum length of a set of strings or lists.";
"default is the minimum length that can be returned; 0 is a safe bet.";
max = args[2] || 0;
for item in (args[1])
  max = max(max, length(item));
endfor
return max;
.

@verb list_utils:"make_alist" this none this rxd
@program list_utils:make_alist
":make_alist(lists[, pad])";
"Make an alist out of n parallel lists (basically a matrix transpose).";
"If the lists are of uneven length, fill the remaining tuples with pad (defaults to 0).";
alist = {};
pad = (length(args) > 1) ? args[2] | 0;
for i in [1..$list_utils:max_length(args[1])]
  tuple = {};
  for l in (args[1])
    tuple = {@tuple, (i > length(l)) ? pad | l[i]};
  endfor
  alist = {@alist, tuple};
endfor
return alist;
.
