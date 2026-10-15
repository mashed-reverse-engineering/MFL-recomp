#!/usr/bin/perl
# memwait.pl <pid> <hex addr> <value|!value> <timeout s>   poll (0.1 s) until the 32 bit value equals <value> (or differs from it with '!'); logs the wait to /out/waitmem.txt
use Time::HiRes qw(time sleep);
my ($pid, $ad, $val, $to, $stall, $pad) = @ARGV;     # $pad: hex address whose changes count as progress for the stall check (default: the watched address itself)     # optional $stall: give up when the value has not changed for that many seconds (frame counters)
# adaptive deadline: the game slows under load, so timeout and stall limit are multiplied by max(1, load1/4) (cap LOADCAP, default 4), re-read every ~2 s.
my $scale = 1; my $ls = 0;
sub lscale { return if time - $ls < 2; $ls = time; if (open(my $l, "</proc/loadavg")) { my ($a) = split ' ', <$l>; close $l; $scale = $a / 4; $scale = 1 if $scale < 1; my $cap = $ENV{LOADCAP} || 4; $scale = $cap if $scale > $cap; } }
my $neg = ($val =~ s/^!//);     # '!V' waits until the value is NOT V
sub ok { my $v = shift; my $want = ($val =~ /^0x/) ? hex($val) : $val; return $neg ? ($v != $want) : ($v == $want); }
open(M, "</proc/$pid/mem") or die;
my $t0 = time; my $v; my $lastp = 0; my ($pv, $tc) = (undef, time);
while (1) {
    lscale();
    sysseek(M, hex($ad), 0); sysread(M, my $b, 4); $v = unpack("V", $b);
    last if ok($v) || time - $t0 >= $to * $scale;
    last if -e "$ENV{OUT}/crashed";                 # the game crashed (play_inner.sh watches wine.log): do not wait for the timeout
    my $pvv = $v;
    if ($pad) { sysseek(M, hex($pad), 0); sysread(M, my $pb, 4); $pvv = unpack("V", $pb); }
    if (!defined $pv || $pvv != $pv) { $pv = $pvv; $tc = time; }
    last if $stall && time - $tc >= $stall * $scale;
    if ($to >= 1 && time - $lastp >= 0.4) {       # live progress for live.sh / LIVE.md: "<unix time> <addr> <wanted> <current value> <timeout> <waited>"
        $lastp = time;
        if (open(P, ">$ENV{OUT}/progress.txt")) { printf P "%.1f %s %s %u %s %.1f\n", time, $ad, ($neg ? "!" : "") . $val, $v, $to, time - $t0; close(P); }
    }
    sleep 0.1;
}
open(O, ">>$ENV{OUT}/waitmem.txt"); printf O "waitmem %s==%s waited %.2fs (value %d) load scale %.1f\n", $ad, $val, time - $t0, $v, $scale;
exit(ok($v) ? 0 : 1);
