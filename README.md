# BFI

BFI is a BrainFuck language interpreter written in C and compatible with C99 standard.


## Build
First clone the source code using git.
```bash
git clone --depth 1 --branch=main https://github.com/thehxdev/bfi

cd bfi
```

Make sure that you have a C compiler like `gcc` or `clang`, `cmake` and `make` installed.
```bash
mkdir -p build/
cmake -DOPTIMIZE=1 -B build/ -S .

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
