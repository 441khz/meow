{ callPackage, pkgsi686Linux }:

{
  gcc4ti = callPackage ./gcc4ti.nix { };
  tiemu = callPackage ./tiemu/tiemu.nix { };
  texinfo4 = callPackage ./texinfo4.nix { };
  tigcc-i686-bin = pkgsi686Linux.callPackage ./tigcc-bin.nix { };
}
