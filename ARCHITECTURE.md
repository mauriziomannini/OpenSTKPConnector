# Architettura OpenSTKPConnector

## Obiettivo

Sostituire il plugin Intel-only `STKPConnector/mac.xpl` con un plugin nuovo, compilabile per Apple Silicon (`arm64`) e opzionalmente Universal (`arm64;x86_64`), compatibile con SimToolkitPro.

## Architettura target

```text
X-Plane 12 nativo Apple Silicon
        │
        │ X-Plane SDK / DataRefs
        ▼
OpenSTKPConnector.xpl
        │
        │ TCP server 127.0.0.1:51303
        ▼
SimToolkitPro client
```

## Componenti

### Plugin entrypoint

File: `src/plugin.cpp`

Responsabilità:

- implementare `XPluginStart`, `XPluginStop`, `XPluginEnable`, `XPluginDisable`;
- inizializzare DataRef manager;
- avviare TCP server;
- registrare il Flight Loop callback.

### TCP Server

File: `src/tcp_server.cpp`

Responsabilità:

- aprire socket TCP su `127.0.0.1:51303`;
- accettare connessioni client;
- inviare payload testuale a tutti i client connessi;
- rimuovere client disconnessi;
- evitare crash in caso di `SIGPIPE`/client chiuso.

### DataRef Manager

File: `src/datarefs.cpp`

Responsabilità:

- registrare l'elenco dei DataRef da esportare;
- risolverli con `XPLMFindDataRef`;
- leggere valori float, double e array float;
- generare un frame testuale nel formato compatibile.

### Logger

File: `src/logger.cpp`

Responsabilità:

- scrivere messaggi diagnostici nel `Log.txt` di X-Plane tramite `XPLMDebugString`.

## Frequenza di invio

La bozza attuale invia un frame ogni `0.05s`, cioè circa 20 Hz.

La frequenza andrà verificata contro la cattura originale. Se SimToolkitPro riceve correttamente, mantenere 20 Hz; altrimenti provare 10 Hz o 30 Hz.

## Output plugin

L'output deve chiamarsi:

```text
mac.xpl
```

Da installare in:

```text
X-Plane 12/Resources/plugins/OpenSTKPConnector/mac.xpl
```

oppure, se si vuole seguire una struttura separata:

```text
X-Plane 12/Resources/plugins/OpenSTKPConnector/64/mac.xpl
```

La struttura esatta va testata con X-Plane 12 su macOS.
