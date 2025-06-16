{ fetchurl
, ncurses
, stdenv
, ...
}:

stdenv.mkDerivation rec {
  pname = "texinfo";
  version = "4.13a";

  src = fetchurl {
    url = "mirror://gnu/texinfo/texinfo-${version}.tar.lzma";
    hash = "sha256-bSiwzq6GbjU2FC/FUuejvJ+EyDAxGcJXMbJHju9kyeU=";
  };

  buildInputs = [
    ncurses
  ];

  NIX_CFLAGS_COMPILE = [
    "-fpermissive"
  ];
}
