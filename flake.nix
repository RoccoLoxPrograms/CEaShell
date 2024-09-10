{
  description = "CEaShell (pronounced like \"Seashell\") is a new shell for the TI-84 Plus CE.";
  inputs = {
    toolchain.url = "github:myclevorname/flake";
    flake-utils.url = "github:numtide/flake-utils";
    app_tools = {
      url = "github:tiny-hacker/app_tools";
      flake = false;
    };
  };

  outputs = { self, toolchain, app_tools, flake-utils }:
    flake-utils.eachSystem [ "x86_64-linux" "x86_64-darwin" ] (system: {
      packages.default = toolchain.packages.x86_64-linux.mkDerivation {
        pname = "CEaShell";
        version = "0.0.1";
        postUnpack = ''
          cp -r ${app_tools}/ $sourceRoot/app_tools
          chmod -R +w $sourceRoot/app_tools
        '';
        src = self;
      };
    });
}
