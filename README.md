# BFI

BFI is a fast and performant BrainFuck interpreter and compiler written in C (C99).

For more information about BrainFuck read the [BrainFuck wikipedia page](https://en.wikipedia.org/wiki/Brainfuck).

You can find more complex and interesting examples in [brainfuck.org](https://brainfuck.org) website.
download `*.b` source files and execute them with BFI.



## Build
First clone the source code using git.
```bash
git clone --depth=1 --branch=main https://github.com/thehxdev/bfi

cd bfi
```

Make sure that you have a C compiler like `gcc` or `clang` and `make` installed.

### make build options:
- `OPTIMIZE`: 
To compile with debug information, set `OPTIMIZE` to 0.

- `SAFE_BFI`: 
To add memory access protections set `SAFE_BFI` to 1.
This makes bfi slower but out of range accesses to memory while executing BrainFuck source code will be catched.
(This only affects the interpreter and NOT the compiler!)

- `STATIC`:
Set `STATIC` option to 1 to build bfi as a standalone, staticly linked binary using [musl libc](https://musl.libc.org/).
But befor that, make sure that you have `musl-clang` and musl libc installed.


### Build bfi
Build bfi using the folloing command:
```bash
make OPTIMIZE=1
```
Then you can use `bfi` executable in `build/` directory.



## Usage
You can use `bfi` as an interpreter or a compiler.


### Interpreter

After you built `bfi`, You can run examples provided in `examples` directory. Just execute `bfi` and provide
the source code file as first command-line arugment.
while you are in the project root directory:
```bash
# run `examples/helloWorld.bf` file
# this will print `Hello World!` to stdout
./build/bfi -r 'examples/helloWorld.bf'
```

### Compiler

To compile BrainFuck source code to **x64 Assembly** (AT&T Syntax), provide source code file as first argument
and output file as second argument.
Then you can use `gcc` to build the executable from generated assembly file.
```bash
# compile `examples/helloWorld.bf` file to `helloWorld.S`
./build/bfi 'examples/helloWorld.bf' './helloWorld.S'

# assemble `helloWorld.S` file
gcc './helloWorld.S'
```

## More Info

bfi ignores all characters that are not a valid BrainFuck command. BrainFuck has only 8 commands:

- `>` : increment data pointer by one
- `<` : decrement data pointer by one
- `+` : increment byte at the data pointer by one
- `-` : decrement byte at the data pointer by one
- `.` : output the data at data pointer (print to stdout as single character)
- `,` : accept one character from stdin and store it in data pointer
- `[` : jump to next command of matching `]`, if the data pointer's data is zero
- `]` : jump to next command of matching `[` (goes backward), if the data pointer's data is non-zero

