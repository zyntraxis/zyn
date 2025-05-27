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

Download .deb package
```bash
url: https://github.com/zyntraxis/zyn/releases/tag/v2.0.0

sudo dpkg -i zyn_2.0.0.deb
```

---

## Features

- 🚀 Project scaffolding for C/C++
- 📦 Git and local dependency management
- 🔄 Automatic dependency resolution
- 🔒 Version locking with hash verification
- ⚡ Parallel dependency installation
- 🛠️ Multiple build configurations (debug/release)

---

# 🚀 Quick Start

## Create a new project:
```bash
zyn new my_project
```

## Navigate to project:
```bash
cd my_project
```

## Add dependencies:
```bash
zyn install https://github.com/fmtlib/fmt.git@9.1.0
```

## Build and run:
```bash
zyn run --release
```

# Commands

| Command                   | Description            |
| ------------------------- | ---------------------- |
| `new <name>`              | Create new project     |
| `install [url]/[url]@tag`           | Install dependencies   |
| `add <path>`              | Add local dependency   |
| `run [--debug --release]` | Build and execute      |
| `update`                  | Update dependencies    |
| `clean`                   | Remove build artifacts |

# Project Structure

Generated project layout:
```
project/
├── .zyn/
│   ├── deps/      # Downloaded dependencies
│   ├── build/     # Dependency build outputs
│   └── lock/      # Version lock files
├── src/           # Source files
├── include/       # Headers
└── zyn.toml       # Project configuration
```

# Configuration (zyn.toml)

Example configuration:
```toml
[project]
name = "my_app"
version = "1.0.0"
language = "cpp"
standard = "c++20"
compiler = "clang++"

[settings.profiles.--release]
flags = [
    "-w -O3 -ffast-math -finline-functions -funroll-loops -fomit-frame-pointer -march=native -flto -DNDEBUG -fstrict-aliasing -fmerge-all-constants",
]
[settings.profiles.--debug]
flags = [
    "-g -O0 -DDEBUG -fno-inline -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined",
]
[settings.profiles.custom_profile]
flags = [
    "-g -O1",
]

[directories]
sources = "src"
include = "include"
build = ".zyn/build"

[dependencies]
fmt = { git = "https://github.com/fmtlib/fmt.git", tag = "9.1.0" }
boost = { path = "/opt/boost" }

[libraries]
lib_dirs = ["/usr/local/lib/build"]
libraries = ["pthread", "dl"]
```

# Dependency Management

## Git Dependencies
```toml
[dependencies]
library = { git = "https://github.com/user/repo.git", tag = "v1.2.3" }
```

## Local Dependencies
```toml
[dependencies]
mylib = { path = "../local/path" }
```
## IDE Integration
Version 2.3.0 introduces powerful new functionality: automatic generation of project configurations for popular IDEs. This greatly improves development workflow and makes it seamless to integrate Zyn-based C++ projects into:
- Visual Studio Code
- CLion
- QtCreator
# 🔧 Command Syntax
```zyn ide --vscode
zyn ide --clion
zyn ide --qtcreator
```
Each command creates the appropriate configuration files in a dedicated folder for your chosen IDE.
## 📁 What’s Generated
```zyn ide --vscode```
Creates the ```.vscode/``` directory with:
- ```c_cpp_properties.json``` — IntelliSense configuration and include paths
- ```tasks.json``` — build tasks using ```zyn run --release```
- ```launch.json``` — debugging configuration using GDB
Fully integrates with Visual Studio Code out of the box.

```zyn ide --clion```
Creates ```.clion/CMakeLists.txt```
This allows CLion to open and work with your Zyn project immediately.

```zyn ide --qtcreator```
Creates ```.qtcreator/ZynProject.pro```
Supports smooth integration with QtCreator without manual setup.

## 🧠 How It Works
- Reads your zyn.toml for:
 - Project name
 - Language (c or cpp)
 - Compiler path
 - Language standard (c11, c++17, etc.)
 - Include directories

- Automatically populates all config files with accurate, project-specific data.

## 💡 Example Usage
``` zyn ide --vscode
# Generates full VSCode config in .vscode/

zyn ide --clion
# Generates CMakeLists.txt in .clion/

zyn ide --qtcreator
# Generates Qt project file in .qtcreator/
```

# Build System

Zyn automatically:

- Detects header directories (`include/`, `Include/`)
- Generates appropriate compiler flags
- Supports CMake-based dependencies
- Maintains version locks in `.zyn/lock/`

# Best Practices

- Commit `zyn.toml` and `.zyn/lock/` to version control
- Use semantic versioning for tags
- Prefer tagged releases over branches
- Run `zyn update` periodicallyroduct on the homepage. Animated images will only play on hover.
- Run `zyn update` periodically

### 🔍 Zyn vs Traditional Build Tools (pluses and minuses)

| Feature                        | ✅ Zyn                                                      | ⚙️ Traditional Tools (CMake, Conan, etc.)                 |
| ------------------------------ | ---------------------------------------------------------- | -------------------------------------------------------- |
| **Zero config to get started** | ✅ One-line setup (`zyn install ...`)                       | ❌ Requires multiple files/configs                        |
| **Dependency fetching**        | ✅ Git-based, no registries needed                          | ⚠️ Requires remotes or package registries                 |
| **Dependency building**        | ✅ Auto-detects & builds (CMake/Make)                       | ⚠️ Often manual, or scripted via toolchains               |
| **Cross-platform by default**  | ✅ Yes                                                      | ⚠️ Requires setup (toolchains, flags, etc.)               |
| **IDE integration**            | 🔜 Planned                                                  | ✅ Mature, supported by many tools                        |
| **CI/CD ready**                | 🔜 Planned                                                  | ✅ Well-supported                                         |
| **Fine-grained control**       | ❌ Minimalist by design                                     | ✅ Full control over every step                           |
| **Custom toolchain support**   | ❌ Not yet                                                  | ✅ Fully supported                                        |
| **Binary caching**             | ❌ Not yet                                                  | ✅ Via Conan/Nix/etc.                                     |
| **Ease of use**                | ✅ Extremely simple                                         | ⚠️ Steep learning curve                                   |
| **Best for**                   | 🛠️ Small/medium projects, internal tools, quick prototyping | 🏗️ Large-scale, multi-platform, production-grade projects |
