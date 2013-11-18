jshund
======

a tool that list unreferenced local variables in JavaScript source code.

building jshund
===============

Download the repo:

    git clone https://github.com/jsteemann/jshund
    cd jshund 

Then compile it using a C++ compiler (e.g. `g++`):

    g++ -Wall -Wextra jshund.cpp -o jshund

You then have a binary `jshund` available in the local directory.

usage
=====

to invoke jshund, run it on a list of files or directories, e.g.:

    jshund <file>
    jshund <file1> <file2> ...
    jshund <directory>

