# my-stdio.h has #  define rename(old, new)	(link(old, new) && unlink(old))
perl -pi -e 's/\Q#  define rename(old, new)	(link(old, new) && unlink(old))\E/#  define rename(old, _new)	(link(old, _new) && unlink(old))/' my-stdio.h
# db_objects.c has new[i] = objects[i];
perl -pi -e 's/\Qnew[i] = objects[i];\E/_new[i] = objects[i];/' db_objects.c
# db_objects.c has objects = new;
perl -pi -e 's/\Qobjects = new;\E/objects = _new;/' db_objects.c
# db_objects.c has Objid new;
perl -pi -e 's/\QObjid new;\E/Objid _new;/' db_objects.c
# db_objects.c has for (new = 0; new < old; new++) {
perl -pi -e 's/\Qfor (new = 0; new < old; new++) {\E/for (_new = 0; _new < old; _new++) {/' db_objects.c
# db_objects.c has if (objects[new] == 0) {
perl -pi -e 's/\Qif (objects[new] == 0) {\E/if (objects[_new] == 0) {/' db_objects.c
# db_objects.c has o = objects[new] = objects[old];
perl -pi -e 's/\Qo = objects[new] = objects[old];\E/o = objects[_new] = objects[old];/' db_objects.c
# db_objects.c has objects[new]->id = new;
perl -pi -e 's/\Qobjects[new]->id = new;\E/objects[_new]->id = _new;/' db_objects.c
# db_objects.c has objects[oid]->parent = new;
perl -pi -e 's/\Qobjects[oid]->parent = new;\E/objects[oid]->parent = _new;/' db_objects.c
# db_objects.c has objects[oid]->location = new;
perl -pi -e 's/\Qobjects[oid]->location = new;\E/objects[oid]->location = _new;/' db_objects.c
# db_objects.c has if (is_user(new)) {
perl -pi -e 's/\Qif (is_user(new)) {\E/if (is_user(_new)) {/' db_objects.c
# db_objects.c has all_users.v.list[i].v.obj = new;
perl -pi -e 's/\Qall_users.v.list[i].v.obj = new;\E/all_users.v.list[i].v.obj = _new;/' db_objects.c
# db_objects.c has if (o->owner == new)
perl -pi -e 's/\Qif (o->owner == new)\E/if (o->owner == _new)/' db_objects.c
# db_objects.c has o->owner = new;
perl -pi -e 's/\Qo->owner = new;\E/o->owner = _new;/' db_objects.c
# db_objects.c has if (v->owner == new)
perl -pi -e 's/\Qif (v->owner == new)\E/if (v->owner == _new)/' db_objects.c
# db_objects.c has v->owner = new;
perl -pi -e 's/\Qv->owner = new;\E/v->owner = _new;/' db_objects.c
# db_objects.c has if (p[i].owner == new)
perl -pi -e 's/\Qif (p[i].owner == new)\E/if (p[i].owner == _new)/' db_objects.c
# db_objects.c has p[i].owner = new;
perl -pi -e 's/\Qp[i].owner = new;\E/p[i].owner = _new;/' db_objects.c
# db_objects.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' db_objects.c
# db_properties.c has h = db_find_property(oid, new, 0);
perl -pi -e 's/\Qh = db_find_property(oid, new, 0);\E/h = db_find_property(oid, _new, 0);/' db_properties.c
# db_properties.c has || property_defined_at_or_below(new, str_hash(new), oid))
perl -pi -e 's/\Q|| property_defined_at_or_below(new, str_hash(new), oid))\E/|| property_defined_at_or_below(_new, str_hash(_new), oid))/' db_properties.c
# db_properties.c has props->l[i].name = str_ref(new);
perl -pi -e 's/\Qprops->l[i].name = str_ref(new);\E/props->l[i].name = str_ref(_new);/' db_properties.c
# db_properties.c has props->l[i].hash = str_hash(new);
perl -pi -e 's/\Qprops->l[i].hash = str_hash(new);\E/props->l[i].hash = str_hash(_new);/' db_properties.c
# db_properties.c has fix_props(Objid oid, int parent_local, int old, int new, int common)
perl -pi -e 's/\Qfix_props(Objid oid, int parent_local, int old, int new, int common)\E/fix_props(Objid oid, int parent_local, int old, int _new, int common)/' db_properties.c
# db_properties.c has if (local + new + common != 0) {
perl -pi -e 's/\Qif (local + new + common != 0) {\E/if (local + _new + common != 0) {/' db_properties.c
# db_properties.c has for (i = 0; i < new; i++) {
perl -pi -e 's/\Qfor (i = 0; i < new; i++) {\E/for (i = 0; i < _new; i++) {/' db_properties.c
# db_properties.c has fix_props(c, local, old, new, common);
perl -pi -e 's/\Qfix_props(c, local, old, new, common);\E/fix_props(c, local, old, _new, common);/' db_properties.c
# disassemble.c has new[i] = d->lines[i];
perl -pi -e 's/\Qnew[i] = d->lines[i];\E/_new[i] = d->lines[i];/' disassemble.c
# disassemble.c has d->lines = new;
perl -pi -e 's/\Qd->lines = new;\E/d->lines = _new;/' disassemble.c
# gnu-malloc.c has if ((new = malloc(n)) == 0)
perl -pi -e 's/\Qif ((new = malloc(n)) == 0)\E/if ((_new = malloc(n)) == 0)/' gnu-malloc.c
# gnu-malloc.c has bcopy(mem, new, tocopy);
perl -pi -e 's/\Qbcopy(mem, new, tocopy);\E/bcopy(mem, _new, tocopy);/' gnu-malloc.c
# gnu-malloc.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' gnu-malloc.c
# list.c has Var new;
perl -pi -e 's/\QVar new;\E/Var _new;/' list.c
# list.c has new.type = TYPE_LIST;
perl -pi -e 's/\Qnew.type = TYPE_LIST;\E/_new.type = TYPE_LIST;/' list.c
# list.c has new.v.list[0].type = TYPE_INT;
perl -pi -e 's/\Qnew.v.list[0].type = TYPE_INT;\E/_new.v.list[0].type = TYPE_INT;/' list.c
# list.c has new.v.list[0].v.num = size;
perl -pi -e 's/\Qnew.v.list[0].v.num = size;\E/_new.v.list[0].v.num = size;/' list.c
# list.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' list.c
# list.c has Var new;
perl -pi -e 's/\QVar new;\E/Var _new;/' list.c
# list.c has new.v.list[i] = var_ref(list.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i] = var_ref(list.v.list[i]);\E/_new.v.list[i] = var_ref(list.v.list[i]);/' list.c
# list.c has new.v.list[pos] = value;
perl -pi -e 's/\Qnew.v.list[pos] = value;\E/_new.v.list[pos] = value;/' list.c
# list.c has new.v.list[i + 1] = var_ref(list.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i + 1] = var_ref(list.v.list[i]);\E/_new.v.list[i + 1] = var_ref(list.v.list[i]);/' list.c
# list.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' list.c
# list.c has Var new;
perl -pi -e 's/\QVar new;\E/Var _new;/' list.c
# list.c has new.v.list[i] = var_ref(list.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i] = var_ref(list.v.list[i]);\E/_new.v.list[i] = var_ref(list.v.list[i]);/' list.c
# list.c has new.v.list[i - 1] = var_ref(list.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i - 1] = var_ref(list.v.list[i]);\E/_new.v.list[i - 1] = var_ref(list.v.list[i]);/' list.c
# list.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' list.c
# list.c has Var new;
perl -pi -e 's/\QVar new;\E/Var _new;/' list.c
# list.c has new.v.list[i] = var_ref(first.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i] = var_ref(first.v.list[i]);\E/_new.v.list[i] = var_ref(first.v.list[i]);/' list.c
# list.c has new.v.list[i + lfirst] = var_ref(second.v.list[i]);
perl -pi -e 's/\Qnew.v.list[i + lfirst] = var_ref(second.v.list[i]);\E/_new.v.list[i + lfirst] = var_ref(second.v.list[i]);/' list.c
# list.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' list.c
# net_multi.c has new[i] = reg_fds[i];
perl -pi -e 's/\Qnew[i] = reg_fds[i];\E/_new[i] = reg_fds[i];/' net_multi.c
# net_multi.c has new[i].fd = -1;
perl -pi -e 's/\Qnew[i].fd = -1;\E/_new[i].fd = -1;/' net_multi.c
# net_multi.c has reg_fds = new;
perl -pi -e 's/\Qreg_fds = new;\E/reg_fds = _new;/' net_multi.c
# parse_cmd.c has new[i] = words[i];
perl -pi -e 's/\Qnew[i] = words[i];\E/_new[i] = words[i];/' parse_cmd.c
# parse_cmd.c has words = new;
perl -pi -e 's/\Qwords = new;\E/words = _new;/' parse_cmd.c
# ref_count.c has ll_insert_entry(&new[index], link);
perl -pi -e 's/\Qll_insert_entry(&new[index], link);\E/ll_insert_entry(&_new[index], link);/' ref_count.c
# ref_count.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' ref_count.c
# str_intern.c has new = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);
perl -pi -e 's/\Qnew = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);\E/_new = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);/' str_intern.c
# str_intern.c has new->size = size;
perl -pi -e 's/\Qnew->size = size;\E/_new->size = size;/' str_intern.c
# str_intern.c has new->handout = 0;
perl -pi -e 's/\Qnew->handout = 0;\E/_new->handout = 0;/' str_intern.c
# str_intern.c has new->next = NULL;
perl -pi -e 's/\Qnew->next = NULL;\E/_new->next = NULL;/' str_intern.c
# str_intern.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' str_intern.c
# sym_table.c has new->size = old->size;
perl -pi -e 's/\Qnew->size = old->size;\E/_new->size = old->size;/' sym_table.c
# sym_table.c has for (i = 0; i < new->size; i++)
perl -pi -e 's/\Qfor (i = 0; i < new->size; i++)\E/for (i = 0; i < _new->size; i++)/' sym_table.c
# sym_table.c has new->names[i] = str_ref(old->names[i]);
perl -pi -e 's/\Qnew->names[i] = str_ref(old->names[i]);\E/_new->names[i] = str_ref(old->names[i]);/' sym_table.c
# sym_table.c has return new;
perl -pi -e 's/\Qreturn new;\E/return _new;/' sym_table.c
# sym_table.c has new->size = old_max;
perl -pi -e 's/\Qnew->size = old_max;\E/_new->size = old_max;/' sym_table.c
