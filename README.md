# profzoom

Superpowers for Linux Flash Projectors

[![travis-ci](https://travis-ci.org/shockpkg/profzoom.svg?branch=master)](https://travis-ci.org/shockpkg/profzoom)


# Overview

Gives native code powered functionality to Linux Flash Projectors via LD_PRELOAD.

For other platforms, Adobe AIR can provide desktop apps with powerful desktop API's.

AIR for Linux is long dead however, so this library can help fill in the gaps in the limitations of Linux Flash Projectors.

Named after Professor Zoom, arch-nemesis to The Flash.


# Usage

For obvious reasons, do not load code from untrusted sources or over any unsecure channels.


## 1. Use the ActionScript 3 code in this project to give your application new powers

See test code for some examples.

Compile it to a SWF, and then optionally into a projector.


## 2. Copy profzoom.so to directory with stand-alone Flash Player or projector

Copy the `profzoom.so` that matches the Flash Player architecture into the same directory as the player and SWF or the projector.


## 3. Use a wrapper script to launch a stand-alone Flash Player or projector.

For use with player and SWF (good for debugging):

```sh
#!/bin/sh

self="`readlink -f "$0"`"
self_dir="`dirname "$self"`"
cd "$self_dir"

export LD_PRELOAD="$LD_PRELOAD:./profzoom.so"

./flashplayerdebugger main.swf
```

For use with player and projector (good for release):

```sh
#!/bin/sh

self="`readlink -f "$0"`"
self_dir="`dirname "$self"`"
cd "$self_dir"

export LD_PRELOAD="$LD_PRELOAD:./profzoom.so"

./someprojector
```


# Bugs

If you find a bug or have compatibility issues, please open a ticket under issues section for this repository.


# License

Copyright (c) 2019 JrMasterModelBuilder

Licensed under the Mozilla Public License, v. 2.0.

If this license does not work for you, feel free to contact me.
