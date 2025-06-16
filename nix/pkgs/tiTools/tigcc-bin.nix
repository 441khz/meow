{ autoPatchelfHook
, fetchzip
, makeWrapper
, stdenv
, ...
}:

stdenv.mkDerivation {
  pname = "tigcc-bin";
  version = "0.96b8-r1";

  src = fetchzip {
    url = "http://tigcc.ticalc.org/linux/tigcc_bin.tar.bz2";
    hash = "sha256-FOW8uI+aGtxEQCwqw77yQ50AaiHDdZQ3bDz8lz9jTK0=";
    stripRoot = false;
  };

  nativeBuildInputs = [
    autoPatchelfHook
    makeWrapper
  ];

  dontStrip = true;

  installPhase = ''
    mkdir $out/

    cp -r bin/ $out/bin/
    cp -r doc/ $out/doc/
    cp -r include/ $out/include/
    cp -r lib $out/lib/
  '';

  postFixup = ''
    find $out/bin -maxdepth 1 -executable -type f |
      while read file; do
        wrapProgram "$file" --set TIGCC $out
      done
  '';
}
