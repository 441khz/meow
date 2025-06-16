{ gcc9Stdenv
, fetchFromGitHub
, makeWrapper
}:

gcc9Stdenv.mkDerivation {
  pname = "gcc4ti";
  version = "0.96b11";

  src = fetchFromGitHub {
    owner = "debrouxl";
    repo = "gcc4ti";
    rev = "4cb128b";
    hash = "sha256-yIP6Oo3Fmtye9B4v5vuskkKwCZCGqg01ZBeuFuWy0yo=";
  };

  nativeBuildInputs = [
    makeWrapper
  ];

  # very important for tigcc.a not to be mangled
  dontStrip = true;

  buildPhase = ''
    # extract a bunch of docs and sources
    cd trunk/tigcc-linux/scripts
    ./updatesrc
  '';

  installPhase = ''
    # the build and install phases are a little muddled
    # i need $out during build time, so building in installPhase
    # see gcc4ti/#13

    export TIGCCDIR="$out"
    export PREFIX_GCC4TI="$out"

    cd ../gcc4ti-0.96b11/scripts
    ./Install_All
    cd ..

    # HACK(bin): fixup symlinks as best as possible
    pushd .
    cd $out/doc/

    # bad symlinks
    rm index.html
    rm tigcclib

    ln -s html/index.html index.html
    ln -s html/ tigcclib

    popd
  '';

  postFixup = ''
    find $out/bin -maxdepth 1 -executable -type f |
      while read file; do
        wrapProgram "$file" --set TIGCC $out
      done
  '';

  hardeningDisable = [ "all" ];
}
