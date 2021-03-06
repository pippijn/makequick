#!/usr/bin/env perl

use strict;
use warnings FATAL => 'all';
use utf8;

use Dir::Self;

my $nodes = do (__DIR__ . "/nodes.pm");

sub gen (&) {
   my $sub = shift;
   for my $node (keys %$nodes) {
      my ($params, $args, $accessors);
      my $i = 0;
      for (@{ $nodes->{$node} }) {
         if ($_ eq '*') {
            $params = "";
            $args = "";
            $accessors = "";
         } elsif ($_ eq '+') {
            $params = ", node_ptr const &e0";
            $args = ", e0";
            $accessors = "";
         } else {
            $params .= ", node_ptr const &$_";
            $args .= ", $_";
            $accessors .= "\n  node_ptr const &$_ () const { return (*this)[$i]; }";
            $accessors .= "\n  void $_ (node_ptr const &n) { set ($i, n); }";
         }
         ++$i;
      }

      my ($num) = $node =~ /_([01])$/;
      $node =~ s/_[01]$//;

      $sub->($num, $node, $params, $args, $accessors);
   }
}

gen {
   my ($num, $node, $params, $args, $accessors) = @_;

   print <<EOF unless $num;
struct t_$node
  : generic_node_t<t_$node>
{
  virtual void accept (visitor &v) { v.visit (*t_${node}_ptr (this)); }

  t_$node ();
  t_$node (location const &loc$params);$accessors
};

EOF
};

gen {
   my ($num, $node, $params, $args, $accessors) = @_;

   print <<EOF;
template<>
inline node_list *
make_node<n_$node> (location const &loc$params)
{
EOF
   if ($num) {
      print "  return (new t_$node (loc))->add (e0);\n}\n\n";
   } else {
      print "  return new t_$node (loc$args);\n}\n\n";
   }
};
