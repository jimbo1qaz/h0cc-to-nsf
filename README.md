# 0CC-FamiTracker

> Modular fork of 0CC-FamiTracker, with NSF export library 

h0CC-to-nsf is a fork of 0CC-FamiTracker by HertzDevil, building only source files needed for NSF export.

The custom build procedure is located at [0ccft-ext/CMakeLists.txt](https://github.com/jimbo1qaz/h0cc-to-nsf/blob/ft2nsf-lib/0ccft-ext/CMakeLists.txt).

## Changes

- CMakeLists.txt
- Assertions altered
- `#ifdef NSF_ONLY` to exclude audio processing from channel classes
