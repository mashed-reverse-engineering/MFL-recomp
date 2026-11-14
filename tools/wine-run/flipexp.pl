#!/usr/bin/perl
# flipexp.pl <pid> [lifts comma list] [car index]: MashedTrainer Player.Flip() experiment on the live game (car 0 settled on the ground, opponents frozen).
# Every trial restores the WHOLE car struct (0xD04 bytes: velocity lives in several places) saved while the car stood settled upright, so trials are independent.
#  up>inv : upright pose, apply Player.Flip with lift L (negate right + up, pos.y += L, both matrix buffers)
#  inv>up : the inverted pose at height y_up + H (car resting on its roof at origin height y_up+H), Flip with lift 0 -> upright.  H sweeps the same list.
# Sample the height 2.5 s; "FELL" = ended more than 1 unit below the upright rest height (kept falling through the ground).  Result: $OUT/flipexp.txt
use Time::HiRes qw(time sleep);
my ($pid, $lifts, $car, $reps) = @ARGV;
my @lifts = split /,/, ($lifts || "0,0.1,0.2,0.3,0.4,0.5,0.6,0.8,1,1.5,2,4");
my $BASE = 0x8B06E0 + ($car || 0) * 0xD04; my $SZ = 0xD04;
open(M, "+<", "/proc/$pid/mem") or die "mem: $!";
open(O, ">>", "$ENV{OUT}/flipexp.txt") or die; select((select(O), $| = 1)[0]);
sub rd { my ($a, $n) = @_; sysseek(M, $a, 0); sysread(M, my $b, $n); $b }
sub wr { my ($a, $b) = @_; sysseek(M, $a, 0); syswrite(M, $b) }
sub fl { my ($a, $n) = @_; unpack("f<$n", rd($a, 4 * $n)) }
sub idx { unpack("V", rd($BASE + 0x9AC, 4)) }
sub cur { $BASE + 0x928 + (idx() & 1) * 0x40 }
sub height { my @p = fl(cur() + 0x30, 3); $p[1] }
sub upy { my @u = fl(cur() + 0x10, 3); $u[1] }
sub flip {   # exactly Player.Flip()
    my $lift = shift; my $c = cur();
    my @r = fl($c, 3); my @u = fl($c + 0x10, 3); my @p = fl($c + 0x30, 3);
    @r = map { -$_ } @r; @u = map { -$_ } @u; $p[1] += $lift;
    for my $i (0, 1) { my $m = $BASE + 0x928 + $i * 0x40; wr($m, pack("f<3", @r)); wr($m + 0x10, pack("f<3", @u)); wr($m + 0x30, pack("f<3", @p)) }
}
sub setheight { my $y = shift; for my $i (0, 1) { my $m = $BASE + 0x928 + $i * 0x40; my @p = fl($m + 0x30, 3); wr($m + 0x30, pack("f<3", $p[0], $y, $p[2])) } }
sub settle { my $t0 = time; my $y0 = height(); my $t1 = $t0;
    while (time - $t0 < 20) { sleep 0.1; my $y = height(); if (abs($y - $y0) > 0.002) { $y0 = $y; $t1 = time } elsif (time - $t1 > 1) { last } } height() }
my ($S, $yup);
sub trial {
    my ($name, $v, $start) = @_;     # $start: coderef run right after the restore
    wr($BASE, $S); sleep 0.15; $start->();
    my ($ymin, $t0, $next, @tr) = (1e9, time, 0.5);
    while ((my $t = time - $t0) < 2.5) { my $y = height(); $ymin = $y if $y < $ymin; if ($t >= $next) { push @tr, sprintf("%.2f", $y); $next += 0.5 } sleep 0.02 }
    my $e = height();
    printf O "%-7s %-4s min %7.2f end %7.2f up.y %+.2f  %s  trace %s\n", $name, $v, $ymin, $e, upy(), ($e < $yup - 1 ? "FELL" : "ok"), join(" ", @tr);
}
printf O "car %d base %x\n", $car || 0, $BASE;
$yup = settle(); $S = rd($BASE, $SZ);
printf O "upright rest y %.3f up.y %+.2f\n", $yup, upy();
print O "-- up>inv: Player.Flip(lift) from the upright rest pose\n";
for my $rep (1 .. ($reps || 1)) { for my $L (@lifts) { trial("up>inv", $L, sub { flip($L) }) }
print O "-- inv>up: car on its roof at height y_up+H (inverted pose), Flip(0)\n";
for my $H (@lifts) { trial("inv>up", $H, sub { flip(0); setheight($yup + $H); flip(0) }) } }
print O "done\n";
