## Lumina Game Engine

<img width="256" height="256" alt="svgviewer-png-output" src="https://github.com/user-attachments/assets/552b8ca0-ebca-4876-9c6a-df38c468d41e" />
<img width="2235" height="1229" alt="image" src="https://github.com/user-attachments/assets/b6811315-7097-4e01-a4fe-d3f06f30c252" />
<img width="2238" height="1239" alt="image" src="https://github.com/user-attachments/assets/283c9475-c1b7-4b6b-bbc8-781dc2aeb000" />
<img width="2253" height="1244" alt="image" src="https://github.com/user-attachments/assets/8d588019-6311-4dbd-b3dd-7b5c832321aa" />
<img width="2235" height="1233" alt="image" src="https://github.com/user-attachments/assets/c4ec5ced-4f1c-4387-8836-78c8cbeda179" />


Lumina is a hobby project and a work-in-progress game engine designed as a learning experience. Still in heavy development, Lumina is inspired by established engines like Unreal Engine, Godot, and others, while aiming to strike a balance between power and simplicity. The engine uses reflection to simplify workflows, and while it is intended to be semi-lightweight, the editor will remain a core feature for the foreseeable future.

In the future, the goal is to make it easier to build games without needing to rely on the editor. Written in C++, Lumina follows a very specific and nuanced coding style that most developers will likely find unconventional, if not outright disagreeable. Despite these quirks, the engine aims to offer a unique environment for game development and experimentation.

[Discord](https://discord.gg/xQSB7CRzQE)

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

1. Clone the repository and ensure all dependencies are included.\
2. Run Setup.py, this will extract External.zip into the appropriate place.
3. The system should automatically add an enviorment variable for you at LUMINA_DIR to the install directory. If for some reason it does not, please add it. For example, mine is under User variables, "LUMINA_DIR" - "H:\Lumina"
4. Run the Python script `Win-GenProjects.py` located in the `Scripts` directory to set up the project.
5. Open the generated Visual Studio solution.
6. Run the "Reflector" application under "Release" mode.
7. Set the **Editor** project as the default startup project.
8. Build and run the solution.

**This process is currently temporary, and will improve once focus is shifted away from engine internals**.

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

Lumina is licensed under the [Apache License 2.0](LICENSE), which allows free use, modification, and distribution of the engine's source code for any purpose, including commercial projects. Use of the code requires proper attribution, including preservation of the license and copyright notices.

---

## Contributions

Contributions are welcome! Whether you're adding features, fixing bugs, or improving documentation, your help is vital to Lumina's growth. Please refer to the Pull Request Checklist for guidelines.

---

## Coding Style

## Naming Conventions:
- **Non-reflection classes/structures**: Prefixed with an **"F"** (e.g., `FMyClass`). This may be adjusted in the future, but all non-reflection classes will have some prefix.
- **Reflection classes**: Prefixed with a **"C"** (e.g., `CMyClass`) to signify that they are tied to the reflection system.
- **Reflection Structures**: Prefixed with a **S** (e.g., `SMyStruct`) to signify that they are tied to the reflection system. 
- **Functions and methods**: Use **PascalCase** (e.g., `MyFunctionName()`).
- **Variables**: Use **PascalCase** for variables (e.g., `MyVariable`).
- **Constants**: Use **UPPER_SNAKE_CASE** (e.g., `MY_CONSTANT`).
- **Global variables**: Use a **"G"** prefix to indicate global scope (e.g., `GGlobalVariable`).
- **Namespaces**: Use **PascalCase** (e.g., `MyNamespace`).
- **No prefixes** like `p`, `m_`, or any other convention — only the **"G"** prefix for global variables.

## Brackets and Indentation:
- **Brackets** must be on their **own separate line** (e.g., the opening `{` starts a new line, not on the same line as the function signature).
- **Indentation**: Use **tabs** per indentation level. **No spaces**.

## Templates:
- Templates should be as **restrictive as possible**. **C++ concepts** should be used for template restrictions. Use of `static_assert` to enforce constraints on template parameters is **not permissible**.

## Code Structure:
- **Class Declarations**: Member functions should be declared in header files and defined in source files. Avoid putting implementation details in the header unless it's small, inline, or templated code.
    - All non-trivial member functions must be declared in the `.h` file.
    - Implementations of functions that are trivial (single-line, one-liner) should be in the `.h` file.

## Function and Variable Names:
- **Function Names**: Must be descriptive, use **PascalCase** (e.g., `CalculateDamage()`, `GetPlayerName()`).
- **Variable Names**: Use **PascalCase** (e.g., `PlayerHealth`, `EnemyPosition`).
- **Avoid**: Using single-letter variables except in cases of short loop iterators (e.g., `i`, `j`).

## Comments and Documentation:
- **Class/Method comments**: For all non-trivial classes and methods, provide brief documentation about their purpose and behavior.
- Use **inline comments** only when necessary, to clarify specific parts of the code.

## Memory Management:
- **Ownership** and **lifetime** should be clearly defined, ensuring there are no memory leaks or dangling pointers.

## Error Handling:
- Use **exceptions** sparingly and only in critical failure paths.
- Prefer **error codes** or **result objects** for non-critical error handling.

## Reflection:
- Reflection classes are prefixed with **"C"** to distinguish them.
- Reflection data should be **separated** into their own class or structure, but closely tied to the code they reflect.

## Miscellaneous:
- **No Magic Numbers**: Avoid hard-coding numbers or values directly. Instead, use **constants** or **enum classes**.
- **Prefer `auto`** for variable declarations when the type is clear and self-evident. But do not use auto in times where the type is not readily apparently.
- **Don't use `using namespace`** in header files. In source files, only use it in limited scopes where necessary.

## Code Readability and Style:
- Ensure **consistent naming** and formatting throughout the codebase.
- **Avoid long functions** or functions that perform too many tasks. Keep functions small and focused on a single purpose.
- **Modular design**: Classes should have clear responsibilities. Aim for **low coupling** and **high cohesion**.



## Pull Request Checklist

Before submitting a pull request, ensure the following:

- [ ] Code compiles without errors.
- [ ] No use of generative AI tools (e.g., GitHub Copilot, ChatGPT) in code contributions.
- [ ] Adherence to the project's coding standards and style guidelines.
- [ ] Inclusion of relevant tests for new features or bug fixes.
- [ ] Updates to documentation, if applicable.
- [ ] Clear and descriptive commit messages.

---

Stay tuned for additional documentation and tutorials as the engine evolves.

---

For more information and to access the source code, visit the [Lumina GitHub Repository](https://github.com/mrdrelliot/lumina).
