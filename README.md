# OCC-GUI
GUI tool for OpenCppCoverage. Require OpenCppCoverage executable binary.


## My Environment
1. Qt 5.12 MinGW 64Bit
2. Windows 10 x64

## An OpenCppCoverage executable binary is required.
1. https://github.com/OpenCppCoverage/OpenCppCoverage repository here

## TODO-List
1. Add single-file support(for --sources and --excluded_sources).
2. Add communicate with guest process.
3. Add set_work_directiry support.
4. Add guest process and OpenCppCoverage process out export support.
5. Add support for argument edit.

## Current state
1. Support add many directory to PATH environment variable.
2. Only html output format support(can specify a path).
3. --sources and --excluded_sources now only support directory.
4. Multiple modules support.
5. Easily it run again.
