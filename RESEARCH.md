# Research and Reverse Engineering

## Initial Problem

SimToolkitPro 1.3.6 with the original `stkpconnector` plugin works with X-Plane 12 on Apple Silicon Macs only when X-Plane is launched through Rosetta.

In native Apple Silicon mode, X-Plane cannot load the original plugin and SimToolkitPro tracking does not work.

## Original Plugin Analysis

The original `mac.xpl` plugin binary is Intel-only (`x86_64`).

Expected result from `file` or `lipo -info`:

```text
Mach-O 64-bit x86_64 dynamically linked shared library
```

The bundled Qt libraries are Intel-only as well, especially:

- `QtCore.framework`
- `QtNetwork.framework`

No `arm64` or Universal Binary was found.

## Technical Consequence

An `arm64` X-Plane process cannot load an `x86_64` plugin.

Rosetta translates the whole process, not individual plugins:

- X-Plane through Rosetta -> `x86_64` process -> Intel plugin can load;
- native Apple Silicon X-Plane -> `arm64` process -> Intel plugin cannot load.

## SimToolkitPro Network Mode

Network Mode does not remove the need for an X-Plane plugin.

It lets SimToolkitPro connect to a host and port, but the TCP server still needs to be provided by an X-Plane-side connector.

## Wireshark

Captures were made on the loopback interface `lo0`.

Key findings:

- local TCP conversation on port `51303`;
- readable ASCII stream;
- the original plugin opens the TCP server;
- the plugin sends an STKP-compatible text greeting;
- the greeting contains `STKPCONNECT 1`, `STKPCONNECT-VERSION 2020`, and `sub <dataref>` lines;
- after the greeting, the plugin sends an initial DataRef snapshot and then periodic DataRef updates.

Published reference files:

- `docs/Wireshark GoldenFlight/GoldenFlight.pcapng`
- `docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt`
- `docs/Wireshark GoldenFlight/Conversations.png`
- `docs/Wireshark GoldenFlight/Endpoints.png`

## `lsof`

Observed output confirms:

- `X-Plane` listens on TCP port `51303`;
- `SimToolkitPro` connects from a local ephemeral port to `127.0.0.1:51303`.

This confirms that the plugin is the server and SimToolkitPro is the client.

## Functional Test Result

Native Apple Silicon testing confirmed that SimToolkitPro displays the aircraft on the map when OpenSTKPConnector sends the STKP greeting before the DataRef stream.

Basic flight tracking was verified during a short test flight. Automatic flight ending in SimToolkitPro after X-Plane shutdown still needs further analysis.

## Conclusion

It is realistic to build a clean-room replacement plugin that emulates the observed STKPConnector protocol without using the original source code.
