#!/usr/bin/perl

while ($input = <DATA>) {

    chomp $input;
    ($file, $lineno, $string) = split /:\s*/, $input;
    print "# $file has $string\n";
    $newstring = $string;
    $newstring =~ s/new/_new/g;
    print "perl -pi -e 's/\\Q$string\\E/$newstring/' $file\n";
}

__END__
my-stdio.h:63:#  define rename(old, new)	(link(old, new) && unlink(old))
db_objects.c:80:	    new[i] = objects[i];
db_objects.c:82:	objects = new;
db_objects.c:184:    Objid new;
db_objects.c:189:    for (new = 0; new < old; new++) {
db_objects.c:190:	if (objects[new] == 0) {
db_objects.c:192:	    o = objects[new] = objects[old];
db_objects.c:194:	    objects[new]->id = new;
db_objects.c:211:		    objects[oid]->parent = new;
db_objects.c:229:		    objects[oid]->location = new;
db_objects.c:233:	    if (is_user(new)) {
db_objects.c:238:			all_users.v.list[i].v.obj = new;
db_objects.c:255:		    if (o->owner == new)
db_objects.c:258:			o->owner = new;
db_objects.c:261:			if (v->owner == new)
db_objects.c:264:			    v->owner = new;
db_objects.c:269:			if (p[i].owner == new)
db_objects.c:272:			    p[i].owner = new;
db_objects.c:276:	    return new;
db_properties.c:174:		h = db_find_property(oid, new, 0);
db_properties.c:176:		|| property_defined_at_or_below(new, str_hash(new), oid))
db_properties.c:180:	    props->l[i].name = str_ref(new);
db_properties.c:181:	    props->l[i].hash = str_hash(new);
db_properties.c:591:fix_props(Objid oid, int parent_local, int old, int new, int common)
db_properties.c:605:    if (local + new + common != 0) {
db_properties.c:609:	for (i = 0; i < new; i++) {
db_properties.c:628:	fix_props(c, local, old, new, common);
disassemble.c:423:	    new[i] = d->lines[i];
disassemble.c:426:	d->lines = new;
gnu-malloc.c:657:	if ((new = malloc(n)) == 0)
gnu-malloc.c:659:	bcopy(mem, new, tocopy);
gnu-malloc.c:661:	return new;
list.c:41:    Var new;
list.c:56:    new.type = TYPE_LIST;
list.c:58:    new.v.list[0].type = TYPE_INT;
list.c:59:    new.v.list[0].v.num = size;
list.c:60:    return new;
list.c:109:    Var new;
list.c:121:	new.v.list[i] = var_ref(list.v.list[i]);
list.c:122:    new.v.list[pos] = value;
list.c:124:	new.v.list[i + 1] = var_ref(list.v.list[i]);
list.c:126:    return new;
list.c:148:    Var new;
list.c:153:	new.v.list[i] = var_ref(list.v.list[i]);
list.c:156:	new.v.list[i - 1] = var_ref(list.v.list[i]);
list.c:158:    return new;
list.c:166:    Var new;
list.c:171:	new.v.list[i] = var_ref(first.v.list[i]);
list.c:173:	new.v.list[i + lfirst] = var_ref(second.v.list[i]);
list.c:177:    return new;
net_multi.c:131:		new[i] = reg_fds[i];
net_multi.c:133:		new[i].fd = -1;
net_multi.c:138:	reg_fds = new;
parse_cmd.c:55:		new[i] = words[i];
parse_cmd.c:58:	    words = new;
ref_count.c:164:	    ll_insert_entry(&new[index], link);
ref_count.c:167:    return new;
str_intern.c:40:    new = mymalloc(sizeof(struct intern_entry_hunk), M_INTERN_HUNK);
str_intern.c:41:    new->size = size;
str_intern.c:42:    new->handout = 0;
str_intern.c:44:    new->next = NULL;
str_intern.c:46:    return new;
sym_table.c:48:    new->size = old->size;
sym_table.c:49:    for (i = 0; i < new->size; i++)
sym_table.c:50:	new->names[i] = str_ref(old->names[i]);
sym_table.c:52:    return new;
sym_table.c:129:	new->size = old_max;
