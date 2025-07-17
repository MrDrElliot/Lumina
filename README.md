<img src="https://github.com/user-attachments/assets/552b8ca0-ebca-4876-9c6a-df38c468d41e" width="120"/>

# Lumina Game Engine

[![License](https://img.shields.io/github/license/mrdrelliot/lumina)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows-blue)]()
[![Build](https://img.shields.io/badge/build-Visual%20Studio%202022-blueviolet)]()
[![Language](https://img.shields.io/badge/language-C++20-blue)]()
[![Renderer](https://img.shields.io/badge/renderer-Vulkan-red)]()
[![Reflection](https://img.shields.io/badge/reflection-custom-lightgrey)]()
[![Style](https://img.shields.io/badge/style-Custom-green)]()
[![Status](https://img.shields.io/badge/status-WIP-orange)]()
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)]()
![Discord](https://img.shields.io/discord/:1200592882080415785)


**Lumina** is a C++-based, Vulkan-powered game engine built as a hands-on learning project. Inspired by engines like Unreal and Godot, it emphasizes reflection, modularity, and a clean visual editor built with ImGui.

> ⚠️ **Early development** – APIs and systems are in flux, and not everything is fully documented or stable yet.

---

## ✨ Features

- 🔁 **Reflection system** for serialization and editor integration
- 🖥️ **ImGui-based editor** with real-time scene editing
- 🎮 **ECS-Based workflow**.
- 🎮 **Vulkan renderer**
- 🔗 Integrated third-party libraries: GLFW, VMA, EASTL, VkBootstrap, EnTT, EnkiTS, ImGui, FastGLTF, GLM, NlohmanJson, RPMalloc, SPDLog, SPIRV-Reflect, STB_Image, Tracy, XXHash.

---

## 📸 Screenshots

<details>
<summary>Click to expand</summary>

[<img src="https://github.com/user-attachments/assets/b6811315-7097-4e01-a4fe-d3f06f30c252" width="400"/>](https://github.com/user-attachments/assets/b6811315-7097-4e01-a4fe-d3f06f30c252)

[<img src="https://github.com/user-attachments/assets/283c9475-c1b7-4b6b-bbc8-781dc2aeb000" width="400"/>](https://github.com/user-attachments/assets/283c9475-c1b7-4b6b-bbc8-781dc2aeb000)

[<img src="https://github.com/user-attachments/assets/c4ec5ced-4f1c-4387-8836-78c8cbeda179" width="400"/>](https://github.com/user-attachments/assets/c4ec5ced-4f1c-4387-8836-78c8cbeda179)

</details>

---

## 🛠️ Building Lumina (Windows Only)

> ✅ MSVC 17.8+ required  
> ❌ Linux/macOS support is not available yet

### Setup:

1. Clone the repo.
2. Run `Setup.py` (this extracts external dependencies).
3. Ensure the `LUMINA_DIR` environment variable is set.
4. Run `Scripts/Win-GenProjects.py`.
5. Open the generated `.sln` in Visual Studio.
6. Build the **Reflector** in `Release` mode.
7. Set **Editor** as startup project.
8. Build & run.

---

## 🧱 Included Applications

- **Editor** – ImGui-based main app
- **Sandbox** – For API testing
- **Reflector** – Generates reflection metadata

---

## 📦 Requirements

- **Visual Studio 2022**
- **Python** (auto-installed if missing)
- **Vulkan SDK**

---

## 🧭 Roadmap

- ✅ Windows support
- 🔄 Refactor to dynamic/shared libraries
- 🔜 Multithreaded renderer
- 🔜 Plugin system
- 🔜 macOS/Linux support

---

## 🤝 Contributing

Pull requests are welcome!

> See [CONTRIBUTING.md](CONTRIBUTING.md) for code style and guidelines.

- ✅ Tests, documentation, and clean commits required.

---

## 🧑‍💻 Coding Style

- `F` prefix = internal engine types (non-reflected)
- `C` prefix = reflected types (classes)
- `S` prefix = reflected types (structs)
- PascalCase for all identifiers
- Tabs (not spaces), braces on new lines
- See [Coding Standards](CONTRIBUTING.md) for full details.

---

## 📄 License

Lumina is licensed under [Apache 2.0](LICENSE).

You may use, modify, and distribute the code freely — just keep the license and attribution intact.

---

## 🔗 Links

- [GitHub Repository](https://github.com/mrdrelliot/lumina)
- [Discord Community](https://discord.gg/xQSB7CRzQE)

