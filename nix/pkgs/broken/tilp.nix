{ stdenv
, fetchFromGitHub
, libticables2
, libticonv
, libtifiles2
, libticalcs2
, libusb1
, gtk2
, cmake
, ninja
, pkg-config
, autoconf
, automake
, libtool
, intltool
, bison
, flex
, libarchive
, texinfo
, gettext
, gnome2
, zlib
}:

stdenv.mkDerivation {
  pname = "tilp";
  version = "1.18";

  src = fetchFromGitHub {
    owner = "debrouxl";
    repo = "tilp_and_gfm";
    rev = "0a52561";
    hash = "sha256-itPop4GO6ty40iucg31YgHEoK0W05eM+rD9tF+nAugE=";
  };

  buildPhase = ''
    cd tilp/trunk
    autoreconf -i -f
  '';

  installPhase = ''
    # we must build in the install phase because tilp wants $out early
    # would preinstallphase expose $out ?
    ./configure --prefix=$out
    make
    make install
  '';

  hardeningDisable = [ "all" ];

  nativeBuildInputs = [
    pkg-config
    autoconf
    automake
    libtool
    intltool
  ];

  buildInputs = [
    gnome2.libglade
    libarchive
    libticables2
    libtifiles2
    libticalcs2
    libticonv
    gtk2
    libusb1
    libtool
    intltool
    zlib
  ];
}
