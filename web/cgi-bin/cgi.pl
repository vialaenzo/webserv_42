#!/usr/bin/env perl

use strict;
use warnings;
use POSIX;

my $content = strftime("%Y-%m-%d %H:%M:%S", localtime);
my $length = length($content);

print <<"END";
HTTPS/1.1 200 OK\r
Content-Type: text/plain\r
Content-Length: $length\r
\r
$content
END
