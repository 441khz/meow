{ callPackage, ... }:

{
  libticables2 = callPackage ./libticables2.nix { };
  libticalcs2 = callPackage ./libticalcs2.nix { };
  libticonv = callPackage ./libticonv.nix { };
  libtifiles2 = callPackage ./libtifiles2.nix { };
  libusb = callPackage ./libusb.nix { };
}
