# Protocollo STKPConnector osservato

Questa documentazione deriva dall'analisi del plugin originale `stkpconnector` per X-Plane 12 su macOS e da catture Wireshark effettuate con X-Plane 12 avviato in Rosetta.

## Ruoli di rete

- Il plugin STKPConnector dentro X-Plane apre un server TCP su `127.0.0.1:51303`.
- SimToolkitPro si collega come client alla porta `51303`.
- In una delle catture osservate la connessione era `127.0.0.1:53110 -> 127.0.0.1:51303`.
- `lsof` conferma che `X-Plane` è in `LISTEN` su `*:51303`.

## Direzione del traffico

- Il traffico principale è server -> client, cioè dati inviati da X-Plane/STKPConnector verso SimToolkitPro.
- Nella cattura completa `docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt` è presente anche traffico client -> server.
- Il client SimToolkitPro invia un handshake iniziale e una lista di sottoscrizioni DataRef.

## Handshake

Nella cattura GoldenFlight SimToolkitPro apre la connessione con:

```text
STKPCONNECT 1
STKPCONNECT-VERSION 2020
```

Subito dopo invia una serie di righe:

```text
sub <dataref>
```

Esempi:

```text
sub sim/time/paused
sub sim/time/sim_speed
sub sim/flightmodel/position/groundspeed
sub sim/flightmodel/position/latitude
sub sim/flightmodel/position/longitude
sub sim/aircraft/view/acf_descrip
sub sim/aircraft/view/acf_ICAO
```

Il plugin originale risponde con una snapshot iniziale dei DataRef sottoscritti e poi continua a inviare aggiornamenti.

## Formato messaggi

Il protocollo è testuale ASCII, una riga per valore:

```text
<tipo> <dataref> <valore>\n
```

Tipi osservati:

- `uf`: float
- `ud`: double
- `ui`: integer
- `ufa`: array di float
- `uia`: array di integer
- `ub`: byte/stringa codificata in Base64

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
ui sim/flightmodel/failures/onground_all 0
ufa sim/flightmodel2/engines/N2_percent [18.2262,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
uia sim/flightmodel2/gear/on_ground [0,1,1,0,0,0,0,0,0,0]
ub sim/aircraft/view/acf_ICAO QzE3MgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==
```

## DataRef iniziali osservati

Elenco iniziale ricavato dalla cattura:

```text
sim/flightmodel/position/groundspeed
sim/flightmodel/position/indicated_airspeed
sim/flightmodel/position/true_airspeed
sim/flightmodel/position/latitude
sim/flightmodel/position/longitude
sim/flightmodel/position/y_agl
sim/flightmodel/position/elevation
sim/flightmodel/position/mag_psi
sim/flightmodel/position/magnetic_variation
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
sim/flightmodel/failures/onground_all
sim/flightmodel/failures/onground_any
sim/flightmodel2/engines/N2_percent
sim/flightmodel2/gear/on_ground
```

La cattura GoldenFlight aggiunge anche questi DataRef osservati nella fase di sottoscrizione e nella snapshot iniziale:

```text
sim/time/paused
sim/time/sim_speed
sim/aircraft/view/acf_descrip
sim/aircraft/parts/acf_gear_deploy
sim/flightmodel/controls/flaprqst
sim/aircraft/controls/acf_flap_detents
sim/operation/override/override_planepath
sim/time/is_in_replay
sim/cockpit/radios/com1_freq_hz
sim/aircraft/engine/acf_num_engines
sim/aircraft/view/acf_ICAO
```

I file completi `docs/Wireshark/stream.txt` e `docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt` contengono campioni del flusso reale.

## Implicazione progettuale

Per compatibilità con SimToolkitPro è sufficiente implementare un plugin X-Plane che:

1. si carichi nativamente in X-Plane 12 Apple Silicon;
2. apra un TCP server su `127.0.0.1:51303`;
3. accetti l'handshake e le righe `sub` inviate da SimToolkitPro;
4. legga i DataRef necessari tramite X-Plane SDK;
5. invii una snapshot iniziale compatibile quando un client si collega;
6. invii righe ASCII nel formato osservato;
7. mantenga la connessione persistente finché il client resta connesso.

Non è necessario modificare SimToolkitPro.
