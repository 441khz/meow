{ fetchurl
, stdenv
, ...
}:

stdenv.mkDerivation rec {
  pname = "libusb";
  version = "0.1.12";

  src = fetchurl {
    url = "mirror://sourceforge/${pname}/${pname}-${version}.tar.gz";
    sha256 = "sha256-N/b32d50GW61/Au+CuqbXJOd5/UArLo69v1kPztTi0Q=";
  };

  NIX_CFLAGS_COMPILE = [
    "-Wno-error"
  ];
}
