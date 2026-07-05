# Compatibility Matrix

This document tracks what has been validated with OpenSTKPConnector and what still needs community testing.

## Platform Compatibility

| Area | Status | Notes |
| --- | --- | --- |
| X-Plane 12 on Apple Silicon, native `arm64` mode | Validated | Plugin loads and SimToolkitPro tracks the aircraft. |
| macOS Universal Binary | Validated | Release builds include both `arm64` and `x86_64` slices. |
| SimToolkitPro connection | Validated | SimToolkitPro connects to `127.0.0.1:51303`. |
| X-Plane 12 under Rosetta | Validated | Verified with X-Plane running as Intel 64-bit and loading the `x86_64` slice of the Universal Binary. |
| Intel Mac, native `x86_64` mode | Expected, needs physical Intel Mac validation | Rosetta validation confirms the `x86_64` plugin slice works, but real Intel Mac hardware testing is still needed. |

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
| SimToolkitPro reconnect while X-Plane remains open | Validated | Verified by closing and reopening SimToolkitPro during an active X-Plane session; a new client received the protocol greeting and subscriptions. |
| SimToolkitPro startup update warning | Known behavior | SimToolkitPro may show this before X-Plane starts because it validates the official plugin package on disk. Tracking can still work. |
| Manual flight ending after closing X-Plane | Matches original connector | The original STKPConnector shows the same behavior; end the flight manually from the SimToolkitPro recorder. |

## Testing Priorities

- Intel Mac validation.
- Additional default and third-party aircraft.
- Longer flights with cruise, descent, landing report, and screenshots.

## v1.0 Readiness Checklist

### Already Validated

- Native Apple Silicon X-Plane 12 plugin loading.
- SimToolkitPro connection on `127.0.0.1:51303`.
- Aircraft tracking on the SimToolkitPro live map.
- Complete flight log recording.
- Landing report saving.
- Screenshot association.
- Universal macOS build output with `arm64` and `x86_64` slices.
- Release packaging with SHA256 and Markdown release notes.
- Local release readiness script.

### Required Before v1.0

- At least one Intel Mac validation report, or a clearly documented reason why physical Intel validation remains community-provided.
- At least one additional aircraft validation beyond the current LevelUp 737-800NG and Laminar C172 tests, following [`TEST_PLAN.md`](TEST_PLAN.md).
- One final full-flight test with the latest pre-`v1.0` build, following [`TEST_PLAN.md`](TEST_PLAN.md).

### Post-v1.0 Nice-To-Have

- Configurable diagnostic verbosity.
- Further DataRef send-policy tuning if real-world tests show a concrete need.
- More aircraft-specific validation reports.
- Optional CI automation for source-only checks.
