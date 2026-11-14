#!/usr/bin/perl
# flipdiag2.pl <pid> <lift> [secs]: like flipdiag.pl but logs both matrix buffers and both indices (cur 0x9A8, prev 0x9AC) every 0.05 s
use Time::HiRes qw(time sleep);
my ($pid, $lift, $secs, $fix) = @ARGV; my $BASE = 0x8B06E0;
open(M, "+<", "/proc/$pid/mem") or die; open(O, ">>", "$ENV{OUT}/flipdiag2.txt") or die; select((select(O), $| = 1)[0]);
sub rd { my ($a, $n) = @_; sysseek(M, $a, 0); sysread(M, my $b, $n); $b }
sub wr { my ($a, $b) = @_; sysseek(M, $a, 0); syswrite(M, $b) }
sub fl { my ($a, $n) = @_; unpack("f<$n", rd($a, 4 * $n)) }
sub u { unpack("V", rd($_[0], 4)) }
sub flip { my $c = $BASE + 0x928 + (u($BASE + 0x9AC) & 1) * 0x40; my @r = fl($c, 3); my @u = fl($c + 0x10, 3); my @p = fl($c + 0x30, 3);
  @r = map { -$_ } @r; @u = map { -$_ } @u; $p[1] += $lift;
  for my $i (0, 1) { my $m = $BASE + 0x928 + $i * 0x40; wr($m, pack("f<3", @r)); wr($m + 0x10, pack("f<3", @u)); wr($m + 0x30, pack("f<3", @p)) } }
sub st { my $t = shift; my @b0 = fl($BASE + 0x928 + 0x10, 3); my @p0 = fl($BASE + 0x928 + 0x30, 3); my @b1 = fl($BASE + 0x968 + 0x10, 3); my @p1 = fl($BASE + 0x968 + 0x30, 3);
  printf O "%.2f cur(9A8) %d prev(9AC) %d | buf0 up.y %+.2f y %.3f | buf1 up.y %+.2f y %.3f | rwp_obj %d rwp_timer %d contacts %d\n", $t, u($BASE + 0x9A8), u($BASE + 0x9AC), $b0[1], $p0[1], $b1[1], $p1[1], u($BASE + 0x14), u($BASE + 0x18), u($BASE + 0x9E8) }
printf O "-- lift %s fix %s\n", $lift, $fix || 0; st(-0.05); flip(); if ($fix) { wr($BASE + 0x14, pack("V", 0)); wr($BASE + 0x18, pack("V", 600)) } my $t0 = time;
while ((my $t = time - $t0) < ($secs || 2)) { st($t); sleep 0.05 }
