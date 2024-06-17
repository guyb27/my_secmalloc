---
title: "my_secmalloc"
description: "Tool that replicates malloc behavior in a secure way"
date: N/A
tags:
  - Programming
  - C
  - Linux
---

# my_secmalloc
C project that securely replicates malloc functionality in order to detect the exploitation of certain vulnerabilities such as double free, heap overflow or memory leak.

## How to setup the project
### Prerequisites
You'll need to check that each of these tools is installed on your device:
- [git](https://git-scm.com/download/win)
- [make](https://cmake.org/download/)
- [criterion](https://criterion.readthedocs.io/en/master/setup.html) build and installed in `/usr` with its dependencies
- [gef](https://github.com/hugsy/gef) and [valgrind](https://valgrind.org/downloads/current.html) for debugging, which will be a great help if you wish to contribute to the project.

### Steps to build the project and use the allocator
#### How to build
```bash
git clone git@github.com:guyb27/my_secmalloc.git
cd my_secmalloc/my_secmalloc
```
**Then you have several options for building the project:**
- ``make static`` which will give you the static library ``libmy_secmalloc.a`` in the ``./build/lib`` folder
- ``make dynamic``, which gives you the dynamic library ``libmy_secmalloc.so`` in the ``./build/lib`` folder
- ``make test`` which gives you the test binary ``test`` in the ``./build/test`` folder, it works thanks to the static lib
- ``make script`` which will give you a ``my_secmalloc.sh`` script in the ``./build`` folder, allowing you to run a program with the allocator without having to specify ``LD_PRELOAD``.
- ``make all``, which builds the two libraries and, at the same time, executes the tests by building the test binary.
  *(actually, make all doesn't build the dynamic library in the right way.
  dynamic library, you'll see that the malloc, free, realloc and calloc functions aren't present, so you'll need to do a make dynamic
  or make clean dynamic (if lib.so has already been built) to get a working dynamic library (a patch is currently under development)*.

You can also directly download the libraries on our repository [here](https://github.com/guyb27/my_secmalloc)!

#### How to use the allocator with a utility/program?
By default, the program saves its logs in a default file named ``default_log.txt``. The log file is determined by the ``MSM_OUTPUT`` environment variable, which you are free to modify.

There are two main options for using the allocator with a utility/program:
- ``LD_PRELOAD``, which loads the dynamic library before other libraries:
````bash
  LD_PRELOAD=./build/lib/libmy_secmalloc.so sl
````

- Use the script at ``./build/my_secmalloc.sh`` having been created via a ``make all`` or ``make script`` which allows you to launch a program with the allocator:
````bash
./build/my_secmalloc.sh python3
````
The script is currently not optimized.

## Troubleshooting
### Troubleshooting common problems
- If you get compilation errors, you may not have installed the necessary dependencies to compile the project.
  Check that you have installed the necessary dependencies to compile the project.
- If you find yourself running a ``make test`` and encounter a problem, make sure you've built the static library needed to run the tests.
  Make sure you've also installed criterion in `/usr` and its dependencies.
  You may also need to perform a ``make clean static`` before redoing a ``make test``.
- At present, the solution does not allow you to run every program, so check that you have the malloc, calloc, realloc and free symbols with this command:
  ``nm libmy_secmalloc.so | grep " T " | grep -v my_ | cut -f3 -d' ' | sort``. If you don't have these symbols, do a ``make clean`` and then a ``make dynamic``.
- For any other error, don't hesitate to open an issue in accordance with the given template.

## Resources used

- [malloc()](https://linux.die.net/man/3/malloc), [calloc()](https://linux.die.net/man/3/malloc), [realloc()](https://linux.die.net/man/3/malloc), and [free()](https://linux.die.net/man/3/malloc)
- [malloc internals](https://sourceware.org/glibc/wiki/MallocInternals)
- [USENIX Security '18 - Guarder: A Tunable Secure Allocator](https://www.youtube.com/watch?v=Q26_z5oKTVA)
