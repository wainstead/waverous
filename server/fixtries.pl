#!/usr/bin/perl

while ($input = <DATA>) {

    chomp $input;
    ($file, $lineno, $string) = split /:\s*/, $input;
    print "# $file has $string\n";
    $newstring = $string;
    $newstring =~ s/try/_try/g;
    print "perl -pi -e 's/\\Q$string\\E/$newstring/' $file\n";
}

__END__
decompile.c:675:			e->e._catch.try = try_expr;
unparse.c:638:	unparse_expr(str, expr->e._catch.try);
parser.y:582:		    $$->e._catch.try = $2;
