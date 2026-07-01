# Brief per Codex / agente di sviluppo

Se stai lavorando su questo repository da VS Code, leggi prima questi file:

1. `README.md`
2. `PROTOCOL.md`
3. `ARCHITECTURE.md`
4. `RESEARCH.md`
5. `DEVELOPMENT.md`

## Missione

Implementare un plugin X-Plane 12 compatibile con SimToolkitPro che sostituisca il plugin originale STKPConnector Intel-only su macOS Apple Silicon.

## Vincoli importanti

- Non usare Qt.
- Usare C++17 e socket BSD/POSIX.
- Output: `mac.xpl`.
- Porta: TCP `127.0.0.1:51303`.
- Protocollo: ASCII, una riga per DataRef.
- SimToolkitPro è client passivo: non invia comandi osservati.
- Evitare di bloccare il flight loop: accept/client management su thread separato.
- La funzione di broadcast deve tollerare client disconnessi.

## Formato protocollo

```text
uf <dataref> <float>\n
ud <dataref> <double>\n
ufa <dataref> [v0,v1,...,v15]\n
```

## Primo obiettivo tecnico

Far compilare il progetto e ottenere un `mac.xpl` caricabile da X-Plane 12 Apple Silicon.

Poi verificare:

```bash
lsof -nP -iTCP:51303
nc 127.0.0.1 51303
```

## Materiale di riferimento

- Flusso reale: `docs/Wireshark/stream.txt`
- Cattura completa: `docs/Wireshark/stkp_capture.pcapng`
- Screenshot Wireshark: `docs/Wireshark/screenshots/`
- Plugin originale: `docs/OriginalPlugin/stkpconnector_original.zip`
