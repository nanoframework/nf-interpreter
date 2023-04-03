#!/usr/bin/perl

use strict;
use File::Slurp;

my $opt = shift;

my $yaml;
if ($opt eq "--main") {
   $yaml = read_file("build_skyworks_evb.main.yml.template");
} elsif ($opt eq "--branch") {
   $yaml = read_file("build_skyworks_evb.yml.template");
} else {
   die "bad usage\n";
}

my $targets_buff = read_file("SKY-TARGETS.txt");
my @targets = split /\r\n|\n|\r/, $targets_buff;

foreach my $target (@targets)
{
   print("$target ...\n");
   $yaml =~ s/\@TARGET\@/$target/g;
   write_file("Build_$target.yaml", $yaml);
}
