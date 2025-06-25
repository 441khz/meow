{
  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
    nixpkgs-old.url = "nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, nixpkgs-old, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        tiToolchainOverlay = final: prev: {
          tiLibraries = final.callPackage ./nix/pkgs/tiLibraries { };
          tiTools = final.callPackage ./nix/pkgs/tiTools { };
        };

        pkgs = import nixpkgs {
          inherit system;

          overlays = [ tiToolchainOverlay ];
        };
      in
      {
        formatter = pkgs.nixpkgs-fmt;

        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            ninja

            python3Packages.python
            python3Packages.pillow
            python3Packages.numpy

            imagemagick

            tiTools.gcc4ti
            tiTools.tiemu
            llvmPackages.clang-tools

            doxygen
          ];

          TIGCC = "${pkgs.tiTools.gcc4ti}";
          CLANGD_FLAGS = "--header-insertion=never";
        };

        packages = {
          inherit (pkgs.tiTools) gcc4ti tiemu tigcc-bin;

          default = pkgs.tiTools.gcc4ti;
        };
      });
}
