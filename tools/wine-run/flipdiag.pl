#!/usr/bin/perl
# flipdiag.pl <pid> <lift> [secs]: flip car 0 once (MashedTrainer Player.Flip) and log every 0.1 s: t, buffer index, y, up.y, vel(0x144) -> $OUT/flipdiag.txt
use Time::HiRes qw(time sleep);
my ($pid, $lift, $secs) = @ARGV; my $BASE = 0x8B06E0;
open(M, "+<", "/proc/$pid/mem") or die; open(O, ">>", "$ENV{OUT}/flipdiag.txt") or die; select((select(O), $| = 1)[0]);
sub rd { my ($a, $n) = @_; sysseek(M, $a, 0); sysread(M, my $b, $n); $b }
sub wr { my ($a, $b) = @_; sysseek(M, $a, 0); syswrite(M, $b) }
sub fl { my ($a, $n) = @_; unpack("f<$n", rd($a, 4 * $n)) }
sub idx { unpack("V", rd($BASE + 0x9AC, 4)) }
sub cur { $BASE + 0x928 + (idx() & 1) * 0x40 }
sub flip { my $c = cur(); my @r = fl($c, 3); my @u = fl($c + 0x10, 3); my @p = fl($c + 0x30, 3);
  @r = map { -$_ } @r; @u = map { -$_ } @u; $p[1] += $lift;
  for my $i (0, 1) { my $m = $BASE + 0x928 + $i * 0x40; wr($m, pack("f<3", @r)); wr($m + 0x10, pack("f<3", @u)); wr($m + 0x30, pack("f<3", @p)) } }
sub st { my $t = shift; my @p = fl(cur() + 0x30, 3); my @u = fl(cur() + 0x10, 3); my @v = fl($BASE + 0x144, 3);
  printf O "%.1f idx %d y %.3f up.y %+.2f  vel %.2f %.2f %.2f  alive %d\n", $t, idx(), $p[1], $u[1], @v, unpack("V", rd($BASE + 4, 4)) }
printf O "-- lift %s\n", $lift; st(-0.1); flip(); my $t0 = time;
while ((my $t = time - $t0) < ($secs || 6)) { st($t); sleep 0.1 }
