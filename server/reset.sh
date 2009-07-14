svn -q revert *
cp ext-FUP-1.8/ext-FUP_options.h .
cp ext-FUP-1.8/ext-FUP.c .
rm *.orig
rm *.rej
[ -f ext-xml.c        ] && rm ext-xml.c
[ -f ext-xml.patch        ] && rm ext-xml.patch
[ -f bg_name_lookup.c     ] && rm bg_name_lookup.c
[ -f bg_name_lookup.patch ] && rm bg_name_lookup.patch
[ -f BG_NAME_LOOKUP.txt   ] && rm BG_NAME_LOOKUP.txt
