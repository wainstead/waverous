#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

extern char *optarg;
extern int optind;
extern int optopt;   // when an invalid arg is found, it's put in this global variable
extern int opterr;   


void display_usage() { printf("usage: [-b --buffy] [-f --fluoride <val>] [-w --wheezy <val>]\n"); }

int main(int argc, char** argv)
{
    int bflag, ch;

    /* options descriptor */
    static struct option longopts[] = {
        { "buffy"    ,  no_argument,            NULL,          'b' }, // -b
        //{ "callous"  ,  required_argument,      NULL,          'c' }, // -c ARG, no long version
        { "fluoride" ,  required_argument,      NULL,          'f' }, // -f ARG --fluoride=ARG
        { "wheezy"   ,  required_argument,      NULL,          'w' }, // -w ARG --wheezy=ARG
        { "xyzzy"    ,  optional_argument,      NULL,          'x' }, // --xyzzy
        { "help"     ,  no_argument,            NULL,          'h' },
        { NULL       ,  no_argument,            NULL,           0  }  // required by getopt_long()
    };

    for (int acounter = 0; acounter < argc; acounter++) {
        printf("argv[%d]: %s\n", acounter, argv[acounter]);
    }

    if (argc == 1) {
        // we didn't get any arguments at all
        printf("no arguments given\n");
        display_usage();
        exit(EXIT_FAILURE);
    }

    bflag = 0;
    int counter = 0;

    while ((ch = getopt_long(argc, argv, "bf:w:x:h", longopts, NULL)) != -1) {
        printf("\ttesting  '%c'\n", ch       );
        printf("\toptind:   %d\n" , optind   );
        printf("\toptopt:   %d\n" , optopt   );
        printf("\topterr:   %d\n" , opterr   );

        switch (ch) {

        case 'b':
            bflag = 1;
            printf("i matched the b flag, set bflag=%d\n", bflag);
            break;

        case 'f':
            printf("this is the eff: %s\n", optarg);
            break;

        case 'w':
            printf("wheezy: %s\n", optarg);
            break;

        case 'x':
            printf("xyzzy: %s\n", optarg);
            break;

        case 'h':
            display_usage();
            break;

        case '?':
            // when getopt_long finds an argument it doesn't know, it
            // prints "illegal option" to stderr and returns '?'.
            printf("invalid argument: %c\n", optopt);
            display_usage();
            exit(EXIT_FAILURE);

        case ':':
            // when getopt_long doesn't find a required argument, it
            // returns ':'.
            printf("Too few arguments\n");
            display_usage();
            exit(EXIT_FAILURE);

        case 0:
            // getopt_long() set a variable, just keep going; this is
            // the case for something like:
            // { "blippy", required_argument, &myvar, 42},
            // and now myvar == 42.
            break;

        default:
            break;
        }

        if (counter++ > argc) {
            printf("error: exceeded argc, exiting...\n");
            exit(EXIT_FAILURE);
        }

    }
    printf("End of line.\n");
    return 0;
}


/*

Some of the constants should be moved out into configure options;
others on the command line. Well, for that matter, all constants above
the "do not edit below this line" should be either command line flags,
configure options, or both. Most appear to be configure options.

--log-commands Keep a log of every command entered by players since the last checkpoing
#define LOG_COMMANDS
./configure:
--with-command-logging
--without-command-logging

--unforked-checkpoints Don't fork() to write out the database.
#define UNFORKED_CHECKPOINTS
./configure:
--with-forked-checkpoints (default)
--without-unforked-checkpoints

--out-of-band-prefix='something'
#define OUT_OF_BAND_PREFIX "#$#"
./configure:
--with-oob-prefix='prefix'
--without-oob-prefix

--out-of-band-prefix-quote='something'
#define OUT_OF_BAND_QUOTE_PREFIX "#$\""
./configure:
--oob-prefix-quote='something' (ignored if OUT_OF_BAND_QUOTE_PREFIX is unset)

--default-max-stack-depth=N default maximum depth allowed for the MOO verb-call stack
#define DEFAULT_MAX_STACK_DEPTH	50
./configure:
--with-max-stack-depth=N

here's what the -O and +O flags mean...
/* disable by default, +O enables: 
/* #define OUTBOUND_NETWORK 0 

/* enable by default, -O disables: 
#define OUTBOUND_NETWORK 1
--enable-outbound-network
--disable-outbound-network
./configure:
--with-outbound-network
--without-outbound-network
x

 */
