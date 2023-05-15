{ pkgs ? import <nixpkgs> {} }:
let
  clang = pkgs.clang;
  ninja = pkgs.ninja;
  cmake = pkgs.cmake;
  json = pkgs.nlohmann_json;
in
pkgs.mkShell {
  buildInputs = [ clang ninja cmake json ];
}