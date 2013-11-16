jshund
======

a tool that list unreferenced local variables in JavaScript source code.

building jshund
===============

Download the repo:

    git clone https://github.com/jsteemann/jshund
    cd jshund 

Then run configure and make

    ./configure
    make

usage
=====

to invoke jshund, run it on a list of files or directories, e.g.:

    jshund <file>
    jshund <file1> <file2> ...
    jshund <directory>

