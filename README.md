# Zyn

### Introduction

Zyn is a cross-platform build system for C/C++ projects.

CMake is supported and maintained by **Zyntraxis Corp**, a company founded by Armen Baghdasaryan.

### License

Zyn is distributed under the Zyntraxis License. See LICENSE for details.

### Installing Zyn

#### Supported Platforms

- **Linux**

Zyn is currently distributed for Linux systems as a `.deb` package.

#### Installation via `.deb` package (Ubuntu/Debian-based systems)

1. Download the latest `.deb` release from the [Zyn GitHub Releases page](https://github.com/zyntraxis-corp/zyn/releases/tag/v1.0.0):

```bash
wget https://github.com/zyntraxis-corp/zyn/releases/download/v1.0.0/zyn_1.0.0_amd64.deb
```

2. Install the package using dpkg:
```bash
sudo dpkg -i zyn_1.0.0_amd64.deb
```

3. If any dependencies are missing, fix them with:
```bash
sudo apt-get install -f
```

### Usage

#### Initialize a Project
Zyn provides an ```init``` command to create a new project. You can choose from three configurations: ```debug``` and ```release```. To create in test mode you just need to write ```zyn init```. Here's how to use the init command:

```bash
zyn init [debug|release]
```
- debug: Initializes a project for debugging, with the necessary compiler flags for debugging (-g, -O0).

- release: Initializes a project for release with optimization flags (-O3 and -DNDEBUG).

- test: Initializes a project for testing with debugging and warning flags enabled.

Example:

```bash
zyn init debug
```
This will create a ```config.zyn``` file with the specified settings and generate a basic ```src/main.c``` or ```src/main.cpp``` depending on the chosen language.

#### Run a Project
Once your project is configured, you can build and run it using the ```run``` command:

```bash
zyn run
```
This will:
- Compile the project using the configured compiler and settings.
- Run the compiled executable.

#### Build System Integration
Zyn can automatically detect the build system of the cloned repository and use it to build the project. The supported build systems are:

- **CMake**: If ```CMakeLists.txt``` is present.

- **Make**: If ```Makefile``` or ```makefile``` is present.

- **Autotools**: If ```configure``` script is present.

- **Ninja**: If ``` build.ninja``` is present.

Zyn automatically detects which build system is used and invokes the corresponding build tool.

#### Dependencies
Dependencies are managed via Git. You can specify a list of dependencies in the ```config.zyn``` file under the ```[dependencies]``` section. Zyn will automatically clone the repositories and attempt to build them using the appropriate build system.

Example ```config.zyn``` file:
```ini
[config]
version=1.0.0
project=my_project
language=cpp
standard=c++17
build_type=release
compiler=g++

[dependencies]
https://github.com/some/dependency.git
```

When running ```zyn run```, Zyn will ensure the dependencies are cloned and built before building your project.

#### Configuration File (```config.zyn```)
The ```config.zyn``` file is essential for defining your project’s settings. It includes sections like ```[config]```, ```[directories]```, ```[dependencies]```, and more.

Example ```config.zyn```:
```ini
[config]
version=1.0.0
project=my_project
language=cpp
standard=c++17
build_type=debug
compiler=g++
warnings=all
optimization=none
c_cache=on

[directories]
sources=src
include=include
build=build

[dependencies]
https://github.com/some/dependency.git

[linting]
linter=clang-tidy
config_file=.clang-tidy
enable_checks=all
treat_warnings_as_errors=true

[analysis]
static_analyzer=cppcheck
cppcheck_enable=all
cppcheck_inconclusive=true
cppcheck_force=true

[profiling]
tool=valgrind
```

#### Build Commands
Zyn supports the following build commands for dependencies:

- **CMake**: Automatically runs ```cmake``` commands to configure and build the project.

- **Make**: Uses ```make``` to build the project.

- **Autotools:** Runs ```./configure```,``` make```, and ```make install```.

- **Ninja:** Runs ```ninja``` if a ```build.ninja``` file is present.

#### Find Include Directories
Zyn can automatically find the ```include``` directories in your dependencies. This is useful for passing the correct include paths to the compiler during the build process.

Example:

```cpp
std::vector<fs::path> includes = find_include_dirs("dependencies");
```

This will find and return all include directories within the dependencies.

#### Configuration Options
The ```config.zyn``` file allows you to configure various aspects of your build process, including:

- **Compiler settings:** Specify the compiler, warnings, optimization level, and cache.

- **Directories:** Set paths for ```sources```, ```include```, and ```build``` directories.

- **Linting:** Configure the linter (```clang-tidy```) and enable or disable specific checks.

- **Static analysis**: Enable tools like ```cppcheck``` for static analysis.

- **Profiling**: Set up profiling tools like ```valgrind``` for performance analysis.

#### Known Issues
- Dependency cloning may fail if the repository URL is incorrect or unreachable.

- Currently only tested on Linux, other platforms may require adjustments or additional dependencies.

#### License
Zyn is distributed under the Zyntraxis License. See LICENSE for details.

#### Contributing

Contributions are welcome! Please feel free to open issues or submit pull requests on GitHub.