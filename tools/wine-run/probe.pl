#!/usr/bin/perl
# probe.pl <pid> <seconds> <hex addr>...   every 0.25 s append "elapsed v1 v2 .." (32 bit values of the live process) to /out/probe.txt
use Time::HiRes qw(time sleep);
my ($pid, $n, @a) = @ARGV;
open(M, "</proc/$pid/mem") or die; open(O, ">>$ENV{OUT}/probe.txt"); select((select(O), $| = 1)[0]);
my $t0 = time;
while (time - $t0 < $n) {
    my @v = map { sysseek(M, hex($_), 0); sysread(M, my $b, 4); unpack("V", $b) } @a;
    printf O "%.2f %s\n", time - $t0, join(" ", @v);
    sleep 0.25;
}
