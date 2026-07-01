# Ricerca e reverse engineering

## Problema iniziale

SimToolkitPro 1.3.6 con plugin `stkpconnector` funziona con X-Plane 12 su MacBook Pro M1 solo se X-Plane viene avviato in Rosetta.

Con X-Plane 12 Apple Silicon nativo il tracking non funziona.

## Analisi del plugin originale

Il file `mac.xpl` del plugin originale risulta compilato solo per Intel `x86_64`.

Risultato atteso con `file` o `lipo -info`:

```text
Mach-O 64-bit x86_64 dynamically linked shared library
```

Le librerie Qt incluse risultano anch'esse Intel-only, in particolare:

- `QtCore.framework`
- `QtNetwork.framework`

Non è stato trovato alcun binario `arm64` o Universal.

## Conseguenza tecnica

Un processo X-Plane `arm64` non può caricare un plugin `x86_64`.

Rosetta traduce l'intero processo, non un singolo plugin. Quindi:

- X-Plane in Rosetta -> processo `x86_64` -> plugin Intel caricabile;
- X-Plane nativo Apple Silicon -> processo `arm64` -> plugin Intel non caricabile.

## Network Mode di SimToolkitPro

Il Network Mode non elimina la necessità del plugin.

Serve a collegare SimToolkitPro a un host/porta specifici, ma il server TCP deve comunque essere offerto dal plugin STKPConnector dentro X-Plane.

## Wireshark

Cattura effettuata su interfaccia loopback `lo0`.

Risultati principali:

- conversazione TCP locale su porta `51303`;
- flusso ASCII leggibile;
- nessun traffico osservato da SimToolkitPro verso il plugin;
- nessun handshake iniziale.

File allegati nel repository:

- `docs/Wireshark/stkp_capture.pcapng`
- `docs/Wireshark/stream.txt`
- `docs/Wireshark/screenshots/`

## `lsof`

Output osservato:

- `X-Plane` in `LISTEN` su `*:51303`;
- `SimToolkitPro` connesso da porta effimera locale verso `127.0.0.1:51303`.

Questo conferma che il plugin è il server e SimToolkitPro il client.

## Conclusione

È realistico creare un plugin alternativo, scritto da zero, che emuli il protocollo STKPConnector senza richiedere codice sorgente originale.
