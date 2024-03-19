{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
    packages = with pkgs; [
        ## compiler and cmake
        clang_17
        llvmPackages_17.bintools
        gnumake
        nasm

        ## debugging stuff
        gdb
        gf
        valgrind
    ];

    shellHook = ''
    export CC=clang
    export CXX=clang++
    '';
}
