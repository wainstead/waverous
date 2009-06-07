# tasks.h has extern enum outcome run_server_program_task(Objid this, const char *verb,
perl -pi -e 's/\Qextern enum outcome run_server_program_task(Objid this, const char *verb,\E/extern enum outcome run_server_program_task(Objid self, const char *verb,/' tasks.h
# execute.c has if (activ_stack[t].vloc != activ_stack[t].this)
perl -pi -e 's/\Qif (activ_stack[t].vloc != activ_stack[t].this)\E/if (activ_stack[t].vloc != activ_stack[t].self)/' execute.c
# execute.c has stream_printf(str, " (this == #%d)", activ_stack[t].this);
perl -pi -e 's/\Qstream_printf(str, " (this == #%d)", activ_stack[t].this);\E/stream_printf(str, " (self == #%d)", activ_stack[t].self);/' execute.c
# execute.c has v.v.list[1].v.obj = stack[i].this;
perl -pi -e 's/\Qv.v.list[1].v.obj = stack[i].this;\E/v.v.list[1].v.obj = stack[i].self;/' execute.c
# execute.c has enum error call_verb2(Objid this, const char *vname, Var args, int do_pass);
perl -pi -e 's/\Qenum error call_verb2(Objid this, const char *vname, Var args, int do_pass);\E/enum error call_verb2(Objid self, const char *vname, Var args, int do_pass);/' execute.c
# execute.c has call_verb(Objid this, const char *vname_in, Var args, int do_pass)
perl -pi -e 's/\Qcall_verb(Objid this, const char *vname_in, Var args, int do_pass)\E/call_verb(Objid self, const char *vname_in, Var args, int do_pass)/' execute.c
# execute.c has result = call_verb2(this, vname, args, do_pass);
perl -pi -e 's/\Qresult = call_verb2(this, vname, args, do_pass);\E/result = call_verb2(self, vname, args, do_pass);/' execute.c
# execute.c has call_verb2(Objid this, const char *vname, Var args, int do_pass)
perl -pi -e 's/\Qcall_verb2(Objid this, const char *vname, Var args, int do_pass)\E/call_verb2(Objid self, const char *vname, Var args, int do_pass)/' execute.c
# execute.c has where = this;
perl -pi -e 's/\Qwhere = this;\E/where = self;/' execute.c
# execute.c has RUN_ACTIV.this = this;
perl -pi -e 's/\QRUN_ACTIV.this = this;\E/RUN_ACTIV.self = self;/' execute.c
# execute.c has set_rt_env_obj(env, SLOT_THIS, this);
perl -pi -e 's/\Qset_rt_env_obj(env, SLOT_THIS, this);\E/set_rt_env_obj(env, SLOT_THIS, self);/' execute.c
# execute.c has set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);
perl -pi -e 's/\Qset_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);\E/set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.self);/' execute.c
# execute.c has return RUN_ACTIV.this;
perl -pi -e 's/\Qreturn RUN_ACTIV.this;\E/return RUN_ACTIV.self;/' execute.c
# execute.c has do_server_verb_task(Objid this, const char *verb, Var args, db_verb_handle h,
perl -pi -e 's/\Qdo_server_verb_task(Objid this, const char *verb, Var args, db_verb_handle h,\E/do_server_verb_task(Objid self, const char *verb, Var args, db_verb_handle h,/' execute.c
# execute.c has return do_server_program_task(this, verb, args, db_verb_definer(h),
perl -pi -e 's/\Qreturn do_server_program_task(this, verb, args, db_verb_definer(h),\E/return do_server_program_task(self, verb, args, db_verb_definer(h),/' execute.c
# execute.c has do_server_program_task(Objid this, const char *verb, Var args, Objid vloc,
perl -pi -e 's/\Qdo_server_program_task(Objid this, const char *verb, Var args, Objid vloc,\E/do_server_program_task(Objid self, const char *verb, Var args, Objid vloc,/' execute.c
# execute.c has RUN_ACTIV.this = this;
perl -pi -e 's/\QRUN_ACTIV.this = this;\E/RUN_ACTIV.self = self;/' execute.c
# execute.c has set_rt_env_obj(env, SLOT_THIS, this);
perl -pi -e 's/\Qset_rt_env_obj(env, SLOT_THIS, this);\E/set_rt_env_obj(env, SLOT_THIS, self);/' execute.c
# execute.c has do_input_task(Objid user, Parsed_Command * pc, Objid this, db_verb_handle vh)
perl -pi -e 's/\Qdo_input_task(Objid user, Parsed_Command * pc, Objid this, db_verb_handle vh)\E/do_input_task(Objid user, Parsed_Command * pc, Objid self, db_verb_handle vh)/' execute.c
# execute.c has RUN_ACTIV.this = this;
perl -pi -e 's/\QRUN_ACTIV.this = this;\E/RUN_ACTIV.self = self;/' execute.c
# execute.c has set_rt_env_obj(env, SLOT_THIS, this);
perl -pi -e 's/\Qset_rt_env_obj(env, SLOT_THIS, this);\E/set_rt_env_obj(env, SLOT_THIS, self);/' execute.c
# execute.c has set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);
perl -pi -e 's/\Qset_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);\E/set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.self);/' execute.c
# execute.c has RUN_ACTIV.this = NOTHING;
perl -pi -e 's/\QRUN_ACTIV.this = NOTHING;\E/RUN_ACTIV.self = NOTHING;/' execute.c
# execute.c has enum error e = call_verb2(RUN_ACTIV.this, RUN_ACTIV.verb, arglist, 1);
perl -pi -e 's/\Qenum error e = call_verb2(RUN_ACTIV.this, RUN_ACTIV.verb, arglist, 1);\E/enum error e = call_verb2(RUN_ACTIV.self, RUN_ACTIV.verb, arglist, 1);/' execute.c
# execute.c has a.this, -7, -8, a.player, -9, a.progr, a.vloc, -10, a.debug);
perl -pi -e 's/\Qa.this, -7, -8, a.player, -9, a.progr, a.vloc, -10, a.debug);\E/a.self, -7, -8, a.player, -9, a.progr, a.vloc, -10, a.debug);/' execute.c
# execute.c has &a->this, &dummy, &dummy, &a->player, &dummy, &a->progr,
perl -pi -e 's/\Q&a->this, &dummy, &dummy, &a->player, &dummy, &a->progr,\E/&a->self, &dummy, &dummy, &a->player, &dummy, &a->progr,/' execute.c
# server.c has char *this_program = str_dup(argv[0]);
perl -pi -e 's/\Qchar *this_program = str_dup(argv[0]);\E/char *self_program = str_dup(argv[0]);/' server.c
# tasks.c has Objid this;
perl -pi -e 's/\QObjid this;\E/Objid self;/' tasks.c
# tasks.c has } else if (find_verb_on(this = tq->player, pc, &vh)
perl -pi -e 's/\Q} else if (find_verb_on(this = tq->player, pc, &vh)\E/} else if (find_verb_on(self = tq->player, pc, &vh)/' tasks.c
# tasks.c has || find_verb_on(this = location, pc, &vh)
perl -pi -e 's/\Q|| find_verb_on(this = location, pc, &vh)\E/|| find_verb_on(self = location, pc, &vh)/' tasks.c
# tasks.c has || find_verb_on(this = pc->dobj, pc, &vh)
perl -pi -e 's/\Q|| find_verb_on(this = pc->dobj, pc, &vh)\E/|| find_verb_on(self = pc->dobj, pc, &vh)/' tasks.c
# tasks.c has || find_verb_on(this = pc->iobj, pc, &vh)
perl -pi -e 's/\Q|| find_verb_on(this = pc->iobj, pc, &vh)\E/|| find_verb_on(self = pc->iobj, pc, &vh)/' tasks.c
# tasks.c has || (valid(this = location)
perl -pi -e 's/\Q|| (valid(this = location)\E/|| (valid(self = location)/' tasks.c
# tasks.c has do_input_task(tq->player, pc, this, vh);
perl -pi -e 's/\Qdo_input_task(tq->player, pc, this, vh);\E/do_input_task(tq->player, pc, self, vh);/' tasks.c
# tasks.c has run_server_program_task(Objid this, const char *verb, Var args, Objid vloc,
perl -pi -e 's/\Qrun_server_program_task(Objid this, const char *verb, Var args, Objid vloc,\E/run_server_program_task(Objid self, const char *verb, Var args, Objid vloc,/' tasks.c
# tasks.c has return do_server_program_task(this, verb, args, vloc, verbname, program,
perl -pi -e 's/\Qreturn do_server_program_task(this, verb, args, vloc, verbname, program,\E/return do_server_program_task(self, verb, args, vloc, verbname, program,/' tasks.c
# tasks.c has list.v.list[9].v.obj = ft.a.this;
perl -pi -e 's/\Qlist.v.list[9].v.obj = ft.a.this;\E/list.v.list[9].v.obj = ft.a.self;/' tasks.c
# tasks.c has list.v.list[9].v.obj = top_activ(the_vm).this;
perl -pi -e 's/\Qlist.v.list[9].v.obj = top_activ(the_vm).this;\E/list.v.list[9].v.obj = top_activ(the_vm).self;/' tasks.c
# timers.c has Timer_Entry *this = free_timers;
perl -pi -e 's/\QTimer_Entry *this = free_timers;\E/Timer_Entry *self = free_timers;/' timers.c
# timers.c has free_timers = this->next;
perl -pi -e 's/\Qfree_timers = this->next;\E/free_timers = self->next;/' timers.c
# timers.c has return this;
perl -pi -e 's/\Qreturn this;\E/return self;/' timers.c
# timers.c has free_timer(Timer_Entry * this)
perl -pi -e 's/\Qfree_timer(Timer_Entry * this)\E/free_timer(Timer_Entry * self)/' timers.c
# timers.c has this->next = free_timers;
perl -pi -e 's/\Qthis->next = free_timers;\E/self->next = free_timers;/' timers.c
# timers.c has free_timers = this;
perl -pi -e 's/\Qfree_timers = this;\E/free_timers = self;/' timers.c
# timers.c has Timer_Entry *this = active_timers;
perl -pi -e 's/\QTimer_Entry *this = active_timers;\E/Timer_Entry *self = active_timers;/' timers.c
# timers.c has Timer_Proc proc = this->proc;
perl -pi -e 's/\QTimer_Proc proc = this->proc;\E/Timer_Proc proc = self->proc;/' timers.c
# timers.c has Timer_ID id = this->id;
perl -pi -e 's/\QTimer_ID id = this->id;\E/Timer_ID id = self->id;/' timers.c
# timers.c has Timer_Data data = this->data;
perl -pi -e 's/\QTimer_Data data = this->data;\E/Timer_Data data = self->data;/' timers.c
# timers.c has free_timer(this);
perl -pi -e 's/\Qfree_timer(this);\E/free_timer(self);/' timers.c
# timers.c has Timer_Entry *this = virtual_timer;
perl -pi -e 's/\QTimer_Entry *this = virtual_timer;\E/Timer_Entry *self = virtual_timer;/' timers.c
# timers.c has Timer_Proc proc = this->proc;
perl -pi -e 's/\QTimer_Proc proc = this->proc;\E/Timer_Proc proc = self->proc;/' timers.c
# timers.c has Timer_ID id = this->id;
perl -pi -e 's/\QTimer_ID id = this->id;\E/Timer_ID id = self->id;/' timers.c
# timers.c has Timer_Data data = this->data;
perl -pi -e 's/\QTimer_Data data = this->data;\E/Timer_Data data = self->data;/' timers.c
# timers.c has free_timer(this);
perl -pi -e 's/\Qfree_timer(this);\E/free_timer(self);/' timers.c
# timers.c has Timer_Entry *this = allocate_timer();
perl -pi -e 's/\QTimer_Entry *this = allocate_timer();\E/Timer_Entry *self = allocate_timer();/' timers.c
# timers.c has this->id = next_id++;
perl -pi -e 's/\Qthis->id = next_id++;\E/self->id = next_id++;/' timers.c
# timers.c has this->when = time(0) + seconds;
perl -pi -e 's/\Qthis->when = time(0) + seconds;\E/self->when = time(0) + seconds;/' timers.c
# timers.c has this->proc = proc;
perl -pi -e 's/\Qthis->proc = proc;\E/self->proc = proc;/' timers.c
# timers.c has this->data = data;
perl -pi -e 's/\Qthis->data = data;\E/self->data = data;/' timers.c
# timers.c has while (*t && this->when >= (*t)->when)
perl -pi -e 's/\Qwhile (*t && this->when >= (*t)->when)\E/while (*t && self->when >= (*t)->when)/' timers.c
# timers.c has this->next = *t;
perl -pi -e 's/\Qthis->next = *t;\E/self->next = *t;/' timers.c
# timers.c has *t = this;
perl -pi -e 's/\Q*t = this;\E/*t = self;/' timers.c
# timers.c has return this->id;
perl -pi -e 's/\Qreturn this->id;\E/return self->id;/' timers.c
# parser.y has Cond_Arm *this_arm = alloc_cond_arm($4, $6);
perl -pi -e 's/\QCond_Arm *this_arm = alloc_cond_arm($4, $6);\E/Cond_Arm *self_arm = alloc_cond_arm($4, $6);/' parser.y
# parser.y has tmp->next = this_arm;
perl -pi -e 's/\Qtmp->next = this_arm;\E/tmp->next = self_arm;/' parser.y
# parser.y has $$ = this_arm;
perl -pi -e 's/\Q$$ = this_arm;\E/$$ = self_arm;/' parser.y
# parser.y has Arg_List *this_arg = alloc_arg_list(ARG_NORMAL, $3);
perl -pi -e 's/\QArg_List *this_arg = alloc_arg_list(ARG_NORMAL, $3);\E/Arg_List *self_arg = alloc_arg_list(ARG_NORMAL, $3);/' parser.y
# parser.y has tmp->next = this_arg;
perl -pi -e 's/\Qtmp->next = this_arg;\E/tmp->next = self_arg;/' parser.y
# parser.y has $$ = this_arg;
perl -pi -e 's/\Q$$ = this_arg;\E/$$ = self_arg;/' parser.y
# parser.y has Arg_List *this_arg = alloc_arg_list(ARG_SPLICE, $4);
perl -pi -e 's/\QArg_List *this_arg = alloc_arg_list(ARG_SPLICE, $4);\E/Arg_List *self_arg = alloc_arg_list(ARG_SPLICE, $4);/' parser.y
# parser.y has tmp->next = this_arg;
perl -pi -e 's/\Qtmp->next = this_arg;\E/tmp->next = self_arg;/' parser.y
# parser.y has $$ = this_arg;
perl -pi -e 's/\Q$$ = this_arg;\E/$$ = self_arg;/' parser.y
