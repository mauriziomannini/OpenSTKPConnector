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

La build copia automaticamente il plugin anche in:

```text
dist/OpenSTKPConnector/mac.xpl
dist/stkpconnector/mac.xpl
```

Per i test con SimToolkitPro usare:

```text
dist/stkpconnector/mac.xpl
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
STKPCONNECT 1
STKPCONNECT-VERSION 2020
sub sim/time/paused
uf sim/flightmodel/position/groundspeed 0
ud sim/flightmodel/position/latitude 43.81456867
```

## Installazione plugin

Per SimToolkitPro, copiare `dist/stkpconnector/mac.xpl` in:

```text
X-Plane 12/Resources/plugins/stkpconnector/mac.xpl
```

La cartella deve chiamarsi `stkpconnector`, perché STKP controlla la presenza del plugin originale in quel percorso.

Controllare `X-Plane 12/Log.txt` per messaggi `[OpenSTKPConnector]`.

## Stato v0.3

Verificato:

- plugin caricato da X-Plane 12 in modalità Apple Silicon nativa;
- server TCP attivo su `127.0.0.1:51303`;
- SimToolkitPro mostra l'aereo sulla mappa;
- tracking base durante un volo breve;
- salvataggio log STKP completato dopo fine volo manuale.

Da analizzare:

- chiusura automatica del volo in STKP;
- gestione più precisa delle connessioni multiple aperte da STKP;
- eventuale riduzione del set DataRef dopo altri test reali.

## Roadmap suggerita per Codex

1. Verificare che il progetto compili con X-Plane SDK reale.
2. Correggere include path SDK se necessario.
3. Confermare caricamento plugin in X-Plane nativo Apple Silicon.
4. Confermare `LISTEN` su porta `51303`.
5. Testare connessione con `nc`.
6. Avviare SimToolkitPro e verificare comparsa aereo sulla mappa.
7. Analizzare la fine volo automatica.
8. Migliorare gestione client multipli/disconnessioni.
9. Aggiungere logging configurabile se necessario.
10. Preparare release Universal.
