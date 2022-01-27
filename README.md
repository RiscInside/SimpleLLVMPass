# Simple LLVM pass

This repository contains a simple example of a fully functional LLVM pass.

`CountPass.cpp` file implements a simple LLVM pass that calculates number of functions, basic blocks, and instructions in each basic block.
This pass also produces a histogram of basic blocks by instruction count using gnuplot tool.

## Dependencies

This project requires llvm-11, make, cmake, and gnuplot to be installed.

## Building

CMake build system is used to build an LLVM plugin with the pass. Makefile wrapper is provided for convinience.

Run `make build` to build the plugin.

Run `make` to test the plugin on the `program.c` file:

```c
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("The program expects one argument\n");
    return 1;
  }

  if (argv[1][0] == 'A')
    printf("A\n");
  else if (argv[1][0] == 'B')
    printf("B\n");
  else
    printf("C\n");

  return 0;
}

```

By default, Makefile uses `clang` and `opt` commands for emiiting LLVM IR and running the pass. To use `clang-11` and `opt-11` instead, set `CLANG` and `OPT` variables to `clang-11` and `opt-11` respectfully.

```bash
make CLANG=clang-11 OPT=opt-11
```

## Acknowledgements

[This](https://osric.com/chris/accidental-developer/2018/10/creating-a-histogram-with-gnuplot/) blog helped me to figure out how to draw histograms in gnuplot.

[This](https://llvm.org/docs/WritingAnLLVMPass.html) page helped me with writing skeleton code for this LLVM pass.

I used CMakeLists.txt from [this](https://github.com/banach-space/llvm-tutor) GitHub repository with some minor changes.
