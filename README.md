# Basic Kernel Implementation
This is a project for Grinnell College CSC-395 2022S with professor Charlie Curtsinger. Starter code for setting up bootloader was provided.

## Functionality
We will list functionalities of our basic kernel below:
* Basic IO
* Print usable memory
* Exception printer
* Virtual to physical address
* Page Mapping
* Basic system call
* Loading and running executable (elf)
* stdlib
* Change to usermode
* Multi-tasking via threads

## Running Our Project & Interpreting Its Output

To run our project, use the command 'make run' in the command line.

Internally, multiple threads are created. This is represented as:
* The message "thread_create called" indicates that a program has created a thread. 
* Upon entering a thread, "got to __threadname__" is printed
* Finally, upon finishing the thread "finished __threadname__" is printed

We have two demos, to run the first one we type "hello" into the shell. This demo uses the system time interrupt to switch between threads. In this demo, the main thread is created first (see "we are in main"). Then, the foo thread is created, the scheduler at some point switches to it and finished (this is followed by take, another thread that is created and finished). Finally, the scheduler switches back and main finishes.

This represents multi-tasking as the main function is started first, but finished last and therefore the processor switched to work on the foo and take threads before switching back to main

The second demo is called by typing "world" in the shell, it shows that the scheduler actually works although it is not being triggered by the system time interupt but by how many sys_calls the user has made.

We have a general protection error that happens occasionally but re running using 'make run' re runs the kernel and typically resolves the issue.

## Acknowledgements
The starter code is provided by professor Charlie Curtsinger, references for starter code is listed below.

The basic kernel implementation was completed by Brady Zheng and Noah Fehr. Multi-tasking was completed by Jemzy Alabi, Noah Fehr, Brady Zheng and Mack Trachtenberg.

The starter code is based on the following example projects:
- [OSDev.org Bare Bones Kernel](https://wiki.osdev.org/Bare_bones)
- [Stivale2 Barebones Kernel](https://github.com/stivale/stivale2-barebones)

In addition to the above example projects, the following references were used when setting up starter code:
- [OSDev.org GCC Cross Compiler Instructions](https://wiki.osdev.org/GCC_Cross-Compiler)
- [Stivale2 Reference](https://github.com/stivale/stivale/blob/master/STIVALE2.md)
