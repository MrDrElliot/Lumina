Lumina Game Engine

Lumina is a lightweight and flexible game engine built to simplify game development while offering powerful tools for creators. Designed with accessibility and efficiency in mind, Lumina empowers developers to bring their creative visions to life.

What’s Included
	•	Core Engine Features: Serialization, math utilities, logging, and other foundational systems.
	•	Graphics Rendering: A simple yet effective 3D rendering engine with Vulkan as the primary graphics API for cutting-edge performance.
	•	Editor with ImGui Integration: A user-friendly editor interface built with ImGui for intuitive workflows.
	•	Cross-platform Support (Planned): Future support for macOS and Linux in addition to Windows.

Features
	•	Vulkan and OpenGL Support: Vulkan is prioritized for its modern rendering techniques, with optional OpenGL support for flexibility.
	•	User-Friendly Editor: A built-in editor application powered by ImGui, providing a seamless interface for asset creation and level design.
	•	Third-party Integrations: Built with key libraries, including:
	•	ImGui for GUI elements.
	•	GLFW for window and input management.
	•	Vulkan for high-performance rendering.

Requirements
	•	Visual Studio 2022 (or compatible build system).
	•	Python for script automation.
	•	Third-party Dependencies: Preconfigured libraries (ImGui, GLFW, Vulkan).

Building Lumina
	1.	Clone the repository and ensure all dependencies are included.
	2.	Run the Python script in the Scripts directory to set up the project.
	3.	Open the Visual Studio solution and set the Editor project as the default startup project.
	4.	Build and run the solution.

Applications
	•	Editor: The main application for designing levels and assets.
	•	Game Client: A standalone runtime for playing your game.

Roadmap
	•	Windows Support First: Initial focus on Windows to establish a solid foundation.
	•	Cross-platform Expansion: Planned support for macOS and Linux.
	•	Advanced Rendering Features: Integration of modern graphics techniques with Vulkan.
	•	User-defined Plugins: Extensible plugin system for custom game logic.

License

Lumina is licensed under the MIT License, allowing free use, modification, and distribution of the engine’s source code.

Contributions

Contributions are welcome! Whether you’re adding features, fixing bugs, or improving documentation, your help is vital to Lumina’s growth. Refer to the CONTRIBUTING file for guidelines.

Getting Started

To start developing with Lumina, run the Editor application from the provided Visual Studio solution. Stay tuned for additional documentation and tutorials as the engine evolves.