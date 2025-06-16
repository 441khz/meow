{ fetchFromGitHub
, glib
, pkg-config
, stdenv
, tiLibraries
, ...
}:

stdenv.mkDerivation rec {
  pname = "libticables2";
  version = "1.2.0";

  src = fetchTarball {
    url = "http://www.calcforge.org:70/downloads/tiemu-3.02a/${pname}-${version}.tar.bz2";
    sha256 = "sha256:0kd8kinc9ax0h7872nha2whp8p0ldc66888vmkl25k5a43byq2lr";
  };

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    glib

    tiLibraries.libusb
  ];
}
