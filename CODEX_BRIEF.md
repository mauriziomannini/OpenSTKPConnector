# Codex Development Brief

If you are working on this repository from VS Code, read these files first:

1. `README.md`
2. `PROTOCOL.md`
3. `ARCHITECTURE.md`
4. `RESEARCH.md`
5. `DEVELOPMENT.md`

## Mission

Implement an X-Plane 12 plugin compatible with SimToolkitPro, replacing the original Intel-only STKPConnector plugin on macOS Apple Silicon.

## Current Status

`v0.3` is the first functional baseline:

- native Apple Silicon X-Plane 12 loads the plugin;
- SimToolkitPro connects to `127.0.0.1:51303`;
- the plugin sends the required STKP greeting;
- SimToolkitPro displays and tracks the aircraft;
- Universal Binary build has been verified.

## Important Constraints

- Do not use Qt.
- Use C++17.
- Use BSD/POSIX sockets.
- Output binary name: `mac.xpl`.
- SimToolkitPro-compatible installation path: `Resources/plugins/stkpconnector/mac.xpl`.
- TCP port: `127.0.0.1:51303`.
- Protocol: ASCII, one line per DataRef value.
- Keep the flight loop non-blocking.
- Accept/client management runs outside the flight loop.
- Broadcast must tolerate disconnected clients.
- Keep the implementation simple and maintainable.

## Protocol Summary

The plugin sends a compatibility greeting immediately after a TCP client connects:

```text
STKPCONNECT 1
STKPCONNECT-VERSION 2020
sub <dataref>
```

Then it sends DataRef values:

```text
uf <dataref> <float>\n
ud <dataref> <double>\n
ui <dataref> <integer>\n
ufa <dataref> [v0,v1,...]\n
uia <dataref> [v0,v1,...]\n
ub <dataref> <base64>\n
```

## Build Checks

Apple Silicon:

```bash
XPLANE_SDK=/Users/maurizio/Development/XPSDK cmake --build build
```

Universal:

```bash
XPLANE_SDK=/Users/maurizio/Development/XPSDK cmake --build build-universal
lipo -info build-universal/mac.xpl
```

Expected Universal output:

```text
Architectures in the fat file: build-universal/mac.xpl are: x86_64 arm64
```

## Reference Material

- Full reference stream: `docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt`
- Full capture: `docs/Wireshark GoldenFlight/GoldenFlight.pcapng`
- Wireshark screenshots: `docs/Wireshark GoldenFlight/*.png`
