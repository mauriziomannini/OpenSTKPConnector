# OpenSTKPConnector

Plugin X-Plane 12 per macOS Apple Silicon pensato per sostituire il plugin STKPConnector Intel-only usato da SimToolkitPro.

## Perché esiste

Su Mac Apple Silicon, il plugin originale `stkpconnector/mac.xpl` funziona solo avviando X-Plane 12 in Rosetta, perché il binario è solo `x86_64`. X-Plane nativo `arm64` non può caricare plugin Intel.

Questo progetto mira a ricreare il comportamento minimo del connector:

- plugin nativo `arm64`/Universal;
- TCP server su `127.0.0.1:51303`;
- invio DataRef X-Plane in formato ASCII compatibile con SimToolkitPro.

## Stato del progetto

Questo repository è uno scheletro tecnico con documentazione di reverse engineering già inclusa.

La parte più importante da leggere prima di sviluppare è:

- [`PROTOCOL.md`](PROTOCOL.md)
- [`ARCHITECTURE.md`](ARCHITECTURE.md)
- [`RESEARCH.md`](RESEARCH.md)
- [`DEVELOPMENT.md`](DEVELOPMENT.md)
- [`CODEX_BRIEF.md`](CODEX_BRIEF.md)

## Materiale incluso

```text
docs/Wireshark/stream.txt                  flusso TCP ASCII esportato
docs/Wireshark/stkp_capture.pcapng         cattura Wireshark completa
docs/Wireshark/screenshots/                screenshot analisi Wireshark/lsof
docs/OriginalPlugin/stkpconnector_original.zip  plugin originale Intel-only
```

## Requisiti

- macOS Apple Silicon
- Xcode Command Line Tools
- CMake
- Ninja consigliato
- Visual Studio Code
- Estensioni VS Code:
  - Microsoft C/C++
  - CMake Tools
- X-Plane SDK

## X-Plane SDK

Imposta la variabile:

```bash
export XPLANE_SDK=$HOME/Development/XPSDK
```

La struttura attesa è:

```text
$XPLANE_SDK/CHeaders/XPLM/XPLMPlugin.h
$XPLANE_SDK/CHeaders/XPLM/XPLMDataAccess.h
$XPLANE_SDK/CHeaders/XPLM/XPLMProcessing.h
$XPLANE_SDK/CHeaders/XPLM/XPLMUtilities.h
```

## Build

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

Output atteso:

```text
build/mac.xpl
```

## Test rapido

Con X-Plane aperto:

```bash
lsof -nP -iTCP:51303
nc 127.0.0.1 51303
```

## Protocollo sintetico

Formato osservato:

```text
uf sim/flightmodel/position/groundspeed 0.0003045624471
ud sim/flightmodel/position/latitude 43.81456867
ufa sim/flightmodel2/engines/N2_percent [18.2262,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
```

Vedi [`PROTOCOL.md`](PROTOCOL.md) per dettagli.
