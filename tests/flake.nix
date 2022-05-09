{
  description = "automato-tests";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pname = "automato-tests";
        pkgs = nixpkgs.legacyPackages."${system}";
      in
        rec {
          inherit pname;

          # `nix develop`
          devShell = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              gcc
              gnumake
            ];
          };
        }
    );
}

