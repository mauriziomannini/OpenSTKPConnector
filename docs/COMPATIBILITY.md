# Compatibility Matrix

This document tracks what has been validated with OpenSTKPConnector and what still needs community testing.

## Platform Compatibility

| Area | Status | Notes |
| --- | --- | --- |
| X-Plane 12 on Apple Silicon, native `arm64` mode | Validated | Plugin loads and SimToolkitPro tracks the aircraft. |
| macOS Universal Binary | Validated | Release builds include both `arm64` and `x86_64` slices. |
| SimToolkitPro connection | Validated | SimToolkitPro connects to `127.0.0.1:51303`. |
| Intel Mac, native `x86_64` mode | Needs testing | The binary contains the `x86_64` slice, but real Intel Mac testing is still needed. |
| X-Plane 12 under Rosetta | Needs testing | Expected to work through the Universal Binary, but full OpenSTKPConnector validation is still needed. |

## Aircraft Validation

| Aircraft | Route | Status | Notes |
| --- | --- | --- | --- |
| LevelUp 737-800NG | KEEN -> KDCA | Validated | Full cold and dark flight recorded successfully, including landing report and screenshots. |
| Laminar Cessna C172 | LIRP -> LIRQ | Validated | Full flight recorded successfully. |
| Other aircraft | Various | Needs testing | Community reports are welcome. |

## SimToolkitPro Behavior

| Behavior | Status | Notes |
| --- | --- | --- |
| Aircraft appears on the live map | Validated | Verified with multiple aircraft. |
| Flight tracking and log saving | Validated | Verified during complete flights. |
| Landing report saving | Validated | Verified after touchdown tests and complete flights. |
| Screenshot association | Validated | Verified with SimToolkitPro flight logs. |
| SimToolkitPro startup update warning | Known behavior | SimToolkitPro may show this before X-Plane starts because it validates the official plugin package on disk. Tracking can still work. |
| Manual flight ending after closing X-Plane | Matches original connector | The original STKPConnector shows the same behavior; end the flight manually from the SimToolkitPro recorder. |

## Testing Priorities

- Intel Mac validation.
- X-Plane 12 under Rosetta with OpenSTKPConnector.
- Additional default and third-party aircraft.
- Longer flights with cruise, descent, landing report, and screenshots.
- Reconnect behavior when SimToolkitPro is restarted while X-Plane remains open.
