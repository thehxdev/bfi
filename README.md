# BFI

BFI is a BrainF\*ck interpreter and compiler written in C and compatible with C99 standard.

For more information about BrainF\*ck read the [BrainF\*ck wikipedia page](https://en.wikipedia.org/wiki/Brainfuck).

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
- `NON_STD`: 
You can set `NON_STD` to 1 if you want the commands that I added to `bfi` like `?` command.
otherwise `bfi` acts like a standard BrainF\*ck interpreter with no extra commands.

- `OPTIMIZE`: 
To compile with debug information, set `OPTIMIZE` to 0.

- `SAFE_BFI`: 
To add memory access protections set `SAFE_BFI` to 1.
This makes bfi slower but out of range accesses to memory while executing BrainF\*ck source code will be catched.
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

After you built `bfi`, You can run examples provided in `examples` directory with `-r` flag.
while you are in the project root directory:
```bash
# run `examples/helloWorld.bf` file
# this will print `Hello World!` to stdout
./build/bfi -r 'examples/helloWorld.bf'
```

### Compiler

> [!NOTE]
> The non-standard commands (see `NON_STD` in build section) are not impelemted in compiler.

To compile BrainF\*ck source code to **x64 Assembly** ([Nasm](https://www.nasm.us/) compatible), use `-asm` and `-o` flag.
Then you can use `basm.sh` script to build the assembly file.
```bash
# compile `examples/helloWorld.bf` file to `helloWorld.S`
./build/bfi -asm 'examples/helloWorld.bf' -o './helloWorld.S'

./basm.sh './helloWorld.S'
```

## More Info

bfi ignores all characters that are not a valid BrainF\*ck command. BrainF\*ck has only 8 commands:

- `>` : increment data pointer by one
- `<` : decrement data pointer by one
- `+` : increment byte at the data pointer by one
- `-` : decrement byte at the data pointer by one
- `.` : output the data at data pointer (print to stdout as single character)
- `,` : accept one character from stdin and store it in data pointer
- `[` : jump to next command of matching `]`, if the data pointer's data is zero
- `]` : jump to next command of matching `[` (goes backward), if the data pointer's data is non-zero


If you bulid `bfi` with `NON_STD=1` flag (see Build section), you have access to more commands
that I added to `bfi`:

- `?` : reset everything to initial state (set all data to 0 and rewind data pointer).
