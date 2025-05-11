# ⚡ Zyn — Cross-platform Build System for C/C++

Zyn is a cross-platform build system for C and C++ projects, designed for simplicity, high performance, and powerful tooling. It manages compilation, dependency handling, linting, static analysis, and profiling in a unified way.

Maintained by **Zyntraxis Corp**, founded by **Armen Baghdasaryan**.

---

## 🛡 License

Zyn is distributed under the **Zyntraxis License**. See `LICENSE` for full terms.

---

## 📦 Installing Zyn

### Supported Platforms
- ✅ **Linux** (Ubuntu/Debian-based systems)

### Install via `.deb` package

```bash
wget https://github.com/zyntraxis-corp/zyn/releases/download/v1.0.2/zyn_1.0.2_amd64.deb
sudo dpkg -i zyn_1.0.2_amd64.deb
sudo apt-get install -f  # Fix missing dependencies if needed
```

---

## 🚀 Quick Start

### Initialize a Project

```bash
zyn init [debug|release|test]
```

- `debug`: Enables debugging flags (`-g -O0`) and tools like linting and analysis.
- `release`: Enables aggressive optimizations for performance (`-O3`, `-flto`, `-march=native`, etc.).
- `test`: Similar to debug but tuned for test environments.

> A `config.zyn` file will be generated along with a basic `src/main.cpp` or `src/main.c`.

---

### Build and Run

```bash
zyn run
```

- Compiles the source using your configuration.
- Automatically handles dependencies.
- Runs the resulting binary.
- If enabled, runs profilers like `valgrind` (debug only).

---

## 🆕 What’s New in 1.0.2

### ✅ `zyn add` — Add Dependencies via Git

You can now add dependencies using a single command:

```bash
zyn add https://github.com/gabime/spdlog
```

Zyn will fetch the dependency and use it during the build.

---

### 🏷 Version-Specific Dependencies

Append `@version` to pin a specific version:

```bash
zyn add https://github.com/gabime/spdlog@v1.12.0
```

---

### 🧼 `zyn clean` — Clean the Project

Clean all build files and dependencies with:

```bash
zyn clean
```

---

### ⚡ Maximum Release Optimization

In `release` mode, Zyn now adds advanced optimization flags:

```bash
-O3 -DNDEBUG -flto -march=native -funroll-loops -fomit-frame-pointer -fno-exceptions -fno-rtti -finline-functions -fprefetch-loop-arrays -Wl,--gc-sections -s -fvisibility=hidden
```

Perfect for production builds.

---

### 🐞 Enhanced Debug Experience

In `debug` mode:

- Debug symbols (`-g -O0`)
- Runs **clang-tidy** automatically if configured
- Runs **cppcheck** with full analysis:
  ```bash
  --enable=all --inconclusive --force
  ```
- Full logging of executed commands for transparency

---

## ⚙️ config.zyn Structure

```ini
[config]
version=1.0.2
project=my_project
language=cpp
standard=c++17
build_type=debug
compiler=clang++
warnings=all
optimization=none
c_cache=on

[directories]
sources=src
include=include
build=build

[dependencies]
https://github.com/gabime/spdlog@v1.12.0

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

---

## 🔧 Dependency Build System Support

Zyn automatically detects and uses the following systems from dependencies:

- **CMake** — CMakeLists.txt
- **Make** — Makefile/makefile
- **Autotools** — `configure` script
- **Ninja** — `build.ninja`

---

## 📂 Include Directory Detection

Zyn scans dependency folders to find include paths and adds them during compilation:

```cpp
std::vector<fs::path> includes = find_include_dirs("dependencies");
```

---

## 📋 Known Issues

- Dependency fetching fails if URL is invalid or private.
- Currently tested only on Linux. Other OS support is experimental.

---

## 🤝 Contributing

Pull requests and issues are welcome on [GitHub](https://github.com/zyntraxis-corp/zyn)!