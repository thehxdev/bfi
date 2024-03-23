{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell {
    packages = with pkgs; [
        ## compiler and cmake
        llvmPackages.clang
        llvmPackages.bintools
        gnumake

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
