# Protocollo osservato

SimToolkitPro si collega come client a `127.0.0.1:51303`.
Il server è il plugin X-Plane.

Il flusso è solo server -> client, senza handshake osservato.

Formato riga:

```text
<tipo> <dataref> <valore>\n
```

Tipi osservati:

- `uf`: float
- `ud`: double
- `ufa`: array float
