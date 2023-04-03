#!/usr/bin/perl

use strict;
use File::Slurp;

my $targets_buff = read_file("SKY-TARGETS.txt");
my @targets = split /\r\n|\n|\r/, $targets_buff;

foreach my $target (@targets)
{
   print("$target ...\n");
   my $yaml = read_file("build_skyworks_evb.yml.template");
   $yaml =~ s/\@TARGET\@/$target/g;
   write_file("Build_$target.yaml", $yaml);
}
