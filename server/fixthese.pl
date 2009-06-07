#!/usr/bin/perl

while ($input = <DATA>) {
    next if $input =~ /_this/;
    chomp $input;
    ($file, $lineno, $string) = split /:\s*/, $input;
    print "# $file has $string\n";
    $newstring = $string;
    $newstring =~ s/this/self/g;
    print "perl -pi -e 's/\\Q$string\\E/$newstring/' $file\n";
}

__END__
execute.h:102:extern enum outcome do_server_program_task(Objid _this, const char *verb,
tasks.h:91:extern enum outcome run_server_program_task(Objid this, const char *verb,
execute.c:129:	if (activ_stack[t].vloc != activ_stack[t].this)
execute.c:130:	    stream_printf(str, " (this == #%d)", activ_stack[t].this);
execute.c:404:	    v.v.list[1].v.obj = stack[i].this;
execute.c:537:enum error call_verb2(Objid this, const char *vname, Var args, int do_pass);
execute.c:544:call_verb(Objid this, const char *vname_in, Var args, int do_pass)
execute.c:549:    result = call_verb2(this, vname, args, do_pass);
execute.c:556:call_verb2(Objid this, const char *vname, Var args, int do_pass)
execute.c:580:	where = this;
execute.c:592:    RUN_ACTIV.this = this;
execute.c:609:    set_rt_env_obj(env, SLOT_THIS, this);
execute.c:610:    set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);
execute.c:2162:    return RUN_ACTIV.this;
execute.c:2246:do_server_verb_task(Objid this, const char *verb, Var args, db_verb_handle h,
execute.c:2250:    return do_server_program_task(this, verb, args, db_verb_definer(h),
execute.c:2258:do_server_program_task(Objid this, const char *verb, Var args, Objid vloc,
execute.c:2269:    RUN_ACTIV.this = this;
execute.c:2279:    set_rt_env_obj(env, SLOT_THIS, this);
execute.c:2293:do_input_task(Objid user, Parsed_Command * pc, Objid this, db_verb_handle vh)
execute.c:2302:    RUN_ACTIV.this = this;
execute.c:2312:    set_rt_env_obj(env, SLOT_THIS, this);
execute.c:2351:    set_rt_env_obj(env, SLOT_CALLER, CALLER_ACTIV.this);
execute.c:2362:    RUN_ACTIV.this = NOTHING;
execute.c:2547:    enum error e = call_verb2(RUN_ACTIV.this, RUN_ACTIV.verb, arglist, 1);
execute.c:2651:	    a.this, -7, -8, a.player, -9, a.progr, a.vloc, -10, a.debug);
execute.c:2675:		 &a->this, &dummy, &dummy, &a->player, &dummy, &a->progr,
server.c:1186:    char *this_program = str_dup(argv[0]);
tasks.c:689:	    Objid this;
tasks.c:704:	    } else if (find_verb_on(this = tq->player, pc, &vh)
tasks.c:705:		       || find_verb_on(this = location, pc, &vh)
tasks.c:706:		       || find_verb_on(this = pc->dobj, pc, &vh)
tasks.c:707:		       || find_verb_on(this = pc->iobj, pc, &vh)
tasks.c:708:		       || (valid(this = location)
tasks.c:711:		do_input_task(tq->player, pc, this, vh);
tasks.c:1345:run_server_program_task(Objid this, const char *verb, Var args, Objid vloc,
tasks.c:1351:    return do_server_program_task(this, verb, args, vloc, verbname, program,
tasks.c:1709:    list.v.list[9].v.obj = ft.a.this;
tasks.c:1751:    list.v.list[9].v.obj = top_activ(the_vm).this;
timers.c:50:	Timer_Entry *this = free_timers;
timers.c:52:	free_timers = this->next;
timers.c:53:	return this;
timers.c:59:free_timer(Timer_Entry * this)
timers.c:61:    this->next = free_timers;
timers.c:62:    free_timers = this;
timers.c:70:    Timer_Entry *this = active_timers;
timers.c:71:    Timer_Proc proc = this->proc;
timers.c:72:    Timer_ID id = this->id;
timers.c:73:    Timer_Data data = this->data;
timers.c:76:    free_timer(this);
timers.c:87:    Timer_Entry *this = virtual_timer;
timers.c:88:    Timer_Proc proc = this->proc;
timers.c:89:    Timer_ID id = this->id;
timers.c:90:    Timer_Data data = this->data;
timers.c:93:    free_timer(this);
timers.c:160:    Timer_Entry *this = allocate_timer();
timers.c:163:    this->id = next_id++;
timers.c:164:    this->when = time(0) + seconds;
timers.c:165:    this->proc = proc;
timers.c:166:    this->data = data;
timers.c:171:    while (*t && this->when >= (*t)->when)
timers.c:173:    this->next = *t;
timers.c:174:    *t = this;
timers.c:178:    return this->id;
parser.y:276:		    Cond_Arm *this_arm = alloc_cond_arm($4, $6);
parser.y:283:			tmp->next = this_arm;
parser.y:286:			$$ = this_arm;
parser.y:621:		    Arg_List *this_arg = alloc_arg_list(ARG_NORMAL, $3);
parser.y:628:			tmp->next = this_arg;
parser.y:631:			$$ = this_arg;
parser.y:635:		    Arg_List *this_arg = alloc_arg_list(ARG_SPLICE, $4);
parser.y:642:			tmp->next = this_arg;
parser.y:645:			$$ = this_arg;
