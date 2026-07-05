# Suggested Community Post

## Discord Short Version

```text
Hi everyone,

I’m working on OpenSTKPConnector, an open-source macOS X-Plane 12 plugin that acts as a SimToolkitPro-compatible replacement for the original STKPConnector plugin.

The goal is mainly Apple Silicon support: the original connector is Intel-only, so it does not load when X-Plane 12 runs natively on Apple Silicon. OpenSTKPConnector is built as a Universal macOS plugin (arm64 + x86_64).

GitHub:
https://github.com/mauriziomannini/OpenSTKPConnector

Latest release:
https://github.com/mauriziomannini/OpenSTKPConnector/releases/tag/v0.8.0

Current status:
- X-Plane 12 native Apple Silicon loads correctly
- SimToolkitPro connects and tracks the aircraft
- Landing reports work
- Screenshot association works
- Full cold & dark test flight KEEN -> KDCA with LevelUp 737-800NG completed successfully
- Full flight LIRP -> LIRQ with Laminar Cessna C172 completed successfully

Install:
Copy the included stkpconnector folder to:

X-Plane 12/Resources/plugins/

Final path:

X-Plane 12/Resources/plugins/stkpconnector/mac.xpl

Notes:
This is a public beta/community preview and is not affiliated with or supported by SimToolkitPro.

Testing and feedback are welcome, especially from Apple Silicon, Intel Mac, Rosetta, and different aircraft setups.
```

## Suggested Title

OpenSTKPConnector - native Apple Silicon replacement for the SimToolkitPro X-Plane connector

## Post Text

Hi everyone,

I would like to share a new open-source project called OpenSTKPConnector:

https://github.com/mauriziomannini/OpenSTKPConnector

OpenSTKPConnector is a macOS X-Plane 12 plugin intended to act as a SimToolkitPro-compatible replacement for the original STKPConnector plugin.

The main reason for the project is Apple Silicon compatibility. The original SimToolkitPro X-Plane connector is currently available as an Intel-only `x86_64` plugin, so it does not load when X-Plane 12 is running natively on Apple Silicon. OpenSTKPConnector is written from scratch and provides a Universal macOS plugin binary with both `arm64` and `x86_64` support.

Current release:

https://github.com/mauriziomannini/OpenSTKPConnector/releases/tag/v0.8.0

Current status:

- X-Plane 12 native Apple Silicon loading works.
- SimToolkitPro connects to the plugin.
- Aircraft tracking on the SimToolkitPro map works.
- Landing report saving works.
- Screenshot association works.
- A full cold and dark flight from KEEN to KDCA with the LevelUp 737-800NG was tracked and recorded successfully.
- A full flight from LIRP to LIRQ with the Laminar Cessna C172 was tracked and recorded successfully.
- The release ZIP contains a Universal macOS `mac.xpl` binary.

Installation summary:

1. Close X-Plane 12.
2. Download the latest release ZIP from GitHub.
3. Copy the included `stkpconnector` folder to:

   `X-Plane 12/Resources/plugins/`

4. The final path should be:

   `X-Plane 12/Resources/plugins/stkpconnector/mac.xpl`

5. Start X-Plane 12 and SimToolkitPro.

Important notes:

- This is a public beta / community preview.
- This project is not affiliated with, endorsed by, or supported by SimToolkitPro.
- SimToolkitPro may still show an "X-Plane Plugin update is required" warning when SimToolkitPro starts. This appears to be related to SimToolkitPro validating the official plugin package on disk. Tracking can still work once X-Plane loads OpenSTKPConnector.
- Ending a flight manually from the SimToolkitPro flight log recorder after closing X-Plane matches the behavior observed with the original connector.

I would appreciate testing and feedback, especially from:

- Apple Silicon Mac users;
- Intel Mac users;
- X-Plane 12 users running under Rosetta;
- different aircraft types;
- full flights with landing reports;
- users who can compare behavior with the original connector.

Issues and test reports are welcome on GitHub:

https://github.com/mauriziomannini/OpenSTKPConnector/issues

Thanks.
