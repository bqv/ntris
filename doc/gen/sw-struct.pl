#!/usr/bin/perl
# synopsis
# perl sw-struct.pl <source path> <prefix of sourcefiles> <output file>

use strict;

my ($path, $prefix, $output) = @ARGV;

my $dot = "dot";

my $data;

# collect includes and externs from the code
sub proc()
{
  while(<$path/*.c $path/*.h>)
  {
    /$prefix(\w+)\.\w$/;

    my $file = $1;

    #print "analyse $file\n";

    open F, $_;

    while (<F>)
    {
      if (/^\s*\#\s*include\s+["<]$prefix(\w+)\.\w+[>"]/)
      {
        my $incfile = $1;

        #print "found include of $incfile in $file\n";

        if ($incfile ne $file)
        {
          $data->{$incfile}->{included}->{$file} = 1;
          $data->{$file}->{includes}->{$incfile} = 1
        }
      }
      elsif (/^\s*extern\s+.*?$file(\w+)\s*(\(|)/)
      {
        my $extern = $1.($2 eq '(' ? '()' : '');
        #print "found extern $extern \n";
        $data->{$file}->{fields}->{$extern} = 1;
      }
    }

    close F;
  }
}

# print and generate dot graph for collected datas
sub printdot()
{
  my ($base, $ext) = ($output =~ /^(.+)\.(\w+)$/)
                     ? ($1, $2)
                     : ('temp','png');

  open OUT, ">$base.dot";

  print OUT "digraph G {\n";
  print OUT "node [shape=box, fontsize=8];\n";
  print OUT "edge [len=5];\n";

  my $n = 0;
  for (sort(keys(%{$data})))
  {
    $data->{$_}->{num} = $n++;
    my $label = $_."|".join("\\n",sort(keys(%{$data->{$_}->{fields}})));
    my $url   = '';
    print OUT "$_ [shape=record,label = \"$label\", href=\"$url\"];\n";
  }

  for (sort(keys(%{$data})))
  {
    my $file = $_;

    #print "print out $file\n";

    for (sort(keys(%{$data->{$file}->{includes}})))
    {
      #print "print include $_\n";

      print OUT "$_ -> $file;\n";
    }
  }

  print OUT "}\n";

  close OUT;

  print `$dot -T$ext -o $base.$ext $base.dot`;
}

#
# M A I N
#

proc();

printdot();
