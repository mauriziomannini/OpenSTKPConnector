# Changelog

## Unreleased - v0.6-dev

### Changed

- Opened the next development cycle after `v0.5.0`.
- Updated the development roadmap for the current project state.
- Added a defensive TCP input buffer limit for malformed client input.
- Documented a successful full cold and dark flight test from KEEN to KDCA with the LevelUp 737-800NG.
- Updated diagnostic log examples to match the current DataRef count.
- Documented the intentional duplicate `sim/aircraft/parts/acf_gear_deploy` subscription in the STKP greeting.

## v0.5.0 - Diagnostic and Build Output Cleanup

### Changed

- Added plugin version logging at startup to make `X-Plane 12/Log.txt` checks clearer during development and testing.
- Added TCP subscription diagnostics to make SimToolkitPro connection debugging easier.
- Added DataRef initialization summary logging.
- Updated manual testing documentation with current diagnostic log examples.
- Made TCP disconnect diagnostics consistent when the oldest client is dropped.
- Simplified build output to only create the SimToolkitPro-compatible `dist/stkpconnector/mac.xpl` path.

## v0.4.0 - Protocol and Packaging Cleanup

### Added

- Release packaging script:
  - builds the Universal Binary;
  - verifies `x86_64` and `arm64`;
  - creates the release folder and ZIP;
  - prints the ZIP SHA256.
- Separate protocol module for the STKP greeting and advertised DataRef subscription list.

### Changed

- Moved STKP greeting construction out of the TCP server.
- Named DataRef send-policy constants to make update frequencies easier to understand.
- Added inline DataRef group documentation.
- Updated architecture documentation for the protocol module.
- Improved GitHub release note formatting.

### Verified

- SimToolkitPro aircraft tracking still works after the protocol and DataRef refactors.
- Universal Binary build remains valid (`x86_64` + `arm64`).

## v0.3.1 - Touchdown Reporting Stabilization

### Changed

- Increased update frequency for touchdown-critical DataRefs:
  - `sim/flightmodel/failures/onground_all`
  - `sim/flightmodel/failures/onground_any`
  - `sim/flightmodel2/gear/on_ground`
- Documented the SimToolkitPro startup warning shown when it validates the official plugin package on disk.
- Documented that manual flight ending after X-Plane shutdown matches the original STKPConnector behavior.

### Verified

- SimToolkitPro aircraft tracking still works.
- SimToolkitPro landing report is saved successfully.
- Universal Binary build remains valid (`x86_64` + `arm64`).

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

- SimToolkitPro may require the flight to be ended manually from the flight log recorder after X-Plane shutdown; this matches the original STKPConnector behavior.
- Release packaging is still manual.
