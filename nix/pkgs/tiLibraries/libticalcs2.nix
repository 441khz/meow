{ glib
, pkg-config
, stdenv
, tiLibraries
, ...
}:

stdenv.mkDerivation rec {
  pname = "libticalcs2";
  version = "1.1.0";

  src = fetchTarball {
    url = "http://www.calcforge.org:70/downloads/tiemu-3.02a/${pname}-${version}.tar.bz2";
    sha256 = "sha256:0nsl7jpr45am5x4daddgib5bms605jyp51f4n7srd3k9bxy5x6vy";
  };

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    glib

    tiLibraries.libticables2
    tiLibraries.libticonv
    tiLibraries.libtifiles2
  ];

  NIX_CFLAGS_COMPILE = [
    "-fcommon"
  ];
}
