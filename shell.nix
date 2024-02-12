{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
    packages = with pkgs; [
        ## compiler and cmake
        clang_17
        llvmPackages_17.bintools
        cmake
        ninja
        gnumake

        ## debugging stuff
        gdb
        valgrind
    ];

    shellHook = ''
    CC=clang
    CXX=clang++
    '';
}
