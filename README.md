# Lumina Game Engine

![Lumina Logo](https://github.com/user-attachments/assets/5efc3b14-caff-4465-8597-33f8d70e50cd)

Lumina is a lightweight and flexible game engine designed to simplify game development while offering powerful tools for creators. Built with accessibility and efficiency in mind, Lumina empowers developers to bring their creative visions to life.

---

## Development Status

**Lumina is currently under heavy development.** As such:

- The engine's use and build process are not yet streamlined.
- Expect frequent changes and potential instability.
- Comprehensive documentation and tutorials are forthcoming.

---

## Platform Support

- **Windows**: Fully supported.
- **Linux**: *Not currently supported.*
- **MacOS**: *Not currently supported.*

- **MSVC**: 17.8 (19.38) or higher.
- **Clang**: *Not currently supported.*
- **GCC**: *Not currently supported.*

---

## What's Included

- **Core Engine Features**: Serialization, math utilities, logging, and other foundational systems.
- **Graphics Rendering**: A simple yet effective 3D rendering engine utilizing Vulkan for high-performance graphics.
- **Editor with ImGui Integration**: A user-friendly editor interface built with ImGui for intuitive workflows.
- **Reflector Application**: A tool that generates reflection code to facilitate serialization and editor integration.
- **Third-party Integrations**:
  - **ImGui**: GUI elements.
  - **GLFW**: Window and input management.
  - **Vulkan**: High-performance rendering.
  - **EASTL**: Versatile data structures.
  - **EnTT**: Entity Component System (ECS) framework (subject to change).
  - **Vulkan Memory Allocator**: Simplifies Vulkan memory management.
  - **VkBootstrap**: Eases Vulkan setup.

---

## Requirements

- **Visual Studio 2022** (or a compatible build system).
- **Python**: Required for script automation (the setup script will attempt to install Python if not present).
- **Third-party Dependencies**: Preconfigured libraries (ImGui, GLFW, Vulkan).

---

## Building Lumina

1. Clone the repository and ensure all dependencies are included.
2. Run the Python script `Win-GenProjects.py` located in the `Scripts` directory to set up the project.
3. Open the generated Visual Studio solution.
4. Set the **Editor** project as the default startup project.
5. Build and run the solution.

---

## Applications

- **Editor**: The main application providing a visual interface to interact with the engine's systems.
- **Sandbox**: A simple application for testing the engine's API.
- **Reflector**: Generates reflection code to support serialization and editor functionalities.

---

## Roadmap

- **Windows Support First**: Establish a solid foundation on Windows.
- **Cross-platform Expansion**: Planned support for macOS and Linux.
- **Advanced Rendering Features**: Integration of modern graphics techniques with Vulkan.
- **Shared Library Transition**: Move from a static library to a shared (DLL) approach.
- **User-defined Plugins**: Develop an extensible plugin system for custom game logic.
- **Multi-threaded Renderer**: Implement a renderer that leverages multi-threading for performance.

---

## License

Lumina is licensed under the [MIT License](LICENSE), allowing free use, modification, and distribution of the engine's source code.

---

## Contributions

Contributions are welcome! Whether you're adding features, fixing bugs, or improving documentation, your help is vital to Lumina's growth. Please refer to the [CONTRIBUTING](CONTRIBUTING.md) file for guidelines.

---

## Pull Request Checklist

Before submitting a pull request, ensure the following:

- [ ] Code compiles without errors.
- [ ] No use of generative AI tools (e.g., GitHub Copilot, ChatGPT) in code contributions.
- [ ] Adherence to the project's coding standards and style guidelines.
- [ ] Inclusion of relevant tests for new features or bug fixes.
- [ ] Updates to documentation, if applicable.
- [ ] Clear and descriptive commit messages.

---

## Getting Started

To begin developing with Lumina:

1. Run the **Editor** application from the provided Visual Studio solution.
2. Explore the engine's features and start building your project.

Stay tuned for additional documentation and tutorials as the engine evolves.

---

For more information and to access the source code, visit the [Lumina GitHub Repository](https://github.com/mrdrelliot/lumina).