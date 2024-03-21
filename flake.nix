{
  description = "Nix flake for tradesim";

  inputs.nixpkgs.url = github:NixOS/nixpkgs;
  inputs.cppevent = {
    url = github:Manoharan-Ajay-Anand/cppevent;
    inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, cppevent }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
    libcppevent = cppevent.packages.${system}.default;
    outpkgs = self.packages.${system};
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      src = builtins.path {
        path = ./.;
      };
      name = "tradesim-1.0";
      inherit system;
      nativeBuildInputs = [pkgs.cmake libcppevent pkgs.doctest pkgs.nlohmann_json];
      buildInputs = [pkgs.liburing];
      cmakeFlags = [
        "-DCMAKE_INSTALL_SRVDIR=${out}/srv"
      ];
    };
    packages.${system}.sample = pkgs.writeShellApplication {
      name = "tradesim-sample-1.0";
      runtimeInputs = [pkgs.h2o outpkgs.default];
      text = ''
        trap terminate SIGINT
        terminate(){
          pkill -SIGINT -P $$
          exit
        }
        cd ${outpkgs.default}/srv &&
        tradesim &
        h2o -c h2o.conf &
        wait
      '';
    };
    devShells.${system}.default = pkgs.mkShell {
      packages = [pkgs.gdb];
      inputsFrom = [outpkgs.default];
      shellHook = ''
        cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build -S .
      '';
    };
  };
}