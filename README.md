# LAPIS-SILO

Sequence Indexing engine for Large Order of genomic data

# License

Original genome indexing logic with roaring bitmaps by Prof. Neumann: https://db.in.tum.de/~neumann/gi/

# Building

We took the approach to scan directories for .cpp files to include instead of listing them manually in the
CMakeLists.txt. This has the advantage that we don't need to maintain a list of files in the CMakeLists.txt.

It has the disadvantage that after a successful build on local machines, CMake can't detect whether files were
added or deleted. This requires a clean build. You can either delete the `build/` directory manually, or you
execute `./build_with_conan clean`.

Since in any approach, a developer has to remember to either trigger a clean build or to adapt the CMakeLists.txt, we
decided for the approach with less maintenance effort, since it will automatically work in GitHub Actions.

## With Conan

We use Conan to install dependencies for local development. See Dockerfile for how to set up Conan and its requirements.
This has been tested on Ubuntu 22.04 and is not guaranteed to work on other systems.

The Conan profile (myProfile) on your system might differ: Create a new profile `~/.conan2/profiles/myProfile`

```shell
conan profile detect
```

Copy `conanprofile.example` to `conanprofile` and insert the values of `os`, `os_build`, `arch` and `arch_build` from
myProfile.

Build silo in `./build`. This build will load and build the required libraries to `~/.conan2/data/` (can not be set by
hand).

```shell
./build_with_conan.py
```

Executables are located in `build/` upon a successful build.

## With Docker:

(for CI and release)

Build docker container

```shell
docker build . --tag=silo
```

Run docker container

```shell
docker run 
  -p 8081:8081
  -v your/data/directory:/data
  silo --api --preprocessingConfig=./yourRelativePathTo/preprocessing_config.yaml --databaseConfig=./yourRelativePathTo/database_config.yaml
```

The mounted directory `your/data/directory` is the place on your machine, where the data for SILO is located.
The path to the preprocessingConfig and the databaseConfig are relative to your mounted directory. If no paths are
provided the files
preprocessing_config.yaml and database_config.yaml are expected directly in the working directory. Thus:

```shell
silo --api --preprocessingConfig=./preprocessing_config.yaml --databaseConfig=./database_config.yaml
```

is equivalent to

```shell
silo --api
```

The preprocessing config contains the following fields:

```yaml
# The directory where the input files are located, relative to the mounted folder
inputDirectory: "./"
# The directory where the output files are located, relative to the mounted folder
outputDirectory: "./output/"
# The filename of the metadata file, relative to the inputDirectory
metadataFilename: "small_metadata_set.tsv"
# The filename of the sequence file, relative to the inputDirectory
sequenceFilename: "small_sequence_set.fasta"
# The filename of the pango lineage definition file, relative to the inputDirectory
pangoLineageDefinitionFilename: "pangolineage_alias.json"
# The filename of the reference genome file, relative to the inputDirectory
referenceGenomeFilename: "reference_genome.txt"
```

Building Docker images locally relies on the local Docker cache.
Docker will cache layers, and it will cache the dependencies built by Conan via cache mounts.

However, cache mounts don't work in GitHub Actions (https://github.com/docker/build-push-action/issues/716),
so there we only rely on Docker's layer cache via Docker's gha cache backend.

# Testing

## Unit Tests

For testing, we use the framework [gtest](http://google.github.io/googletest/)
and [gmock](http://google.github.io/googletest/gmock_cook_book.html) for mocking. Tests are built using the same script
as the production code: `./build_with_conan`.

We use the convention, that each tested source file has its own test file, ending with `*.test.cpp`. The test file is
placed in the same folder as the source file. If the function under test is described in a header file, the test file is
located in the corresponding source folder.

To run all tests, run

```shell
build/silo_test
```

For linting we use clang-tidy. The config is stored in `.clang-tidy`. It will run automatically with the build process
and will throw errors accordingly. However, it is rather slow. If you only want a fast build use

```shell
./build_with_conan.py --build_without_clang_tidy
```

When pushing to GitHub, a separate Docker image will be built, which runs the formatter. (This is a workaround, because
building with clang-tidy under alpine was not possible yet.)

## Functional End-To-End Tests

End-to-end tests are located in `/endToEndTests`. Those tests are used to verify the overall functionality of the SILO
queries. To execute the tests:

* have a running SILO instance, e.g. via `docker run -p 8081:8081 ghcr.io/genspectrum/lapis-silo`
* `cd endToEndTests`
* `npm install`
* `SILO_URL=localhost:8081 npm run test`

# Logging

We use [spdlog](https://github.com/gabime/spdlog) for logging. The log level and log target can be controlled via
environment variables:

* Start SILO with `SPDLOG_LEVEL=off,console_logger=debug` to log to stdout at debug level.
* Start SILO with `SPDLOG_LEVEL=off,file_logger=trace` to log to daily rotating files in `log/` (relative from the
  location where SILO is executed) at trace level.

If `SPDLOG_LEVEL` is not set, SILO will log to the console at info level.

We decided to use the macros provided by spdlog rather than the functions, because this lets us disable log statements
at compile time by adjusting `add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)` to the desired log level
via CMake. This might be desirable for benchmarking SILO. However, the default should be `SPDLOG_LEVEL_TRACE` to give
the maintainer the possibility to adjust the log level to a log level that they prefer, without the need to recompile
SILO.

# Code Style Guidelines

## Naming

We mainly follow the styleguide provided by [google](https://google.github.io/styleguide/cppguide.html), with a few
additions. The naming is enforced by clang-tidy. Please refer to `.clang-tidy` for more details on naming inside the
code. Clang-tidy can not detect filenames. We decided to use snake_case for filenames.

## Includes

The includes are sorted in the following order:

1. Corresponding header file (for source files)
2. System includes
3. External includes
4. Internal includes

Internal includes are marked by double quotes. External includes are marked by angle brackets.