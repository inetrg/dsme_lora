DSME-LoRa for Omnet++
=====================

## Instructions

1. Install Omnet++ 6 (Preview 10) from https://omnetpp.org/download/preview

Note: In some Linux distrubutions it might be necessary to set the
`LD_LIBRARY_PATH` to `<omnetpp_dir>/lib` in case `opp_*` commands fail
to link against Omnet++ libraries.

2. Update all git submodules
```
git submodule update --init --recursive
```

3. Build the `inet` framework:
```
make -C inet makefiles
make -C inet -j4
```

This version points to INET v4.3.0

5. Build `flora` as a static library
```
make -C flora makefiles-static-lib
make -C flora -j4
```

6. Run the simulation with:
```
make run
```

## Use an existing INET library instance

```
INET_PATH=<path_to_inet> make ...
```

This also works for `flora`

## Choose a different simulation

```
SIMULATION=<simulation> make run
```

Simulations are a ini file stored in `simulations`.
By default, `SIMULATION=s_30_a_10_spa_4` (points to `simulations/s_30_a_10_spa_4.ini`)

## Use command line simulation instead of Qt/Tk based

```
CMDENV=1 make run
```

## Run all available configurations

```
cd simulations && bash runall.sh
```
