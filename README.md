jshund
======
A tool to list unreferenced local variables in JavaScript source code files.

Building jshund
---------------
Download the repo:

    git clone https://github.com/jsteemann/jshund
    cd jshund 

Then compile it using a C++ compiler (e.g. `g++`):

    g++ -Wall -Wextra jshund.cpp -o jshund

or, when using `clang++`:

    clang++ -Wall -Wextra jshund.cpp -o jshund

You then have a binary `jshund` available in the local directory.

Usage
-----
To invoke jshund, run it on a list of files or directories, e.g.:

    jshund <file>
    jshund <file1> <file2> ...
    jshund <directory>

All results will be printed on stdout. If stdout is a terminal, the output
may be colorful. To turn off color output, use the `--no-colors` option.

To suppress complaints about global variables, use the `--no-globals` option.

Caveats
-------
jshund assumes that the JavaScript source code you feed it is parseable and 
syntactially valid. It might return lots of bogus messages or crash if the
input is syntactically invalid JavaScript code.
