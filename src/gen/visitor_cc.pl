#!/usr/bin/env perl

use strict;
use warnings FATAL => 'all';
use utf8;

use Dir::Self;

my $nodes = do (__DIR__ . "/nodes.pm");

for my $node (keys %$nodes) {
   my ($num) = $node =~ /_([01])$/;
   $node =~ s/_([01])$//;
   print "void visitor::visit (t_$node &n) { return visit ((generic_node &)n); }\n"
      unless $num
}
