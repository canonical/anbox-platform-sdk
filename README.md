# Anbox Platform SDK

The Anbox Platform SDK (APS) allows cloud providers to integrate Anbox
with their existing infrastructure.

It provides several integration points for things like rendering, audio
or input processing.

## Build the example platform plugins

A collection of example platform plugins are provided with Anbox Platform SDK
to help developers get started with plugin development. The following examples
are included:

- minimal - A platform plugin which provides a dummy implementation of a minimal platform
plugin to demonstrate the general plugin layout.
- audio_streaming - A platform plugin providing a more advanced example of how a platform
plugin can process audio and input data. It accepts audio data from Anbox and
uses libav to encode and stream it over RTP to an on demand connected client.
In addition it also shows how the OpenGL ES driver used by Anbox can be customized.

You need the following build dependencies:

```
$ sudo apt install cmake-extras libavcodec-dev libavformat-dev libelf-dev libegl1-mesa-dev
```

Now the examples can be build with the following commands:

```
$ cd examples
$ cmake ./ -Bbuild
$ cd build
$ make -j$(nproc)
```

The resulting platform plugins are following the naming convention `platform_<plugin name>.so`
and can be found within the corresponding sub directory of the build directory.

## Test a platform plugin

The SDK comes with a tool called `anbox-platform-tester` which allows validation of the
platform plugin implementation. The `anbox-platform-tester` performs various tests to ensure
the plugin is correctly implemented and behaves the way Anbox expect it to behave.

A built platform plugin can be tested with the `anbox-platform-tester` like this:

```
$ bin/anbox-platform-tester <path to plugin>/platform_<platform name>.so
```

The platform tester will print out a detailed report which of the test cases fails.
Together with the source code of the platform tester this allows to implement the plugin
in a proper way.

A production ready platform plugin needs to pass all test cases without exceptions.
