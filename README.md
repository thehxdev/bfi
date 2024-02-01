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
# the commands that I added to `bfi` like `?` command.
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
that I added to `bfi`:

- `?` : reset everything to initial state (set all data to 0 and rewind data pointer).



## Nix
Use `nix` package manager to build `bfi` or make an interactive shell for development with all dependencies.

### Build
> [!NOTE]
> Make sure that you enabled nix experimental features such as `nix-command` and `flakes`.

If you use [NixOs](https://nixos.org) or you installed `nix` package manager, you can build `bfi` in one command.
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
