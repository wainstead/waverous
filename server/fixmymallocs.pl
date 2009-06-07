#!/usr/bin/perl

while ($input = <DATA>) {
    chomp $input;
    ($file, $line) = split(/:\s+/, $input);
    $line =~ m/sizeof\(([a-zA-Z0-9_]+)\)/;
    print "# line:   $line\n";
    print "# match:  $1\n";
    if ($1) {
        $match = $1;
        $newline = $line;
        $newline =~ s/mymalloc/($match *) mymalloc/;
        printf 'perl -pi -e \'s|\Q%s\E|%s|\' %s', $line,  $newline, $file;
        print "\n";
    }
}

__END__
code_gen.c:    state->fixups = mymalloc(sizeof(Fixup) * state->max_fixups, M_CODE_GEN);
code_gen.c:    state->bytes = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
code_gen.c:    state->pushmap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
code_gen.c:    state->trymap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
code_gen.c:    state->loops = mymalloc(sizeof(Loop) * state->max_loops, M_CODE_GEN);
code_gen.c:	Fixup *new_fixups = mymalloc(sizeof(Fixup) * new_max,
code_gen.c:	    Var *new_literals = mymalloc(sizeof(Var) * new_max,
code_gen.c:	Bytecodes *new_fv = mymalloc(sizeof(Bytecodes) * new_max,
code_gen.c:	Loop *new_loops = mymalloc(sizeof(Loop) * new_max,
code_gen.c:    bc.vector = mymalloc(sizeof(Byte) * bc.size, M_BYTECODES);
code_gen.c:    bbd = mymalloc(sizeof(*bbd) * (state.num_fixups + 2), M_CODE_GEN);
code_gen.c:	prog->literals = mymalloc(sizeof(Var) * gstate.num_literals,
db_file.c:	v = mymalloc(sizeof(Verbdef), M_VERBDEF);
db_file.c:	o->propdefs.l = mymalloc(i * sizeof(Propdef), M_PROPDEF);
db_file.c:	o->propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
db_objects.c:	objects = mymalloc(max_objects * sizeof(Object *), M_OBJECT_TABLE);
db_objects.c:	new = mymalloc(max_objects * 2 * sizeof(Object *), M_OBJECT_TABLE);
db_objects.c:    o = objects[num_objects] = mymalloc(sizeof(Object), M_OBJECT);
db_properties.c:    new_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
db_properties.c:	o->propdefs.l = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);
db_properties.c:	new_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
db_properties.c:		new_props = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);
db_properties.c:	new_propval = mymalloc((local + new + common) * sizeof(Pval), M_PVAL);
db_verbs.c:	    current_alias = mymalloc(sizeof(struct pt_entry), M_PREP);
db_verbs.c:    newv = mymalloc(sizeof(Verbdef), M_VERBDEF);
db_verbs.c:    vc_table = mymalloc(size * sizeof(vc_entry *), M_VC_TABLE);
db_verbs.c:    new_vc = mymalloc(sizeof(vc_entry), M_VC_ENTRY);
decompile.c:    expr_stack = mymalloc(sum * sizeof(Expr *), M_DECOMPILE);
disassemble.c:	char **new = mymalloc(sizeof(char **) * new_max, M_DISASSEMBLE);
eval_env.c:	ret = mymalloc(MAX(size, NUM_READY_VARS) * sizeof(Var), M_RT_ENV);
eval_vm.c:    vm the_vm = mymalloc(sizeof(vmstruct), M_VM);
eval_vm.c:    the_vm->activ_stack = mymalloc(sizeof(activation) * stack_size, M_VM);
execute.c:	res = mymalloc(MAX(size, RT_STACK_QUICKSIZE) * sizeof(Var), M_RT_STACK);
execute.c:	activ_stack = mymalloc(sizeof(activation) * max, M_VM);
extensions.c:    stdin_waiter *w = mymalloc(sizeof(stdin_waiter), M_TASK);
list.c:	    emptylist.v.list = mymalloc(1 * sizeof(Var), M_LIST);
list.c:    s = mymalloc(sizeof(char) * (newsize + 1), M_STRING);
net_bsd_lcl.c:    l = mymalloc(sizeof(listener), M_NETWORK);
net_mp_fake.c:	Port *new_ports = mymalloc(new_max * sizeof(Port), M_NETWORK);
net_mp_poll.c:	Port *new_ports = mymalloc(new_num * sizeof(Port), M_NETWORK);
net_multi.c:	reg_fds = mymalloc(max_reg_fds * sizeof(fd_reg), M_NETWORK);
net_multi.c:	fd_reg *new = mymalloc(new_max * sizeof(fd_reg), M_NETWORK);
net_multi.c:    h = mymalloc(sizeof(nhandle), M_NETWORK);
net_multi.c:	nl->ptr = l = mymalloc(sizeof(nlistener), M_NETWORK);
net_sysv_lcl.c:    l = mymalloc(sizeof(listener), M_NETWORK);
numbers.c:    v.v.fnum = mymalloc(sizeof(double), M_FLOAT);
parse_cmd.c:	words = mymalloc(max_words * sizeof(char *), M_STRING_PTRS);
parse_cmd.c:	    char **new = mymalloc(new_max * sizeof(char *), M_STRING_PTRS);
pattern.c:    regexp_t buf = mymalloc(sizeof(*buf), M_PATTERN);
pattern.c:	buf->fastmap = mymalloc(256 * sizeof(char), M_PATTERN);
ref_count.c:    ref_table = mymalloc(table_size * sizeof(reftab_entry *), M_REF_TABLE);
ref_count.c:	ans = mymalloc(sizeof(reftab_entry), M_REF_ENTRY);
ref_count.c:	new_table = mymalloc(table_size * sizeof(reftab_entry *), M_REF_TABLE);
server.c:    slistener *l = mymalloc(sizeof(slistener), M_NETWORK);
str_intern.c:    new = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);
str_intern.c:    new->contents = mymalloc(sizeof(struct intern_entry) * size, M_INTERN_ENTRY);
str_intern.c:    table = mymalloc(sizeof(struct intern_entry *) * size, M_INTERN_POINTER);
str_intern.c:    /* p = mymalloc(sizeof(struct intern_entry), M_INTERN_ENTRY); */
streams.c:    Stream *s = mymalloc(sizeof(Stream), M_STREAM);
sym_table.c:    Names *names = mymalloc(sizeof(Names), M_NAMES);
sym_table.c:    names->names = mymalloc(sizeof(char *) * max_size, M_NAMES);
tasks.c:    tq = mymalloc(sizeof(tqueue), M_TASK);
tasks.c:	t = mymalloc(sizeof(task), M_TASK);
tasks.c:    task *t = mymalloc(sizeof(task), M_TASK);
tasks.c:    ext_queue *eq = mymalloc(sizeof(ext_queue), M_TASK);
parser.y:    struct loop_entry  *entry = mymalloc(sizeof(struct loop_entry), M_AST);
parser.y:    struct loop_entry  *entry = mymalloc(sizeof(struct loop_entry), M_AST);
