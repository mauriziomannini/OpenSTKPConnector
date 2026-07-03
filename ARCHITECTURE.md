# OpenSTKPConnector Architecture

## Goal

Replace the original Intel-only `STKPConnector/mac.xpl` with a new plugin that can be built for Apple Silicon (`arm64`) and as a Universal Binary (`arm64;x86_64`) while remaining compatible with SimToolkitPro.

## High-Level Architecture

```text
Native X-Plane 12
        |
        | X-Plane SDK / DataRefs
        v
OpenSTKPConnector.xpl
        |
        | STKP greeting + DataRef stream over TCP 127.0.0.1:51303
        v
SimToolkitPro client
```

## Components

### Plugin Entrypoint

File: `src/plugin.cpp`

Responsibilities:

- implement `XPluginStart`, `XPluginStop`, `XPluginEnable`, `XPluginDisable`;
- initialize the DataRef manager;
- start the TCP server;
- register the flight loop callback;
- broadcast DataRef frames from the simulator thread.

### TCP Server

File: `src/tcp_server.cpp`

Responsibilities:

- open a TCP socket on `127.0.0.1:51303`;
- accept SimToolkitPro client connections;
- send the STKP protocol greeting immediately after accept;
- send text payloads to connected clients;
- read client input opportunistically without blocking the flight loop;
- remove disconnected clients;
- tolerate closed sockets and avoid `SIGPIPE` crashes.

The server keeps a small client list with numeric ids for readable logging.

### Protocol

Files:

- `include/protocol.h`
- `src/protocol.cpp`

Responsibilities:

- define the STKP compatibility greeting;
- keep the ordered list of DataRefs advertised with `sub <dataref>` lines;
- preserve protocol details outside the TCP server implementation.

The TCP server only asks this module to build the greeting. This keeps socket handling separate from STKP protocol content.

### DataRef Manager

File: `src/datarefs.cpp`

Responsibilities:

- define the DataRefs exported to SimToolkitPro;
- resolve DataRefs with `XPLMFindDataRef`;
- read scalar, array, and byte/string DataRef values;
- serialize values into the STKP-compatible ASCII format;
- throttle low-frequency DataRefs where appropriate;
- produce a full snapshot when requested after a new client connection.

### Logger

File: `src/logger.cpp`

Responsibilities:

- write diagnostic messages to X-Plane's `Log.txt` through `XPLMDebugString`.

## Send Frequency

The current flight loop callback runs every `0.05s`, approximately 20 Hz.

High-frequency flight position values are sent every frame. Lower-frequency or mostly static values are throttled and forced into the initial snapshot.

## Plugin Output

The binary must be named:

```text
mac.xpl
```

For SimToolkitPro compatibility, install it as:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

The `dist/stkpconnector/mac.xpl` output is prepared specifically for this layout.
