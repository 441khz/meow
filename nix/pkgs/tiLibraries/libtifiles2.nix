{ glib
, pkg-config
, stdenv
, tiLibraries
, ...
}:

stdenv.mkDerivation rec {
  pname = "libtifiles2";
  version = "1.1.0";

  src = fetchTarball {
    url = "http://www.calcforge.org:70/downloads/tiemu-3.02a/${pname}-${version}.tar.bz2";
    sha256 = "sha256:01f612csmmbhj689qxrmdg3wh57qddr6aq37aff9c1y1bfd222zy";
  };

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    glib

    tiLibraries.libticonv
  ];

  # Fixes for GCC >= 13
  NIX_CFLAGS_COMPILE = [
    "-fpermissive"
  ];
}
