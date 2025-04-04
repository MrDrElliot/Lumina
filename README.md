# Lumina Game Engine  

![image](https://github.com/user-attachments/assets/5efc3b14-caff-4465-8597-33f8d70e50cd)

Lumina is a lightweight and flexible game engine built to simplify game development while offering powerful tools for creators. Designed with accessibility and efficiency in mind, Lumina empowers developers to bring their creative visions to life.  

---

## What's Included  

- **Core Engine Features**: Serialization, math utilities, logging, and other foundational systems.  
- **Graphics Rendering**: A simple yet effective 3D rendering engine with Vulkan as the primary graphics API for cutting-edge performance.  
- **Editor with ImGui Integration**: A user-friendly editor interface built with ImGui for intuitive workflows.  
- **Cross-platform Support (Planned)**: Future support for macOS and Linux in addition to Windows.  

---

## Features  

-  Vulkan is prioritized for its modern rendering technique.  
- **User-Friendly Editor**  
  A built-in editor application powered by ImGui, providing a seamless interface for asset creation and level design.  
- **Third-party Integrations**  
  Lumina includes the following key libraries:  
  - **ImGui** for GUI elements.  
  - **GLFW** for window and input management.  
  - **Vulkan** for high-performance rendering.
  - **EASTL** for versatile code.
  - **EnTT** Possibly temporary, for quick ECS
  - **Vulkan Memory Allocator** Ease of use allocating Vk memory
  - **VkBootstrap** Save some time with initial VK setup.

---

## Requirements  

- **Visual Studio 2022** (or a compatible build system).  
- **Python** for script automation (script will attempt to install Python for you)..  
- **Third-party Dependencies**: Preconfigured libraries (ImGui, GLFW, Vulkan).  

---

## Building Lumina  

1. Clone the repository and ensure all dependencies are included.  
2. Run the Python script `Win-GenProjects.py` in the `Scripts` directory to set up the project.  
3. Open the Visual Studio solution and set the **Editor** project as the default startup project.  
4. Build and run the solution.  

---

## Applications  

- **Editor**: The main application for interfacing with the engine's systems in a visual manner.  
- **Sandbox**: A simple app for testing the engine API.  

---

## Roadmap  

- **Windows Support First**  
  Initial focus on Windows to establish a solid foundation.  
- **Cross-platform Expansion**  
  Planned support for macOS and Linux.  
- **Advanced Rendering Features**  
  Integration of modern graphics techniques with Vulkan.  
- **Shared Library**
  Planned to switch from the current static library to a shared (DLL).
- **User-defined Plugins**  
  Extensible plugin system for custom game logic.
- **Multi-Threaded renderer**

---

## License  

Lumina is licensed under the [MIT License](LICENSE), allowing free use, modification, and distribution of the engine's source code.  

---

## Contributions  

Contributions are welcome! Whether you're adding features, fixing bugs, or improving documentation, your help is vital to Lumina's growth.  
Refer to the [CONTRIBUTING](CONTRIBUTING.md) file for guidelines.  

---

## Getting Started  

To start developing with Lumina:  
- Run the **Editor** application from the provided Visual Studio solution.  

Stay tuned for additional documentation and tutorials as the engine evolves.  
