# line:   state->fixups = mymalloc(sizeof(Fixup) * state->max_fixups, M_CODE_GEN);
# match:  Fixup
perl -pi -e 's|\Qstate->fixups = mymalloc(sizeof(Fixup) * state->max_fixups, M_CODE_GEN);\E|state->fixups = (Fixup *) mymalloc(sizeof(Fixup) * state->max_fixups, M_CODE_GEN);|' code_gen.c
# line:   state->bytes = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
# match:  Byte
perl -pi -e 's|\Qstate->bytes = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);\E|state->bytes = (Byte *) mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);|' code_gen.c
# line:   state->pushmap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
# match:  Byte
perl -pi -e 's|\Qstate->pushmap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);\E|state->pushmap = (Byte *) mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);|' code_gen.c
# line:   state->trymap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);
# match:  Byte
perl -pi -e 's|\Qstate->trymap = mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);\E|state->trymap = (Byte *) mymalloc(sizeof(Byte) * state->max_bytes, M_BYTECODES);|' code_gen.c
# line:   state->loops = mymalloc(sizeof(Loop) * state->max_loops, M_CODE_GEN);
# match:  Loop
perl -pi -e 's|\Qstate->loops = mymalloc(sizeof(Loop) * state->max_loops, M_CODE_GEN);\E|state->loops = (Loop *) mymalloc(sizeof(Loop) * state->max_loops, M_CODE_GEN);|' code_gen.c
# line:   Fixup *new_fixups = mymalloc(sizeof(Fixup) * new_max,
# match:  Fixup
perl -pi -e 's|\QFixup *new_fixups = mymalloc(sizeof(Fixup) * new_max,\E|Fixup *new_fixups = (Fixup *) mymalloc(sizeof(Fixup) * new_max,|' code_gen.c
# line:   Var *new_literals = mymalloc(sizeof(Var) * new_max,
# match:  Var
perl -pi -e 's|\QVar *new_literals = mymalloc(sizeof(Var) * new_max,\E|Var *new_literals = (Var *) mymalloc(sizeof(Var) * new_max,|' code_gen.c
# line:   Bytecodes *new_fv = mymalloc(sizeof(Bytecodes) * new_max,
# match:  Bytecodes
perl -pi -e 's|\QBytecodes *new_fv = mymalloc(sizeof(Bytecodes) * new_max,\E|Bytecodes *new_fv = (Bytecodes *) mymalloc(sizeof(Bytecodes) * new_max,|' code_gen.c
# line:   Loop *new_loops = mymalloc(sizeof(Loop) * new_max,
# match:  Loop
perl -pi -e 's|\QLoop *new_loops = mymalloc(sizeof(Loop) * new_max,\E|Loop *new_loops = (Loop *) mymalloc(sizeof(Loop) * new_max,|' code_gen.c
# line:   bc.vector = mymalloc(sizeof(Byte) * bc.size, M_BYTECODES);
# match:  Byte
perl -pi -e 's|\Qbc.vector = mymalloc(sizeof(Byte) * bc.size, M_BYTECODES);\E|bc.vector = (Byte *) mymalloc(sizeof(Byte) * bc.size, M_BYTECODES);|' code_gen.c
# line:   bbd = mymalloc(sizeof(*bbd) * (state.num_fixups + 2), M_CODE_GEN);
# match:  
# line:   prog->literals = mymalloc(sizeof(Var) * gstate.num_literals,
# match:  Var
perl -pi -e 's|\Qprog->literals = mymalloc(sizeof(Var) * gstate.num_literals,\E|prog->literals = (Var *) mymalloc(sizeof(Var) * gstate.num_literals,|' code_gen.c
# line:   v = mymalloc(sizeof(Verbdef), M_VERBDEF);
# match:  Verbdef
perl -pi -e 's|\Qv = mymalloc(sizeof(Verbdef), M_VERBDEF);\E|v = (Verbdef *) mymalloc(sizeof(Verbdef), M_VERBDEF);|' db_file.c
# line:   o->propdefs.l = mymalloc(i * sizeof(Propdef), M_PROPDEF);
# match:  Propdef
perl -pi -e 's|\Qo->propdefs.l = mymalloc(i * sizeof(Propdef), M_PROPDEF);\E|o->propdefs.l = (Propdef *) mymalloc(i * sizeof(Propdef), M_PROPDEF);|' db_file.c
# line:   o->propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
# match:  Pval
perl -pi -e 's|\Qo->propval = mymalloc(nprops * sizeof(Pval), M_PVAL);\E|o->propval = (Pval *) mymalloc(nprops * sizeof(Pval), M_PVAL);|' db_file.c
# line:   objects = mymalloc(max_objects * sizeof(Object *), M_OBJECT_TABLE);
# match:  
# line:   new = mymalloc(max_objects * 2 * sizeof(Object *), M_OBJECT_TABLE);
# match:  
# line:   o = objects[num_objects] = mymalloc(sizeof(Object), M_OBJECT);
# match:  Object
perl -pi -e 's|\Qo = objects[num_objects] = mymalloc(sizeof(Object), M_OBJECT);\E|o = objects[num_objects] = (Object *) mymalloc(sizeof(Object), M_OBJECT);|' db_objects.c
# line:   new_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
# match:  Pval
perl -pi -e 's|\Qnew_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);\E|new_propval = (Pval *) mymalloc(nprops * sizeof(Pval), M_PVAL);|' db_properties.c
# line:   o->propdefs.l = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);
# match:  Propdef
perl -pi -e 's|\Qo->propdefs.l = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);\E|o->propdefs.l = (Propdef *) mymalloc(new_size * sizeof(Propdef), M_PROPDEF);|' db_properties.c
# line:   new_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);
# match:  Pval
perl -pi -e 's|\Qnew_propval = mymalloc(nprops * sizeof(Pval), M_PVAL);\E|new_propval = (Pval *) mymalloc(nprops * sizeof(Pval), M_PVAL);|' db_properties.c
# line:   new_props = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);
# match:  Propdef
perl -pi -e 's|\Qnew_props = mymalloc(new_size * sizeof(Propdef), M_PROPDEF);\E|new_props = (Propdef *) mymalloc(new_size * sizeof(Propdef), M_PROPDEF);|' db_properties.c
# line:   new_propval = mymalloc((local + new + common) * sizeof(Pval), M_PVAL);
# match:  Pval
perl -pi -e 's|\Qnew_propval = mymalloc((local + new + common) * sizeof(Pval), M_PVAL);\E|new_propval = (Pval *) mymalloc((local + new + common) * sizeof(Pval), M_PVAL);|' db_properties.c
# line:   current_alias = mymalloc(sizeof(struct pt_entry), M_PREP);
# match:  
# line:   newv = mymalloc(sizeof(Verbdef), M_VERBDEF);
# match:  Verbdef
perl -pi -e 's|\Qnewv = mymalloc(sizeof(Verbdef), M_VERBDEF);\E|newv = (Verbdef *) mymalloc(sizeof(Verbdef), M_VERBDEF);|' db_verbs.c
# line:   vc_table = mymalloc(size * sizeof(vc_entry *), M_VC_TABLE);
# match:  
# line:   new_vc = mymalloc(sizeof(vc_entry), M_VC_ENTRY);
# match:  vc_entry
perl -pi -e 's|\Qnew_vc = mymalloc(sizeof(vc_entry), M_VC_ENTRY);\E|new_vc = (vc_entry *) mymalloc(sizeof(vc_entry), M_VC_ENTRY);|' db_verbs.c
# line:   expr_stack = mymalloc(sum * sizeof(Expr *), M_DECOMPILE);
# match:  
# line:   char **new = mymalloc(sizeof(char **) * new_max, M_DISASSEMBLE);
# match:  
# line:   ret = mymalloc(MAX(size, NUM_READY_VARS) * sizeof(Var), M_RT_ENV);
# match:  Var
perl -pi -e 's|\Qret = mymalloc(MAX(size, NUM_READY_VARS) * sizeof(Var), M_RT_ENV);\E|ret = (Var *) mymalloc(MAX(size, NUM_READY_VARS) * sizeof(Var), M_RT_ENV);|' eval_env.c
# line:   vm the_vm = mymalloc(sizeof(vmstruct), M_VM);
# match:  vmstruct
perl -pi -e 's|\Qvm the_vm = mymalloc(sizeof(vmstruct), M_VM);\E|vm the_vm = (vmstruct *) mymalloc(sizeof(vmstruct), M_VM);|' eval_vm.c
# line:   the_vm->activ_stack = mymalloc(sizeof(activation) * stack_size, M_VM);
# match:  activation
perl -pi -e 's|\Qthe_vm->activ_stack = mymalloc(sizeof(activation) * stack_size, M_VM);\E|the_vm->activ_stack = (activation *) mymalloc(sizeof(activation) * stack_size, M_VM);|' eval_vm.c
# line:   res = mymalloc(MAX(size, RT_STACK_QUICKSIZE) * sizeof(Var), M_RT_STACK);
# match:  Var
perl -pi -e 's|\Qres = mymalloc(MAX(size, RT_STACK_QUICKSIZE) * sizeof(Var), M_RT_STACK);\E|res = (Var *) mymalloc(MAX(size, RT_STACK_QUICKSIZE) * sizeof(Var), M_RT_STACK);|' execute.c
# line:   activ_stack = mymalloc(sizeof(activation) * max, M_VM);
# match:  activation
perl -pi -e 's|\Qactiv_stack = mymalloc(sizeof(activation) * max, M_VM);\E|activ_stack = (activation *) mymalloc(sizeof(activation) * max, M_VM);|' execute.c
# line:   stdin_waiter *w = mymalloc(sizeof(stdin_waiter), M_TASK);
# match:  stdin_waiter
perl -pi -e 's|\Qstdin_waiter *w = mymalloc(sizeof(stdin_waiter), M_TASK);\E|stdin_waiter *w = (stdin_waiter *) mymalloc(sizeof(stdin_waiter), M_TASK);|' extensions.c
# line:   emptylist.v.list = mymalloc(1 * sizeof(Var), M_LIST);
# match:  Var
perl -pi -e 's|\Qemptylist.v.list = mymalloc(1 * sizeof(Var), M_LIST);\E|emptylist.v.list = (Var *) mymalloc(1 * sizeof(Var), M_LIST);|' list.c
# line:   s = mymalloc(sizeof(char) * (newsize + 1), M_STRING);
# match:  char
perl -pi -e 's|\Qs = mymalloc(sizeof(char) * (newsize + 1), M_STRING);\E|s = (char *) mymalloc(sizeof(char) * (newsize + 1), M_STRING);|' list.c
# line:   l = mymalloc(sizeof(listener), M_NETWORK);
# match:  listener
perl -pi -e 's|\Ql = mymalloc(sizeof(listener), M_NETWORK);\E|l = (listener *) mymalloc(sizeof(listener), M_NETWORK);|' net_bsd_lcl.c
# line:   Port *new_ports = mymalloc(new_max * sizeof(Port), M_NETWORK);
# match:  Port
perl -pi -e 's|\QPort *new_ports = mymalloc(new_max * sizeof(Port), M_NETWORK);\E|Port *new_ports = (Port *) mymalloc(new_max * sizeof(Port), M_NETWORK);|' net_mp_fake.c
# line:   Port *new_ports = mymalloc(new_num * sizeof(Port), M_NETWORK);
# match:  Port
perl -pi -e 's|\QPort *new_ports = mymalloc(new_num * sizeof(Port), M_NETWORK);\E|Port *new_ports = (Port *) mymalloc(new_num * sizeof(Port), M_NETWORK);|' net_mp_poll.c
# line:   reg_fds = mymalloc(max_reg_fds * sizeof(fd_reg), M_NETWORK);
# match:  fd_reg
perl -pi -e 's|\Qreg_fds = mymalloc(max_reg_fds * sizeof(fd_reg), M_NETWORK);\E|reg_fds = (fd_reg *) mymalloc(max_reg_fds * sizeof(fd_reg), M_NETWORK);|' net_multi.c
# line:   fd_reg *new = mymalloc(new_max * sizeof(fd_reg), M_NETWORK);
# match:  fd_reg
perl -pi -e 's|\Qfd_reg *new = mymalloc(new_max * sizeof(fd_reg), M_NETWORK);\E|fd_reg *new = (fd_reg *) mymalloc(new_max * sizeof(fd_reg), M_NETWORK);|' net_multi.c
# line:   h = mymalloc(sizeof(nhandle), M_NETWORK);
# match:  nhandle
perl -pi -e 's|\Qh = mymalloc(sizeof(nhandle), M_NETWORK);\E|h = (nhandle *) mymalloc(sizeof(nhandle), M_NETWORK);|' net_multi.c
# line:   nl->ptr = l = mymalloc(sizeof(nlistener), M_NETWORK);
# match:  nlistener
perl -pi -e 's|\Qnl->ptr = l = mymalloc(sizeof(nlistener), M_NETWORK);\E|nl->ptr = l = (nlistener *) mymalloc(sizeof(nlistener), M_NETWORK);|' net_multi.c
# line:   l = mymalloc(sizeof(listener), M_NETWORK);
# match:  listener
perl -pi -e 's|\Ql = mymalloc(sizeof(listener), M_NETWORK);\E|l = (listener *) mymalloc(sizeof(listener), M_NETWORK);|' net_sysv_lcl.c
# line:   v.v.fnum = mymalloc(sizeof(double), M_FLOAT);
# match:  double
perl -pi -e 's|\Qv.v.fnum = mymalloc(sizeof(double), M_FLOAT);\E|v.v.fnum = (double *) mymalloc(sizeof(double), M_FLOAT);|' numbers.c
# line:   words = mymalloc(max_words * sizeof(char *), M_STRING_PTRS);
# match:  
# line:   char **new = mymalloc(new_max * sizeof(char *), M_STRING_PTRS);
# match:  
# line:   regexp_t buf = mymalloc(sizeof(*buf), M_PATTERN);
# match:  
# line:   buf->fastmap = mymalloc(256 * sizeof(char), M_PATTERN);
# match:  char
perl -pi -e 's|\Qbuf->fastmap = mymalloc(256 * sizeof(char), M_PATTERN);\E|buf->fastmap = (char *) mymalloc(256 * sizeof(char), M_PATTERN);|' pattern.c
# line:   ref_table = mymalloc(table_size * sizeof(reftab_entry *), M_REF_TABLE);
# match:  
# line:   ans = mymalloc(sizeof(reftab_entry), M_REF_ENTRY);
# match:  reftab_entry
perl -pi -e 's|\Qans = mymalloc(sizeof(reftab_entry), M_REF_ENTRY);\E|ans = (reftab_entry *) mymalloc(sizeof(reftab_entry), M_REF_ENTRY);|' ref_count.c
# line:   new_table = mymalloc(table_size * sizeof(reftab_entry *), M_REF_TABLE);
# match:  
# line:   slistener *l = mymalloc(sizeof(slistener), M_NETWORK);
# match:  slistener
perl -pi -e 's|\Qslistener *l = mymalloc(sizeof(slistener), M_NETWORK);\E|slistener *l = (slistener *) mymalloc(sizeof(slistener), M_NETWORK);|' server.c
# line:   new = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);
# match:  
# line:   new->contents = mymalloc(sizeof(struct intern_entry) * size, M_INTERN_ENTRY);
# match:  
# line:   table = mymalloc(sizeof(struct intern_entry *) * size, M_INTERN_POINTER);
# match:  
# line:   /* p = mymalloc(sizeof(struct intern_entry), M_INTERN_ENTRY); */
# match:  
# line:   Stream *s = mymalloc(sizeof(Stream), M_STREAM);
# match:  Stream
perl -pi -e 's|\QStream *s = mymalloc(sizeof(Stream), M_STREAM);\E|Stream *s = (Stream *) mymalloc(sizeof(Stream), M_STREAM);|' streams.c
# line:   Names *names = mymalloc(sizeof(Names), M_NAMES);
# match:  Names
perl -pi -e 's|\QNames *names = mymalloc(sizeof(Names), M_NAMES);\E|Names *names = (Names *) mymalloc(sizeof(Names), M_NAMES);|' sym_table.c
# line:   names->names = mymalloc(sizeof(char *) * max_size, M_NAMES);
# match:  
# line:   tq = mymalloc(sizeof(tqueue), M_TASK);
# match:  tqueue
perl -pi -e 's|\Qtq = mymalloc(sizeof(tqueue), M_TASK);\E|tq = (tqueue *) mymalloc(sizeof(tqueue), M_TASK);|' tasks.c
# line:   t = mymalloc(sizeof(task), M_TASK);
# match:  task
perl -pi -e 's|\Qt = mymalloc(sizeof(task), M_TASK);\E|t = (task *) mymalloc(sizeof(task), M_TASK);|' tasks.c
# line:   task *t = mymalloc(sizeof(task), M_TASK);
# match:  task
perl -pi -e 's|\Qtask *t = mymalloc(sizeof(task), M_TASK);\E|task *t = (task *) mymalloc(sizeof(task), M_TASK);|' tasks.c
# line:   ext_queue *eq = mymalloc(sizeof(ext_queue), M_TASK);
# match:  ext_queue
perl -pi -e 's|\Qext_queue *eq = mymalloc(sizeof(ext_queue), M_TASK);\E|ext_queue *eq = (ext_queue *) mymalloc(sizeof(ext_queue), M_TASK);|' tasks.c
# line:   struct loop_entry  *entry = mymalloc(sizeof(struct loop_entry), M_AST);
# match:  
# line:   struct loop_entry  *entry = mymalloc(sizeof(struct loop_entry), M_AST);
# match:  
