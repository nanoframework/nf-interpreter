#!/usr/bin/perl

use strict;
use File::Slurp;

my $opt = shift;

my $yaml_file;
if ($opt eq "--main") {
   $yaml_file = "build_skyworks_evb.main.yml.template";
} elsif ($opt eq "--branch") {
   $yaml_file = "build_skyworks_evb.yml.template";
} else {
   die "bad usage\n";
}

my %presets_found;

my $builds_file = "SKY-BUILDS.txt"; 
my $buff = read_file($builds_file);
my @lines = split /\r\n|\n|\r/, $buff;

foreach my $line (@lines)
{
   # warn $line;
   next if ($line =~ /^\s*#/);
   next if ($line =~ /^\s*$/);
   my($target,$preset) = split(/,/,$line);
   
   # Don't allow a preset name to be used more than once
   die "preset $preset is used more than one in  $builds_file\n" if ($presets_found{$preset});
   $presets_found{$preset} = 1;
   
   print("$target/$preset ...\n");
   my $yaml = read_file($yaml_file);
   $yaml =~ s/\@TARGET\@/$target/g;
   $yaml =~ s/\@PRESET\@/$preset/g;
   write_file("Build_$preset.yaml", $yaml);
}
