#!/usr/bin/perl
# memread.pl <pid> <hex addr>: print the 32 bit value (decimal, unsigned)
open(M, "</proc/$ARGV[0]/mem") or die; sysseek(M, hex($ARGV[1]), 0); sysread(M, my $b, 4); print unpack("V", $b), "\n";
