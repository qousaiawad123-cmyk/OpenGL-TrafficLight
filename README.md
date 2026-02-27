This project is a 2D graphical simulation developed using Modern OpenGL.
It demonstrates the integration of multiple graphical concepts to create a dynamic scene featuring a functional traffic light system set against a rainy background.
🚀 Technical Features
The project strictly follows the core concepts of the OpenGL pipeline:

Vertex Customization (VBO & VAO): Managed 5 different Vertex Array Objects for organized rendering of scene elements.

Dynamic Shaders: * Vertex Shader: Handles the real-time rain animation using a mathematical mod function for seamless looping.

Fragment Shader: Implements time-based logic to toggle traffic light states (Red, Yellow, Green) without external libraries.

Uniforms: Utilized for passing time data, object types, and state switches from CPU to GPU.

Alpha Blending: Enabled transparency for a realistic rain effect over the scene.

🛠️ Tools Used
Language: C++

Graphics API: OpenGL 3.3 (Core Profile)

Libraries: GLFW (Window management), GLEW (Extension loading).

💡 How it Works
Rain Logic: Points are randomly generated and shifted downwards based on glfwGetTime(). When a drop hits the bottom, it resets to the top automatically.

Traffic Logic: A 6-second cycle is calculated in the shader. Each light (Red, Yellow, Green) glows for 2 seconds respectively.
