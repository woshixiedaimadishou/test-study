#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void testShader(int Shader);
void vertex_shader_create(unsigned int *vertexShader);
void fragment_shader_create(unsigned int *fragmentShader);
void shaders_link(unsigned int *shaderProgram, unsigned int *vertexShader, unsigned int *fragmentShader);
// 顶点着色器源代码硬编码
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n" // 位置变量的属性值为0
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "out vec3 outColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos,1.0);\n"
                                 "   outColor = aColor;\n"
                                 "}\0";
// 片段着色器源代码硬编码
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 outColor;\n"
                                   "//uniform vec4 time_change_color;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    //FragColor = time_change_color;\n"
                                   "    FragColor = vec4(outColor,1.0);\n"
                                   "}\0";
int main()
{
    // 实例化GLFW窗口
    glfwInit();                                    // 初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);(Mac OS X专属)

    GLFWwindow *window = glfwCreateWindow(800, 600, "hello_opengl", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "error,窗口打开失败\n"
                  << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // 设置窗口为主上下文
    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // gGPA是用来加载系统相关的OpenGL函数指针地址的函数
    {
        std::cout << "error,GLAD初始化失败\n"
                  << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600); // 设置渲染窗口维度
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //
    //
    //
    //  编译顶点着色器
    //   id引用，所以先储存再创建顶点着色器
    unsigned int vertexShader;
    vertex_shader_create(&vertexShader);
    // 编译片段着色器
    unsigned int fragmentShader;
    fragment_shader_create(&fragmentShader);
    // 链接着色器
    unsigned int shaderProgram;
    shaders_link(&shaderProgram, &vertexShader, &fragmentShader);
    // 创建三角形的三个顶点坐标
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
    // 生成带有缓冲id的VBO对象并绑定,创建顶点数组对象VAO
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    // 绑定VAO
    glBindVertexArray(VAO);
    // 顶点数据复制到缓冲内存
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    // 位置属性
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //
    //
    //
    //  渲染循环
    while (!glfwWindowShouldClose(window)) // 检查GLFW是否退出
    {
        // 输入
        processInput(window);
        // 渲染指令
        glClearColor(0.2f, 0.1f, 0.3f, 1.0f); // 填充背景颜色
        glClear(GL_COLOR_BUFFER_BIT);         // 清空屏幕颜色
        // 激活着色器
        glUseProgram(shaderProgram);
        // 随时间更新uniform颜色
        /*
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "time_change_color");
        glUseProgram(shaderProgram);
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        */
        // 画三角形
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0);
        //  检查并调用事件，交换缓冲
        glfwSwapBuffers(window); // 交换颜色缓冲，进行绘制并输出在屏幕上
        glfwPollEvents();        // 检查触发事件
    }
    // 释放内存
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); // 释放资源
    return 0;
}
// 调整窗口函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// 检测ESC键
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }
}
// 检测着色器是否编译成功
void testShader(int Shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(Shader, 512, NULL, infoLog);
        std::cout << "error,shader wrong\n"
                  << infoLog << std::endl;
    }
}
// 编译顶点着色器
void vertex_shader_create(unsigned int *vertexShader)
{
    *vertexShader = glCreateShader(GL_VERTEX_SHADER); // 传递顶点着色器的参数
    // 顶点着色器源码附加，然后编译
    glShaderSource(*vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(*vertexShader);
    // 检测是否编译成功并返回错误信息
    testShader(*vertexShader);
}
// 编译片段着色器
void fragment_shader_create(unsigned int *fragmentShader)
{
    *fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(*fragmentShader);
    testShader(*fragmentShader);
}
// 链接着色器
void shaders_link(unsigned int *shaderProgram, unsigned int *vertexShader, unsigned int *fragmentShader)
{
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, *vertexShader);
    glAttachShader(*shaderProgram, *fragmentShader);
    glLinkProgram(*shaderProgram);
    // 检测链接是否失败
    int success;
    char infoLog[512];
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        std::cout << "error,shader link wrong\n"
                  << infoLog << std::endl;
    }
    // 激活程序对象
    glUseProgram(*shaderProgram);
    // 删除着色器对象
    glDeleteShader(*vertexShader);
    glDeleteShader(*fragmentShader);
}
