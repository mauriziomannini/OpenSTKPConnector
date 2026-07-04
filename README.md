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

Current internal version: `v0.5.0`.

Verified:

- X-Plane 12 loads the plugin in native Apple Silicon mode;
- the plugin opens a TCP server on `127.0.0.1:51303`;
- SimToolkitPro connects and displays the aircraft on the map;
- basic aircraft position and flight tracking were verified during a short flight;
- SimToolkitPro landing report was verified after a short landing test;
- STKP protocol and DataRef refactors were verified with SimToolkitPro;
- Universal Binary build was verified with `lipo`.

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
- Xcode Command Line Tools
- CMake
- Ninja recommended
- Visual Studio Code recommended
- X-Plane SDK

## X-Plane SDK

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

## Installation

SimToolkitPro checks for the original plugin folder name. For compatibility, install OpenSTKPConnector as:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

Close X-Plane before replacing `mac.xpl`.

When X-Plane starts, `X-Plane 12/Log.txt` should contain lines similar to:

```text
[OpenSTKPConnector] starting v0.5.0
[OpenSTKPConnector] DataRefs initialized: 33; found: 33; missing: 0
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
