{ glib
, pkg-config
, stdenv
, ...
}:

stdenv.mkDerivation rec {
  pname = "libticonv";
  version = "1.1.0";

  src = fetchTarball {
    url = "http://www.calcforge.org:70/downloads/tiemu-3.02a/${pname}-${version}.tar.bz2";
    sha256 = "sha256:159qxh59aschzjsnbp7pxglmq15lr9id56clnfi4z1w832yfv4gn";
  };

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    glib
  ];
}
