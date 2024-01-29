# BFI

BFI is a BrainF\*ck language interpreter written in C and compatible with C99 standard.

For more information about BrainF\*ck read the [BrainF\*ck wikipedia page](https://en.wikipedia.org/wiki/Brainfuck).

You can find more complex and interesting examples in [brainfuck.org](https://brainfuck.org) website. download `*.b` source files
and execute them with BFI.


## Build
First clone the source code using git.
```bash
git clone --depth 1 --branch=main https://github.com/thehxdev/bfi

cd bfi
```

Make sure that you have a C compiler like `gcc` or `clang`, `cmake` and `make` installed.
```bash
mkdir -p build/

# You can set `-DNON_STANDARD_CMDS` to 1 if you want
# the commands I personaly added to `bfi` like `?` command.
# otherwise `bfi` acts like a standard BrainF*ck interpreter
# with no extra commands.
#
# To compile with debug information, set `-DOPTIMIZE` to 0.
cmake -DNON_STANDARD_CMDS=0 -DOPTIMIZE=1 -B build/ -S .

cd build/
cmake --build .
cd ..
```

Then you can use `bfi` executable in `build/` directory.


## Use
After you built `bfi`, You can run examples provided in `examples` directory.
if you are in the project root directory:
```bash
# run `examples/HelloWorld.bf` file
# this will print `Hello World!` to stdout
./build/bfi 'examples/HelloWorld.bf'
```

`bfi` ignores all characters that are not a valid BrainF\*ck command. BrainF\*ck has only 8 commands:

- `>` : increment data pointer by one
- `<` : decrement data pointer by one
- `+` : increment byte at the data pointer by one
- `-` : decrement byte at the data pointer by one
- `.` : output the data at data pointer
- `,` : accept one byte of input and store it in data pointer
- `[` : jump to next command of matching `]`, if the data pointer's data is zero
- `]` : jump to next command of matching `[` (goes backward), if the data pointer's data is non-zero


If you bulid `bfi` with `-DNON_STANDARD_CMDS=1` flag (see Build section), you have access to more commands
that I personaly added to `bfi`:

- `?` : reset everything to initial state (set all data to 0 and rewind data pointer).
