# decompile.c has e->e._catch.try = try_expr;
perl -pi -e 's/\Qe->e._catch.try = try_expr;\E/e->e._catch._try = _try_expr;/' decompile.c
# unparse.c has unparse_expr(str, expr->e._catch.try);
perl -pi -e 's/\Qunparse_expr(str, expr->e._catch.try);\E/unparse_expr(str, expr->e._catch._try);/' unparse.c
# parser.y has $$->e._catch.try = $2;
perl -pi -e 's/\Q$$->e._catch.try = $2;\E/$$->e._catch._try = $2;/' parser.y
