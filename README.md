jshund
======
A tool to list unreferenced local variables in JavaScript source code files.

Unreferenced local variables typically occur after a few iterations of 
modifications a program. Some code is changed, and some variables become
useless afterwards. This is not necessarily a program, but if a variable
name is misspelled or assigned only, it might be worth checking if a real
error is lurking behind that.

JavaScript engines normally don't complain about unreferenced local variables 
in script code. jshund helps finding them.

Example:
```javascript
function () {
  var value = Math.random();

  return 2 + 3;
}
```
In the above example, variable `value` is a local variable which is assigned
a value but is not used otherwise. At least the left-hand side of the assignment
(`var value = `) can be removed here without changing the meaning of the program.

Whether or not the right-hand side of the assignment (`Math.random()`) can be 
removed without changing the meaning of the program is up to developer inspection. 
Non-constant expressions on the right-hand side may have side effects (e.g. function 
calls, exceptions thrown etc.) and simply removing them might lead to different
program behavior.
    
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
jshund will simply assume these are meaningful global variables (yuck!).

jshund also assumes `eval` is not used anywhere in the code. `eval` has nice
side-effects, and whether or not a local variable is referenced in eval'ed
code is hard to determine. So if you use `eval` somewhere, don't let jshund
check these scripts.
