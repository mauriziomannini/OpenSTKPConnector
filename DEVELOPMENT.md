# Guida sviluppo

## Ambiente consigliato

- macOS Apple Silicon
- Visual Studio Code
- Estensione Microsoft C/C++
- Estensione CMake Tools
- Xcode Command Line Tools
- CMake
- Ninja
- X-Plane SDK

## Installazione strumenti

```bash
xcode-select --install
brew install cmake ninja
```

## X-Plane SDK

Scaricare l'X-Plane SDK e impostare:

```bash
export XPLANE_SDK=$HOME/Development/XPSDK
```

La cartella attesa è:

```text
$XPLANE_SDK/CHeaders/XPLM/XPLMPlugin.h
$XPLANE_SDK/CHeaders/XPLM/XPLMDataAccess.h
$XPLANE_SDK/CHeaders/XPLM/XPLMProcessing.h
$XPLANE_SDK/CHeaders/XPLM/XPLMUtilities.h
```

Se la struttura è diversa, aggiornare `CMakeLists.txt`.

## Build terminale

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

Build Universal:

```bash
cmake -S . -B build-universal -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build-universal
```

## Test porta

Con X-Plane aperto:

```bash
lsof -nP -iTCP:51303
```

Output atteso:

```text
X-Plane ... TCP 127.0.0.1:51303 (LISTEN)
```

## Test manuale senza SimToolkitPro

```bash
nc 127.0.0.1 51303
```

Dovresti vedere righe tipo:

```text
uf sim/flightmodel/position/groundspeed 0
ud sim/flightmodel/position/latitude 43.81456867
```

## Installazione plugin

Copiare `mac.xpl` in:

```text
X-Plane 12/Resources/plugins/OpenSTKPConnector/mac.xpl
```

Controllare `X-Plane 12/Log.txt` per messaggi `[OpenSTKPConnector]`.

## Roadmap suggerita per Codex

1. Verificare che il progetto compili con X-Plane SDK reale.
2. Correggere include path SDK se necessario.
3. Confermare caricamento plugin in X-Plane nativo Apple Silicon.
4. Confermare `LISTEN` su porta `51303`.
5. Testare connessione con `nc`.
6. Avviare SimToolkitPro in Network Mode su `127.0.0.1:51303`.
7. Aggiungere DataRef mancanti osservati nel file `stream.txt`.
8. Aggiungere logging configurabile.
9. Migliorare gestione client multipli/disconnessioni.
10. Preparare release Universal.
