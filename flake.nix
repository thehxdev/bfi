{
    description = "BFI Nix Flake";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";
    };

    outputs = { self, nixpkgs, ... }: let
        pkgName = "bfi";

        supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];

        forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

        nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in {

        packages = forAllSystems (system: let
          pkgs = nixpkgsFor.${system};
        in {
            ${pkgName} = pkgs.stdenv.mkDerivation {
                name = "${pkgName}";
                src = ./.;
                cmakeFlags = [
                    "-GNinja"
                    "-DOPTIMIZE=1"
                ];
                buildInputs = with pkgs; [
                    clang_17
                    llvmPackages_17.bintools
                ];
                nativeBuildInputs = with pkgs; [
                    cmake
                    ninja
                ];
            };
        });

        defaultPackage = forAllSystems (system: self.packages.${system}.${pkgName});

        devShells = forAllSystems (system: let
            pkgs = nixpkgsFor.${system};
        in { 
            default = import ./shell.nix { inherit pkgs; };
        });
    };
}
