{
    description = "Nix Flake-Based development environment for BFI";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
    };

    outputs = { self, nixpkgs, ... }: let
        supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

        forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

        nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in {

        defaultPackage = forAllSystems (system: let
            pkgs = nixpkgsFor.${system};
        in pkgs.stdenv.mkDerivation {
            name = "bfi";
            src = ./.;
            cmakeFlags = [
                "-DOPTIMIZE=1"
            ];
            nativeBuildInputs = with pkgs; [
                clang_17
                llvmPackages_17.bintools
                cmake
            ];
        });

        devShells = forAllSystems (system: let
            pkgs = nixpkgsFor.${system};
        in { 
            default = pkgs.mkShell {
                buildInputs = with pkgs; [
                    # compiler and cmake
                    clang_17
                    llvmPackages_17.bintools
                    cmake
                    gnumake

                    # debugging stuff
                    gdb
                    valgrind
                ];
                # shellHook = ''
                # '';
            };
        });
    };
}
