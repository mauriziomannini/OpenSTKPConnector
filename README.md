# OpenSTKPConnector

OpenSTKPConnector is an open-source X-Plane 12 plugin for macOS that provides a SimToolkitPro-compatible replacement for the original Intel-only `STKPConnector` plugin.

## Why This Exists

On Apple Silicon Macs, the original SimToolkitPro `stkpconnector/mac.xpl` only works when X-Plane 12 is launched through Rosetta, because the original plugin binary is `x86_64` only. Native `arm64` X-Plane cannot load Intel-only plugins.

This project recreates the connector behavior from scratch:

- native Apple Silicon support;
- optional Universal Binary support (`arm64` + `x86_64`);
- TCP server on `127.0.0.1:51303`;
- ASCII DataRef stream compatible with SimToolkitPro.

## Current Status

Current internal version: `v0.7.0`.

OpenSTKPConnector is currently a public beta / community preview. It is not affiliated with, endorsed by, or supported by SimToolkitPro.

Verified:

- X-Plane 12 loads the plugin in native Apple Silicon mode;
- the plugin opens a TCP server on `127.0.0.1:51303`;
- SimToolkitPro connects and displays the aircraft on the map;
- basic aircraft position and flight tracking were verified during a short flight;
- SimToolkitPro landing report was verified after a short landing test;
- a full cold and dark flight from KEEN to KDCA with the LevelUp 737-800NG was tracked and recorded correctly, including landing report and associated screenshots;
- STKP protocol and DataRef refactors were verified with SimToolkitPro;
- Universal Binary build was verified with `lipo`.

Community testing is welcome, especially with:

- other Apple Silicon Macs;
- Intel Macs;
- X-Plane 12 under Rosetta;
- additional aircraft;
- longer full-flight profiles.

Known limitation:

- SimToolkitPro requires the flight to be ended manually from the flight log recorder after X-Plane is closed. This matches the behavior observed with the original STKPConnector plugin.
- SimToolkitPro may show an "X-Plane Plugin update is required" warning when SimToolkitPro starts, even before X-Plane is launched. This appears to be caused by SimToolkitPro validating the official plugin package on disk, not by the OpenSTKPConnector TCP protocol. The warning does not necessarily indicate a tracking failure; tracking can still work once X-Plane loads OpenSTKPConnector.

Recommended reading before development:

- [`PROTOCOL.md`](PROTOCOL.md)
- [`ARCHITECTURE.md`](ARCHITECTURE.md)
- [`RESEARCH.md`](RESEARCH.md)
- [`DEVELOPMENT.md`](DEVELOPMENT.md)
- [`CODEX_BRIEF.md`](CODEX_BRIEF.md)

## Included Research Material

```text
docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt  full exported TCP stream
docs/Wireshark GoldenFlight/GoldenFlight.pcapng      full Wireshark capture
docs/Wireshark GoldenFlight/*.png                    Wireshark analysis screenshots
```

The original plugin archive may be kept locally under `docs/OriginalPlugin/`, but it is intentionally ignored and not published.

## Requirements

- macOS
- X-Plane 12
- SimToolkitPro

For users installing a release ZIP, no build tools are required.

For developers building from source:

- Xcode Command Line Tools
- CMake
- Ninja recommended
- Visual Studio Code recommended
- X-Plane SDK

## Download

Download the latest release ZIP from:

```text
https://github.com/mauriziomannini/OpenSTKPConnector/releases
```

The release asset is named like:

```text
OpenSTKPConnector-vX.Y.Z-mac-universal.zip
```

The ZIP contains a ready-to-install `stkpconnector` folder.

## Installation From Release ZIP

1. Close X-Plane 12.
2. Download and unzip the latest release asset.
3. Copy the included `stkpconnector` folder to:

```text
X-Plane 12/Resources/plugins/
```

The final path must be:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

SimToolkitPro checks for the original plugin folder name, so the folder must be named `stkpconnector`.

## Updating From A Previous Version

1. Close X-Plane 12.
2. Replace the existing:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

with the new `mac.xpl` from the release ZIP.

If you prefer, you can replace the whole `stkpconnector` folder with the one from the ZIP.

## Verifying Installation

When X-Plane starts, `X-Plane 12/Log.txt` should contain lines similar to:

```text
[OpenSTKPConnector] starting v0.7.0
[OpenSTKPConnector] DataRefs initialized: 34; found: 34; missing: 0
[OpenSTKPConnector] TCP server listening on 127.0.0.1:51303
[OpenSTKPConnector] client #1 connected; active clients: 1
[OpenSTKPConnector] client #1 protocol greeting sent
[OpenSTKPConnector] client #1 first subscription received: sim/time/paused
[OpenSTKPConnector] client #1 subscriptions received: 25
```

## Quick Test

With X-Plane running:

```bash
lsof -nP -iTCP:51303
nc 127.0.0.1 51303
```

## Troubleshooting

### SimToolkitPro shows "X-Plane Plugin update is required"

SimToolkitPro may show this warning when it starts, even before X-Plane is launched. This appears to be related to SimToolkitPro validating the official plugin package on disk.

This warning does not necessarily mean OpenSTKPConnector failed. Start X-Plane and check whether SimToolkitPro connects and tracks the aircraft.

### Aircraft does not appear in SimToolkitPro

Check `X-Plane 12/Log.txt` for:

```text
[OpenSTKPConnector] TCP server listening on 127.0.0.1:51303
[OpenSTKPConnector] client #1 connected; active clients: 1
[OpenSTKPConnector] client #1 protocol greeting sent
[OpenSTKPConnector] client #1 first subscription received: sim/time/paused
```

If these lines are missing, verify that `mac.xpl` is installed at:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

### Flight does not end automatically after closing X-Plane

This matches the behavior observed with the original STKPConnector plugin. End the flight manually from the SimToolkitPro flight log recorder.

## X-Plane SDK

This section is only needed when building from source.

Set:

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

## Build

Apple Silicon build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

Verified Universal build:

```bash
cmake -S . -B build-universal -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-universal
lipo -info build-universal/mac.xpl
```

Expected `lipo` output:

```text
Architectures in the fat file: build-universal/mac.xpl are: x86_64 arm64
```

Expected outputs:

```text
build/mac.xpl
dist/stkpconnector/mac.xpl
```

Note: `dist/stkpconnector/mac.xpl` is updated by the last build that ran. After a Universal build, this file is Universal.

Use this file for SimToolkitPro testing:

```text
dist/stkpconnector/mac.xpl
```

## Protocol Summary

When SimToolkitPro connects, the plugin first sends a compatibility greeting:

```text
STKPCONNECT 1
STKPCONNECT-VERSION 2020
sub sim/time/paused
...
```

It then sends DataRef values as text lines:

```text
uf sim/flightmodel/position/groundspeed 0.0003045624471
ud sim/flightmodel/position/latitude 43.81456867
ufa sim/flightmodel2/engines/N2_percent [18.2262,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
```

See [`PROTOCOL.md`](PROTOCOL.md) for details.
