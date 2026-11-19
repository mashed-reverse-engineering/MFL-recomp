#!/usr/bin/perl
# memwait.pl <pid> <hex addr> <value|!value> <timeout s>   poll (0.1 s) until the 32 bit value equals <value> (or differs from it with '!'); logs the wait to /out/waitmem.txt
use Time::HiRes qw(time sleep);
my ($pid, $ad, $val, $to, $stall, $pad) = @ARGV;     # $pad: hex address whose changes count as progress for the stall check (default: the watched address itself)     # optional $stall: give up when the value has not changed for that many seconds (frame counters)
# adaptive deadline: the game slows under load, so timeout and stall limit are multiplied by max(1, load/4) (load = max(load1, running tasks), cap LOADCAP, default 3: tools/wine-run/loadscale.sh), re-read every ~2 s.
my $scale = 1; my $ls = 0;
sub lscale { return if time - $ls < 2; $ls = time; if (open(my $l, "</proc/loadavg")) { my ($a, undef, undef, $rt) = split ' ', <$l>; close $l; my ($run) = split '/', $rt; $a = $run if $run > $a; $scale = $a / 4; $scale = 1 if $scale < 1; my $cap = $ENV{LOADCAP} || 3; $scale = $cap if $scale > $cap; } }      # W13 C4: the rule of loadscale.sh (max(load1, running tasks) / 4, cap LOADCAP default 3)
my $neg = ($val =~ s/^!//);     # '!V' waits until the value is NOT V
my $gt = ($val =~ s/^>//);      # '>V' waits until the value is above V (HARN2 W15: FRAMED end-of-race wait, race phase > 5)
sub ok { my $v = shift; my $want = ($val =~ /^0x/) ? hex($val) : $val; return $gt ? ($v > $want) : $neg ? ($v != $want) : ($v == $want); }
open(M, "</proc/$pid/mem") or do { open(O, ">>$ENV{OUT}/waitmem.txt"); printf O "waitmem %s==%s process gone (no /proc/%s/mem)\n", $ad, $val, $pid; exit 2; };
my $t0 = time; my $v; my $lastp = 0; my ($pv, $tc) = (undef, time);
while (1) {
    lscale();
    sysseek(M, hex($ad), 0); my $n = sysread(M, my $b, 4);
    if (!defined $n || $n != 4) {                   # W12 (harness review): the process is gone; undef == 0 was true, so `waitmem ADDR 0` passed on a dead game
        open(O, ">>$ENV{OUT}/waitmem.txt"); printf O "waitmem %s==%s process gone after %.2fs\n", $ad, $val, time - $t0; exit 2;
    }
    $v = unpack("V", $b);
    last if ok($v) || time - $t0 >= $to * $scale;
    last if -e "$ENV{OUT}/crashed";                 # the game crashed (play_inner.sh watches wine.log): do not wait for the timeout
    my $pvv = $v;
    if ($pad) { sysseek(M, hex($pad), 0); my $pn = sysread(M, my $pb, 4); $pvv = (defined $pn && $pn == 4) ? unpack("V", $pb) : $pv; }
    if (!defined $pv || $pvv != $pv) { $pv = $pvv; $tc = time; }
    last if $stall && time - $tc >= $stall * $scale;
    if ($to >= 1 && time - $lastp >= 0.4) {       # live progress for live.sh / LIVE.md: "<unix time> <addr> <wanted> <current value> <timeout> <waited>"
        $lastp = time;
        if (open(P, ">$ENV{OUT}/progress.txt")) { printf P "%.1f %s %s %u %s %.1f\n", time, $ad, ($neg ? "!" : $gt ? ">" : "") . $val, $v, $to, time - $t0; close(P); }
    }
    sleep 0.1;
}
open(O, ">>$ENV{OUT}/waitmem.txt"); printf O "waitmem %s==%s waited %.2fs (value %d) load scale %.1f\n", $ad, $val, time - $t0, $v, $scale;
exit(ok($v) ? 0 : 1);
