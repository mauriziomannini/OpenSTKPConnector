# Development Guide

## Recommended Environment

- macOS
- Visual Studio Code
- Microsoft C/C++ extension
- CMake Tools extension
- Xcode Command Line Tools
- CMake
- Ninja
- X-Plane SDK

## Tool Installation

```bash
xcode-select --install
brew install cmake ninja
```

## X-Plane SDK

Download the X-Plane SDK and set:

```bash
export XPLANE_SDK=$HOME/Development/XPSDK
```

Expected SDK layout:

```text
$XPLANE_SDK/CHeaders/XPLM/XPLMPlugin.h
$XPLANE_SDK/CHeaders/XPLM/XPLMDataAccess.h
$XPLANE_SDK/CHeaders/XPLM/XPLMProcessing.h
$XPLANE_SDK/CHeaders/XPLM/XPLMUtilities.h
```

If your SDK is laid out differently, update `CMakeLists.txt`.

## Terminal Build

Apple Silicon build:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

The build automatically copies the plugin to:

```text
dist/stkpconnector/mac.xpl
```

For SimToolkitPro testing, use:

```text
dist/stkpconnector/mac.xpl
```

Universal build:

```bash
cmake -S . -B build-universal -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-universal
lipo -info build-universal/mac.xpl
```

Expected output:

```text
Architectures in the fat file: build-universal/mac.xpl are: x86_64 arm64
```

The `POST_BUILD` step always copies the latest build output to `dist/stkpconnector/mac.xpl`. After a Universal build, this file is Universal too.

## Release Packaging

Use the packaging script to create a Universal release ZIP:

```bash
scripts/package_release.sh v0.x.y
```

The script:

- requires a release version in `vX.Y.Z` format;
- verifies that `src/plugin.cpp` contains the same `kPluginVersion` before packaging;
- configures and builds `build-universal`;
- verifies that `mac.xpl` contains both `x86_64` and `arm64`;
- creates `release/OpenSTKPConnector-<version>-mac-universal/`;
- creates `release/OpenSTKPConnector-<version>-mac-universal.zip`;
- creates `release/OpenSTKPConnector-<version>-release-notes.md`;
- prints the ZIP SHA256.

Before creating a release package, update:

- `src/plugin.cpp`
- `README.md`
- `DEVELOPMENT.md`
- `CHANGELOG.md`

from the development version, for example `v0.8-dev`, to the final release version, for example `v0.8.0`.

Use the generated Markdown file when creating the GitHub release. This avoids passing release notes as a quoted shell string with visible `\n` sequences:

```bash
gh release create v0.x.y \
  release/OpenSTKPConnector-v0.x.y-mac-universal.zip \
  --title "OpenSTKPConnector v0.x.y" \
  --notes-file release/OpenSTKPConnector-v0.x.y-release-notes.md
```

## Validation Scripts

Run the full local readiness check before tagging or publishing a release:

```bash
scripts/check_release_ready.sh
```

The script builds the Universal plugin, verifies both build and `dist` binaries with `lipo`, checks Bash script syntax, runs the protocol/DataRef consistency check, and executes `git diff --check`.

Check that the protocol greeting does not advertise DataRefs missing from the runtime reader:

```bash
scripts/check_protocol_datarefs.sh
```

The script allows the known GoldenFlight duplicate subscription for `sim/aircraft/parts/acf_gear_deploy`, while still requiring the DataRef to be read only once at runtime.

## Port Test

With X-Plane running:

```bash
lsof -nP -iTCP:51303
```

Expected output:

```text
X-Plane ... TCP 127.0.0.1:51303 (LISTEN)
```

## Manual Test Without SimToolkitPro

```bash
nc 127.0.0.1 51303
```

You should see lines such as:

```text
STKPCONNECT 1
STKPCONNECT-VERSION 2020
sub sim/time/paused
uf sim/flightmodel/position/groundspeed 0
ud sim/flightmodel/position/latitude 43.81456867
```

## Plugin Installation

For SimToolkitPro, copy `dist/stkpconnector/mac.xpl` to:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

The folder must be named `stkpconnector`, because STKP checks for the original plugin path.

Check `X-Plane 12/Log.txt` for `[OpenSTKPConnector]` messages.

## Useful Log Lines

During manual testing, useful `X-Plane 12/Log.txt` lines include:

```text
[OpenSTKPConnector] starting v0.9-dev
[OpenSTKPConnector] DataRefs initialized: 34; found: 34; missing: 0
[OpenSTKPConnector] TCP server listening on 127.0.0.1:51303
[OpenSTKPConnector] client #1 connected; active clients: 1
[OpenSTKPConnector] client #1 protocol greeting sent
[OpenSTKPConnector] client #1 first subscription received: sim/time/paused
[OpenSTKPConnector] client #1 subscriptions received: 25
```

## Current Development Status

Verified:

- plugin loads in native Apple Silicon X-Plane 12;
- TCP server listens on `127.0.0.1:51303`;
- SimToolkitPro displays the aircraft on the map;
- basic tracking works during a short flight;
- SimToolkitPro landing report is saved successfully;
- full cold and dark flight test from KEEN to KDCA with the LevelUp 737-800NG was tracked and recorded successfully, including landing report and associated screenshots;
- full flight test from LIRP to LIRQ with the Laminar Cessna C172 was tracked and recorded successfully;
- protocol and DataRef refactors were verified with SimToolkitPro;
- startup logs include the current development version;
- Universal Binary build works.

Known behavior:

- SimToolkitPro may show an update-required warning on startup because it validates the official plugin package on disk;
- SimToolkitPro requires manual flight ending from the flight log recorder after X-Plane shutdown, matching the original plugin behavior.

To investigate:

- possible DataRef set reduction after more real-world tests.

## Suggested Roadmap

1. Keep `v0.8.0` stable as the current public beta baseline.
2. Use `v0.9-dev` for final validation work before `v1.0`.
3. Gather community validation for Intel Mac and Rosetta setups.
4. Refine DataRef send policy only if more real-world flights show a concrete need.
5. Add configurable logging only if diagnostics become too noisy.
6. Keep release packaging reproducible.
