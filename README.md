jshund
======
A tool to list unreferenced local variables in JavaScript source code files.

JavaScript engines normally don't complain about unreferenced local variables 
in script code, at least not at compile time. They may report such problems at
run time, but that might already be too late. Finding such cases at compile
time is much better. jshund assists in finding a few of those issues with
static analysis.

Unreferenced local variables typically occur after a few iterations of 
modifications of a program: some code gets changed, and that change may lead
to variables becoming assigned-only or even declared-only. 

This is not necessarily always a real problem, but sometimes indicates one. 
For example, a variable name might have been misspelled and that might manifest 
itself in a variable becoming assigned-only.

Example:
```javascript
function getName () {
  var firstname = "foobar"; // assigned only

  return firstName; // different spelling, mind the caps!
}
```

In this case, the unreferenced variable `firstname` indicates a real error.

In the following example, a variable `value` was used once but the due to some
modification it became unused:

```javascript
function () {
  var value = Math.random();

  //return value;
  return 0.5;
}
```

Variable `value` is now a local variable which is assigned a value but is not 
used otherwise anymore. 
    
Building jshund
---------------
Download the repo:

    git clone https://github.com/jsteemann/jshund
    cd jshund 

Then compile it using a C++ compiler (e.g. g++):

    g++ -Wall -Wextra src/jshund.cpp -o ./jshund

or, when using clang++:

    clang++ -Wall -Wextra src/jshund.cpp -o ./jshund

You will then have a binary `jshund` available in the local directory.

Usage
-----
To invoke jshund, run the compiled binary on a list of files or directories, 
e.g.:

    ./jshund <file>
    ./jshund <file1> <file2> ...
    ./jshund <directory>
    ./jshund <directory1> <directory2> ...

jshunt will print all its results on stdout. If stdout is a terminal, the 
output may be colorful. To turn off color output, use the `--no-colors` option.

To suppress complaints about global variables, use the `--no-globals` option.

Caveats
-------
jshund assumes that the JavaScript source code you feed it is parseable and 
syntactially valid. 

It might return lots of bogus messages or crash if the input is syntactically 
invalid JavaScript code.

jshund will only look for variables assigned using the `var` keyword. It will
not care about any variables that are assigned without using the `var` keyword.
jshund will simply assume these are meaningful global variables :cry:

jshund also assumes `eval` is not used anywhere in the code. `eval` has some
side-effects, and whether or not a local variable is referenced in eval'ed
code is hard to determine. So if you use `eval` somewhere, don't let jshund
check these scripts.

For all unreferenced variables that jshund will find, you need to figure out
yourself how to fix the problem. In the simplest case, the left-hand side of 
the variable assignment can be removed. In some cases, also the right-hand
side of the assignment can be removed without changing the meaning of the
program.

Please be aware that non-constant expressions on the right-hand side of an 
assignment may have side effects (e.g. function calls, exceptions thrown etc.) 
and simply removing them might lead to different program behavior.

In short: you should always inspect the code at the position indicated and 
determine for yourself if it needs a fix and which.
