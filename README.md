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
