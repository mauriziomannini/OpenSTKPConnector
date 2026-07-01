# Protocollo STKPConnector osservato

Questa documentazione deriva dall'analisi del plugin originale `stkpconnector` per X-Plane 12 su macOS e da una cattura Wireshark effettuata con X-Plane 12 avviato in Rosetta.

## Ruoli di rete

- Il plugin STKPConnector dentro X-Plane apre un server TCP su `127.0.0.1:51303`.
- SimToolkitPro si collega come client alla porta `51303`.
- Nella cattura osservata la connessione era `127.0.0.1:53110 -> 127.0.0.1:51303`.
- `lsof` conferma che `X-Plane` è in `LISTEN` su `*:51303`.

## Direzione del traffico

- Il traffico osservato è solo server -> client.
- Nel `Follow TCP Stream` di Wireshark compare solo testo rosso, cioè dati inviati da X-Plane/STKPConnector verso SimToolkitPro.
- Non è stato osservato alcun comando inviato da SimToolkitPro al plugin.

## Handshake

Non è stato osservato alcun handshake iniziale.

Assenti, nella cattura:

- `HELLO`
- `CONNECT`
- autenticazione
- versione protocollo
- sottoscrizione a DataRef
- heartbeat lato client

La connessione inizia direttamente con righe DataRef.

## Formato messaggi

Il protocollo è testuale ASCII, una riga per valore:

```text
<tipo> <dataref> <valore>\n
```

Tipi osservati:

- `uf`: float
- `ud`: double
- `ufa`: array di float

Esempi reali:

```text
uf sim/flightmodel/position/groundspeed 0.0003045624471
uf sim/flightmodel/position/true_airspeed 5.489738464
ud sim/flightmodel/position/latitude 43.81456867
ud sim/flightmodel/position/longitude 11.21206011
uf sim/flightmodel/position/y_agl -0.009446382523
ud sim/flightmodel/position/elevation 40.4230721
uf sim/flightmodel/position/mag_psi 224.2736206
uf sim/flightmodel/position/theta 4.567834854
uf sim/flightmodel/position/phi -0.6007277369
uf sim/flightmodel/position/psi 228.0806427
uf sim/time/zulu_time_sec 68714.3125
ufa sim/flightmodel2/engines/N2_percent [18.2262,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
```

## DataRef iniziali osservati

Elenco iniziale ricavato dalla cattura:

```text
sim/flightmodel/position/groundspeed
sim/flightmodel/position/true_airspeed
sim/flightmodel/position/latitude
sim/flightmodel/position/longitude
sim/flightmodel/position/y_agl
sim/flightmodel/position/elevation
sim/flightmodel/position/mag_psi
sim/flightmodel/position/theta
sim/flightmodel/position/phi
sim/flightmodel/position/psi
sim/flightmodel/position/local_vy
sim/flightmodel/forces/g_nrml
sim/flightmodel/position/local_vx
sim/flightmodel/position/local_vz
sim/time/local_time_sec
sim/time/zulu_time_sec
sim/flightmodel/weight/m_fuel_total
sim/flightmodel2/engines/N2_percent
```

Il file completo `docs/Wireshark/stream.txt` contiene molti altri campioni del flusso reale.

## Implicazione progettuale

Per compatibilità con SimToolkitPro è sufficiente implementare un plugin X-Plane che:

1. si carichi nativamente in X-Plane 12 Apple Silicon;
2. apra un TCP server su `127.0.0.1:51303`;
3. legga i DataRef necessari tramite X-Plane SDK;
4. invii righe ASCII nel formato osservato;
5. mantenga la connessione persistente finché il client resta connesso.

Non è necessario modificare SimToolkitPro.
