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
dist/OpenSTKPConnector/mac.xpl
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

The `POST_BUILD` step always copies the latest build output to `dist/OpenSTKPConnector/mac.xpl` and `dist/stkpconnector/mac.xpl`. After a Universal build, the files in `dist/` are Universal too.

## Release Packaging

Use the packaging script to create a Universal release ZIP:

```bash
scripts/package_release.sh v0.x.y
```

The script:

- configures and builds `build-universal`;
- verifies that `mac.xpl` contains both `x86_64` and `arm64`;
- creates `release/OpenSTKPConnector-<version>-mac-universal/`;
- creates `release/OpenSTKPConnector-<version>-mac-universal.zip`;
- prints the ZIP SHA256.

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

## Current Development Status

Verified:

- plugin loads in native Apple Silicon X-Plane 12;
- TCP server listens on `127.0.0.1:51303`;
- SimToolkitPro displays the aircraft on the map;
- basic tracking works during a short flight;
- SimToolkitPro landing report is saved successfully;
- protocol and DataRef refactors were verified with SimToolkitPro;
- startup logs include the current development version;
- Universal Binary build works.

Known behavior:

- SimToolkitPro may show an update-required warning on startup because it validates the official plugin package on disk;
- SimToolkitPro requires manual flight ending from the flight log recorder after X-Plane shutdown, matching the original plugin behavior.

To investigate:

- more precise handling of client reconnects;
- possible DataRef set reduction after more real-world tests.

## Suggested Roadmap

1. Keep v0.3.x stable and reproducible.
2. Improve client connection handling if needed.
3. Refine DataRef grouping and send frequency policy.
4. Add configurable logging if needed.
5. Keep release packaging reproducible.
