{
    description = "Nix Flake-Based development environment for BFI";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
    };

    outputs = { self, nixpkgs, ... }: let
        system = "x86_64-linux";
    in {
        devShells."${system}".default = let
            pkgs = import nixpkgs { inherit system; };
        in pkgs.mkShell {
            packages = with pkgs; [
                # compiler and cmake
                clang_17
                llvmPackages_17.bintools
                cmake
                gnumake

                # debugging stuff
                gdb
                valgrind
            ];

            shellHook = ''
            mkdir -p build/
            CC=clang cmake -DOPTIMIZE=1 -B build/ -S .
            '';
        };
    };
}
