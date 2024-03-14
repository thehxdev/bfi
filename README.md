# BFI

BFI is a BrainF\*ck interpreter and compiler written in C and compatible with C99 standard.

For more information about BrainF\*ck read the [BrainF\*ck wikipedia page](https://en.wikipedia.org/wiki/Brainfuck).

You can find more complex and interesting examples in [brainfuck.org](https://brainfuck.org) website. download `*.b` source files
and execute them with BFI.



## Build
First clone the source code using git.
```bash
git clone --depth=1 --branch=main https://github.com/thehxdev/bfi

cd bfi
```

Make sure that you have a C compiler like `gcc` or `clang`, `cmake` and `make` installed.

### Cmake build options:
- `NON_STD_CMDS`: 
You can set `-DNON_STD_CMDS` to 1 if you want the commands that I added to `bfi` like `?` command.
otherwise `bfi` acts like a standard BrainF*ck interpreter
with no extra commands.

- `OPTIMIZE`: 
To compile with debug information, set `-DOPTIMIZE` to 0.

- `SAFE_BFI`: 
To add memory access protections set `-DSAFE_BFI` to 1.
This makes bfi slower but out of range accesses to memory while executing BrainF\*ck source code will be catched.


To build `bfi`:
```bash
mkdir -p build/

cmake -DNON_STD_CMDS=0 -DSAFE_BFI=0 -DOPTIMIZE=1 -B build/ -S .

cmake --build build/
```

Then you can use `bfi` executable in `build/` directory.



## Usage

You can use `bfi` as an interpreter or a compiler.


### Interpreter

After you built `bfi`, You can run examples provided in `examples` directory with `run` sub-command.
while you are in the project root directory:
```bash
# run `examples/helloWorld.bf` file
# this will print `Hello World!` to stdout
./build/bfi run 'examples/helloWorld.bf'
```

### Compiler

To compile BrainF\*ck source code to **x64 Assembly** (Nasm compatible), use `compile` sub-command.
Then you can use `basm.sh` script to build the assembly file.
```bash
# compile `examples/helloWorld.bf` file to `helloWorld.asm`
./build/bfi compile -s 'examples/helloWorld.bf' -o './helloWorld.asm'

./basm.sh './helloWorld.asm'
```

## More Info

`bfi` ignores all characters that are not a valid BrainF\*ck command. BrainF\*ck has only 8 commands:

- `>` : increment data pointer by one
- `<` : decrement data pointer by one
- `+` : increment byte at the data pointer by one
- `-` : decrement byte at the data pointer by one
- `.` : output the data at data pointer
- `,` : accept one byte of input and store it in data pointer
- `[` : jump to next command of matching `]`, if the data pointer's data is zero
- `]` : jump to next command of matching `[` (goes backward), if the data pointer's data is non-zero


If you bulid `bfi` with `-DNON_STD_CMDS=1` flag (see Build section), you have access to more commands
that I added to `bfi`:

- `?` : reset everything to initial state (set all data to 0 and rewind data pointer).



## Nix
Use `nix` package manager to build `bfi` or make an interactive shell for development with all dependencies.

> [!NOTE]
> Nix flake version of BFI will be compiled with optimizations and **without** `SAFE_BFI` and `NON_STD_CMDS` flags.

> [!NOTE]
> Make sure that you enabled nix experimental features such as `nix-command` and `flakes`.

### Build

If you use [NixOS](https://nixos.org) or you installed `nix` package manager, you can build `bfi` in one command.
```bash
nix build
```
Then, you can run the executable from `./result/bin/bfi` path.


### Development Environment
To enter development environment:
```bash
nix develop
```

### Use BFI directly
Run `bfi` without downloading the source code and manually building it:
```bash
nix run github:thehxdev/bfi PATH_TO_BF_SOURCE_FILE
```

Or Enter an interactive shell with `bfi` command available without installing it:
```bash
nix shell github:thehxdev/bfi

# `bfi` is now available in new shell (exit with `exit` command or ctrl-d key)
$ bfi PATH_TO_BF_SOURCE_FILE
```
