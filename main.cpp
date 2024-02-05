#include <GL/glew.h>

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <format>


std::string MatrixVect(float &&a, float &&b, float &&c)
{
    //glm::mat4 matrix(1.0f); // Создание матрицы 3x3
    glm::mat3 myMatrix({2,4,-1}, {-1,2,1}, {3,0,1});
    //glm::vec3 myVector(1,2,-1);
    glm::vec3 myVector(a,b,c);
    glm::vec3 result = myMatrix * myVector; // Умножение матрицы на вектор
    return (std::format("Vector: ({},{},{})", result.x, result.y, result.z));
}


bool rotateflag = false;

const char* vertexShaderSource1 = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 position;
    uniform float angle;
    void main()
    {
        float x = position.x * cos(angle) - position.y * sin(angle);
        float y = position.x * sin(angle) + position.y * cos(angle);
        gl_Position = vec4(x, y, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
)";


int main()
{
    //Инициализация GLFW
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);                // 4x Сглаживание
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  //Версия OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Не используем старый OpenGL


    //Размеры окна
    const GLuint WIDTH = 1024, HEIGHT = 768;

    //Создание окна приложения
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Create Line and Rotate", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Инициализация GLEW
    glewExperimental = true; // Флаг необходим в Core-режиме OpenGL
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    //Определение размеров вывода на плоскость экрана
    glViewport(0, 0, WIDTH, HEIGHT);

    // Компиляция и связывание шейдеров
    // Создание вершинного шейдера
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Создание фрагментного шейдера
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Создание шейдерной программы
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Удаление шейдеров
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Создание буфера вершин и массива атрибутов вершин
    // Определение вершин отрезка
    float vertices[] = {
        -0.5f, -0.5f,
        0.5f,  0.8f
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    //Инициализация интерфейса ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Режим отслеживания нажатия клавиш в приложении
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //glm::vec3 myVector(1,2,-1);
    char textBuffer[64] = "Vector: (1,2,-1)";

    //Основной цикл, при нажатии на ECS или закрытии приложения работа завершается
    while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           !glfwWindowShouldClose(window))
    {
        //Очиста буфера цвета с одновременным определением цвета фона
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Запуск нового окна ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // Render
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);


        glDrawArrays(GL_LINES, 0, 2);

        if (rotateflag == true)
        {
            // Установка угла вращения
            float angle = glfwGetTime();
            glUniform1f(glGetUniformLocation(shaderProgram, "angle"), angle);
        }


        //Элементы интерфейса в окне
        ImGui::Text("Hello, ImGui!");
        ImGui::Text("%s",textBuffer);
        if (ImGui::Button("Press Me"))
        {
            //Vec(1,2,-1);
            auto str = MatrixVect(1,2,-1);
            ImGui::Text("Hello, ImGui!");
            snprintf(textBuffer, sizeof(textBuffer),  + str.data());
            rotateflag = rotateflag != false ? false : true;
        }

        //Отображение ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //Обмен данными между передним и задним буфером
        glfwSwapBuffers(window);
        //Опрос и обработка событий
        glfwPollEvents();
    }

    //Освобождение ресурсов ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //Освобождение ресурсов
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    //Уничтожение контекста GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
