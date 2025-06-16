{ bison
, fetchFromGitHub
, flex
, glib
, gnome2
, gtk2
, libjpeg
, ncurses
, pkg-config
, SDL
, gcc10Stdenv
, tiLibraries
, tiTools
, xorg
, ...
}:

gcc10Stdenv.mkDerivation rec {
  pname = "tiemu";
  version = "3.02";

  # NOTE(cd): v3.02, buggy
  src = fetchTarball {
    url = "http://www.calcforge.org:70/downloads/tiemu-3.02a/tiemu-3.02.tar.bz2";
    sha256 = "sha256:1bhhqq4qdgx0vdzp80palkham1qi1k1mnchsc11rq6vfyp0gwjcb";
  };

  nativeBuildInputs = [
    pkg-config
  ];

  buildInputs = [
    bison
    flex

    glib
    gtk2
    gnome2.libglade
    libjpeg
    ncurses
    SDL
    xorg.libXt

    tiLibraries.libticables2
    tiLibraries.libticalcs2
    tiLibraries.libticonv
    tiLibraries.libtifiles2

    tiTools.texinfo4
  ];

  patches = [
    ./tiemu-3.03-fix-ftbfs-with-customized-abort-function.patch
  ];

  configureFlags = [
    "--disable-gdb"
    "--without-kde"
  ];

  # Needed for src/gdb/readline.
  dontDisableStatic = true;

  hardeningDisable = [ "all" ];

  NIX_CFLAGS_COMPILE = [
    "-Wno-error"
    "-UGTK_DISABLE_DEPRECATED"
  ];
}
