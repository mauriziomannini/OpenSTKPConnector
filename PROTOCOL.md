# Observed STKPConnector Protocol

This document is based on reverse engineering of the original macOS `stkpconnector` plugin for X-Plane 12 and Wireshark captures made while running X-Plane through Rosetta.

## Network Roles

- The X-Plane plugin opens a TCP server on `127.0.0.1:51303`.
- SimToolkitPro connects as a TCP client.
- `lsof` confirms that `X-Plane` listens on port `51303`.

## Traffic Direction

- The main stream is server to client: X-Plane plugin to SimToolkitPro.
- The complete GoldenFlight capture also contains client-to-server traffic.
- The compatibility-critical part is an initial greeting sent by the plugin to STKP.

## Greeting

The compatible stream starts with:

```text
STKPCONNECT 1
STKPCONNECT-VERSION 2020
```

It is followed by subscription-style lines:

```text
sub <dataref>
```

OpenSTKPConnector sends this greeting immediately after accepting a TCP client. Native Apple Silicon testing showed that SimToolkitPro starts tracking only when this greeting is sent before the first DataRef snapshot.

Examples:

```text
sub sim/time/paused
sub sim/time/sim_speed
sub sim/flightmodel/position/groundspeed
sub sim/flightmodel/position/latitude
sub sim/flightmodel/position/longitude
sub sim/aircraft/view/acf_descrip
sub sim/aircraft/view/acf_ICAO
```

After the greeting, the plugin sends an initial DataRef snapshot and then continues with periodic updates.

## Message Format

The protocol is text-based ASCII, one value per line:

```text
<type> <dataref> <value>\n
```

Observed types:

- `uf`: float
- `ud`: double
- `ui`: integer
- `ufa`: float array
- `uia`: integer array
- `ub`: byte/string value encoded as Base64

Examples:

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

## Observed DataRefs

Initial observed set:

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

The GoldenFlight capture added these DataRefs in the greeting and initial snapshot:

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

The complete reference stream is stored in:

```text
docs/Wireshark GoldenFlight/GoldenFlight_Stream.txt
```

## Implementation Implications

A compatible plugin must:

1. load natively in X-Plane 12;
2. open a TCP server on `127.0.0.1:51303`;
3. send `STKPCONNECT`, `STKPCONNECT-VERSION`, and `sub` lines as soon as a client connects;
4. read the required DataRefs through the X-Plane SDK;
5. send a compatible initial snapshot after a client connects;
6. send ASCII lines in the observed format;
7. keep the connection alive while the client remains connected.

No SimToolkitPro modification is required.
