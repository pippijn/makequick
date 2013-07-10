#!/usr/bin/env perl

use strict;
use warnings FATAL => 'all';
use utf8;

use Dir::Self;

my $nodes = do (__DIR__ . "/nodes.pm");

for my $node (keys %$nodes) {
   my ($params, $args);
   my $i = 0;
   for (@{ $nodes->{$node} }) {
      if ($_ eq '*') {
         $params = "";
         $args = "";
      } elsif ($_ eq '+') {
         $params = ", node_ptr const &e0";
         $args = ", e0";
      } else {
         $params .= ", node_ptr const &$_";
         $args .= ", $_";
      }
      ++$i;
   }

   my ($num) = $node =~ /_([01])$/;
   $node =~ s/_[01]$//;

   print <<EOF unless $num;
t_${node}::t_$node ()
  : generic_node_t (n_$node, location::generated)
{
}

t_${node}::t_$node (location const &loc$params)
  : generic_node_t (n_$node, loc$args)
{
}

EOF
}
