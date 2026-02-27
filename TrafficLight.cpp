#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <ctime>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//  كود المظللات (Shaders)

// Vertex Shader: مسؤول عن المواقع وتحريك المطر
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform float u_time;\n"
"uniform bool isRain;\n"
"void main()\n"
"{\n"
"   vec2 pos = aPos;\n"
"   if(isRain) {\n"
"       // لحتى ترجع نقطة المطر من اول النافذة \n"
"       pos.y = mod(pos.y - u_time * 1.5 + 1.0, 2.0) - 1.0;\n"
"   }\n"
"   gl_Position = vec4(pos, 0.0, 1.0);\n"
"}\0";

// Fragment Shader: مسؤول عن تلوين الأجزاء ومنطق إشارة المرور
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform float u_time;\n"
"uniform int objectType;\n" // 0: مطر، 1: جسم الإشارة، 2: أحمر، 3: أصفر، 4: أخضر
"void main()\n"
"{\n"
// منحط كل رقم مشان الدلاة يلي استخدمناها
"   float cycle = mod(u_time, 6.0);\n" //دورة زمنية مدتها 6 ثواني
"   if(objectType == 0) FragColor = vec4(0.6, 0.7, 1.0, 0.5);\n" // لون مطر شفاف
"   else if(objectType == 1) FragColor = vec4(0.1, 0.1, 0.1, 1.0);\n" // جسم أسود
"   else if(objectType == 2) FragColor = (cycle < 2.0) ? vec4(1, 0, 0, 1) : vec4(0.2, 0, 0, 1);\n"
"   else if(objectType == 3) FragColor = (cycle >= 2.0 && cycle < 4.0) ? vec4(1, 1, 0, 1) : vec4(0.2, 0.2, 0, 1);\n"
"   else if(objectType == 4) FragColor = (cycle >= 4.0) ? vec4(0, 1, 0, 1) : vec4(0, 0.2, 0, 1);\n"
"}\n\0";

int main()
{
    //  تهيئة المكتبات والنافذة 
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project: Traffic Light in Rain", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    //  بناء برنامج الشيدر 
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // تعريف الرؤوس

    // بيانات المطر نقاط عشوائية
    float rainVertices[600];
    for (int i = 0; i < 600; i++) rainVertices[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

    //  بيانات إشارة المرور أجسام مستطيلة باستخدام Triangle Strip
    float body[] = { -0.2f, 0.7f,  0.2f, 0.7f,  -0.2f, -0.7f,  0.2f, -0.7f };
    float redL[] = { -0.1f, 0.6f,  0.1f, 0.6f,  -0.1f, 0.25f,  0.1f, 0.25f };
    float yelL[] = { -0.1f, 0.15f, 0.1f, 0.15f, -0.1f, -0.25f, 0.1f, -0.25f };
    float greL[] = { -0.1f, -0.35f, 0.1f, -0.35f, -0.1f, -0.60f, 0.1f, -0.60f };

    //  (VAOs & VBOs)
    unsigned int VAO[5], VBO[5];
    glGenVertexArrays(5, VAO);
    glGenBuffers(5, VBO);

    // دالة مساعدة لتجهيز كل كائن 
    auto setup = [&](int i, float* data, int size) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        };

    setup(0, rainVertices, sizeof(rainVertices));
    setup(1, body, sizeof(body));
    setup(2, redL, sizeof(redL));
    setup(3, yelL, sizeof(yelL));
    setup(4, greL, sizeof(greL));

    // تفعيل الشفافية مشان المطر 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // . حلقة الرسم 
    while (!glfwWindowShouldClose(window))
    {
        // لون خلفية ليلية كحلية
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        float time = glfwGetTime();

        // الحصول على مواقع الـ Uniforms
        int timeLoc = glGetUniformLocation(shaderProgram, "u_time");
        int isRainLoc = glGetUniformLocation(shaderProgram, "isRain");
        int typeLoc = glGetUniformLocation(shaderProgram, "objectType");

        glUniform1f(timeLoc, time);

        //   رسم المطر خلف الإشارة
        glUniform1i(isRainLoc, 1);
        glUniform1i(typeLoc, 0);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_POINTS, 0, 300); // رسم المطر كنقاط

        //   رسم إشارة المرور بدون تحريك المطر
        glUniform1i(isRainLoc, 0);

        // رسم جسم الإشارة
        glUniform1i(typeLoc, 1);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // رسم الأضواء تتبدل ألوانها تلقائياً بالشيدر
        for (int i = 2; i <= 4; i++) {
            glUniform1i(typeLoc, i);
            glBindVertexArray(VAO[i]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //  التنظيف 
    glDeleteVertexArrays(5, VAO);
    glDeleteBuffers(5, VBO);
    glfwTerminate();
    return 0;
}