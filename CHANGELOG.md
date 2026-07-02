# Changelog

## v0.3.0 - First Functional Baseline

### Added

- Native Apple Silicon X-Plane 12 plugin build.
- TCP server on `127.0.0.1:51303`.
- SimToolkitPro-compatible STKP greeting:
  - `STKPCONNECT 1`
  - `STKPCONNECT-VERSION 2020`
  - `sub <dataref>` lines
- DataRef snapshot and streaming support.
- Scalar, array, and byte/string protocol value types:
  - `uf`
  - `ud`
  - `ui`
  - `ufa`
  - `uia`
  - `ub`
- Base64 encoding for byte/string DataRefs such as aircraft ICAO and description.
- Universal Binary build verification (`x86_64` + `arm64`).
- SimToolkitPro aircraft tracking verified during a short test flight.

### Changed

- The plugin is installed as `Resources/plugins/stkpconnector/mac.xpl` for SimToolkitPro compatibility.
- Documentation was updated for public English-language use.
- The GoldenFlight Wireshark capture is now the canonical protocol reference.

### Known Limitations

- Automatic flight ending in SimToolkitPro after X-Plane shutdown still needs investigation.
- Release packaging is still manual.
