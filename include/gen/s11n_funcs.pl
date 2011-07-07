#!/usr/bin/env perl

use strict;
use warnings FATAL => 'all';
use utf8;

use Dir::Self;

my $nodes = do (__DIR__ . "/nodes.pm");

for my $node (keys %$nodes) {
   my ($num) = $node =~ /_([01])$/;
   $node =~ s/_([01])$//;
   print <<EOF unless $num
template<typename Archive>
static void
serialize (Archive &ar, t_$node &n, unsigned int version)
{
  serialize (ar, implicit_cast<generic_node &> (n), version);
}

EOF
}
