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

  # some issues with -Wformat-security and whatnot
  # not willing to look into the specifics, so "all" for now.
  hardeningDisable = [ "all" ];

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
      ./Install_step_1
      ./Install_step_2
      ./Install_step_3
      ./Install_step_4
    # BUG(cd): docs are broken for now
    # ./Install_step_5
      cd ..
  '';

  postFixup = ''
    find $out/bin -maxdepth 1 -executable -type f |
      while read file; do
        wrapProgram "$file" --set TIGCC $out
      done
  '';

}
