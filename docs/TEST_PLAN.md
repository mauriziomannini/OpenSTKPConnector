# Manual Test Plan

This document defines manual validation steps that cannot be fully covered by local scripts because they depend on X-Plane 12 and SimToolkitPro UI behavior.

## Reconnect Test

Goal: verify that SimToolkitPro can reconnect to OpenSTKPConnector while X-Plane remains running.

Status: validated on `v0.9-dev` during an active X-Plane session. The X-Plane log showed `client #1` disconnecting after SimToolkitPro was closed and `client #2` connecting, receiving the protocol greeting, and sending subscriptions after SimToolkitPro was reopened.

### Prerequisites

- X-Plane 12 is running.
- OpenSTKPConnector is installed at:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

- A flight is loaded and active in X-Plane.
- SimToolkitPro is running and connected to X-Plane.
- The aircraft is visible on the SimToolkitPro live map.

### Baseline Checks

1. Confirm `X-Plane 12/Log.txt` contains:

```text
[OpenSTKPConnector] TCP server listening on 127.0.0.1:51303
[OpenSTKPConnector] client #1 connected; active clients: 1
[OpenSTKPConnector] client #1 protocol greeting sent
```

2. Confirm the TCP server is listening:

```bash
lsof -nP -iTCP:51303
```

Expected result:

```text
X-Plane ... TCP 127.0.0.1:51303 (LISTEN)
```

### Test Steps

1. Keep X-Plane running with the active flight loaded.
2. Close SimToolkitPro only.
3. Wait at least 10 seconds.
4. Reopen SimToolkitPro.
5. Open or return to the SimToolkitPro live map.
6. Confirm the aircraft appears again on the map.

### Success Criteria

- X-Plane remains running throughout the test.
- SimToolkitPro reconnects without restarting X-Plane.
- The aircraft appears again on the SimToolkitPro live map.
- `X-Plane 12/Log.txt` shows a new client connection and protocol greeting after SimToolkitPro restarts.

Useful log lines:

```text
[OpenSTKPConnector] client #N disconnected; reason: ...
[OpenSTKPConnector] client #N connected; active clients: 1
[OpenSTKPConnector] client #N protocol greeting sent
[OpenSTKPConnector] client #N first subscription received: sim/time/paused
```

### Result Template

```text
Date:
OpenSTKPConnector version:
macOS version:
Mac model / CPU:
X-Plane version:
X-Plane mode: native Apple Silicon / Rosetta / Intel
SimToolkitPro version:
Aircraft:
Route or location:
Result: pass / fail
Notes:
```

### Validated Result

```text
Date: 2026-07-05
OpenSTKPConnector version: v0.9-dev
X-Plane mode: native Apple Silicon
Aircraft: Laminar Cessna C172
Result: pass
Notes: SimToolkitPro was closed and reopened while X-Plane remained running. X-Plane Log.txt showed client #1 disconnecting with Connection reset by peer, then client #2 connecting, receiving the protocol greeting, and sending 35 subscriptions.
```

## Full-Flight Pre-v1.0 Test

Goal: verify that the latest pre-`v1.0` build still records a complete flight successfully.

Minimum success criteria:

- SimToolkitPro tracks the aircraft from departure to arrival.
- Flight log is saved.
- Landing report is saved.
- Screenshots are associated when taken by SimToolkitPro.
- Known manual flight ending behavior still matches the original connector.
