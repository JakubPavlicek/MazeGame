#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <utility>
#include <vector>
#include <stack>

#include <ft2build.h>
#include FT_FREETYPE_H
//#include FT_GLYPH_H

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "Maze.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static float posX = 0.0f, posY = 0.0f;

static int WIDTH = 640, HEIGHT = 480;

std::pair<int, int> DisableWindowAwayPos()
{
    if(posX < 0.0f)    { posX = 0.0f;   }
    if(posX > 620.0f)  { posX = 620.0f; }
    if(posY < 0.0f)    { posY = 0.0f;   }
    if(posY > 460.0f)  { posY = 460.0f; }

    return { posX, posY };
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    DisableWindowAwayPos();

    switch(key)
    {
    default:
        if(action == GLFW_PRESS || action == GLFW_REPEAT)
            std::cout << "spatne tlacitko" << std::endl;
        break;

    case GLFW_KEY_ESCAPE:
            exit(0);
            
    case GLFW_KEY_UP:
        switch(action)
        {
            case GLFW_PRESS:
                std::cout << "Nahoru" << std::endl;
                posY += 23.0f;
                break;
            case GLFW_REPEAT:
                std::cout << "Nahoru - opak" << std::endl;
                posY += 23.0f;
                break;
        }
        break;

    case GLFW_KEY_DOWN:
        switch(action)
        {
            case GLFW_PRESS:
                std::cout << "Dolu" << std::endl;
                posY -= 23.0f;
                break;
            case GLFW_REPEAT:
                std::cout << "Dolu - opak" << std::endl;
                posY -= 23.0f;
                break;
        }
        break;
            
    case GLFW_KEY_LEFT:
        switch(action)
        {
            case GLFW_PRESS:
                std::cout << "Doleva" << std::endl;
                posX -= 23.0f;
                break;
            case GLFW_REPEAT:
                std::cout << "Doleva - opak" << std::endl;
                posX -= 23.0f;
                break;
        }
        break;
            
    case GLFW_KEY_RIGHT:
        switch(action)
        {
            case GLFW_PRESS:
                std::cout << "Doprava" << std::endl;
                posX += 23.0f;
                break;
            case GLFW_REPEAT:
                std::cout << "Doprava - opak" << std::endl;
                posX += 23.0f;
                break;
        }
    break;
    }
}

void key_callback_end_state(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE || key == GLFW_KEY_ENTER || key == GLFW_KEY_SPACE)
        exit(0);
}

int main()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Maze Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    const GLubyte* shading_language = glGetString(GL_SHADING_LANGUAGE_VERSION); // version as a string

    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);
    printf("Supported GLSL version is %s.\n", shading_language);
    //
    
    glfwSetKeyCallback(window, key_callback);
    
    {
        
    float end_triangles[] = {
        620.0f, 460.0f, 0.0f, 0.0f,
        640.0f, 460.0f, 1.0f, 0.0f,
        640.0f, 480.0f, 1.0f, 1.0f,
        620.0f, 480.0f, 0.0f, 1.0f
    };
        
    float path_triangles[] = {
        0.0f,  0.0f,  0.0f, 0.0f,
        20.0f, 0.0f,  1.0f, 0.0f,
        20.0f, 20.0f, 1.0f, 1.0f,
        0.0f,  20.0f, 0.0f, 1.0f
    };
        
    float cell_triangles[] = {
        0.0f,  0.0f,  0.0f, 0.0f,
        20.0f, 0.0f,  1.0f, 0.0f,
        20.0f, 3.0f, 1.0f, 1.0f,
        0.0f,  3.0f, 0.0f, 1.0f
    };
    
    float cell_triangles2[] = {
        0.0f, 0.0f,  0.0f, 0.0f,
        3.0f, 0.0f,  1.0f, 0.0f,
        3.0f, 20.0f, 1.0f, 1.0f,
        0.0f, 20.0f, 0.0f, 1.0f
    };
        
    float text_triangles[] = {
        20.0f,  160.0f, 0.0f, 0.0f,
        100.0f, 160.0f, 1.0f, 0.0f,
        100.0f, 320.0f, 1.0f, 1.0f,
        20.0f,  320.0f, 0.0f, 1.0f,
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        
    VertexArray vao1, vao2, vao3, vao4, vao5, vao6;
    VertexBuffer vbo1(nullptr, 4 * 4 * sizeof(float), GL_DYNAMIC_DRAW);
    VertexBuffer vbo2(end_triangles, 4 * 4 * sizeof(float), GL_STATIC_DRAW);
    VertexBuffer vbo3(path_triangles, 4 * 4 * sizeof(float), GL_STATIC_DRAW);
    VertexBuffer vbo4(text_triangles, 4 * 4 * sizeof(float), GL_STATIC_DRAW);
    VertexBuffer vbo5(cell_triangles, 4 * 4 * sizeof(float), GL_STATIC_DRAW);
    VertexBuffer vbo6(cell_triangles2, 4 * 4 * sizeof(float), GL_STATIC_DRAW);
    VertexBufferLayout layout1, layout2, layout3, layout4, layout5, layout6;
    IndexBuffer ibo(indices, 6);
        
    vao1.Bind();
    vbo1.Bind();
    layout1.PushFloat(2);
    layout1.PushFloat(2);
    vao1.AddBuffer(vbo1, layout1);
    ibo.Bind();

    vao2.Bind();
    vbo2.Bind();
    layout2.PushFloat(2);
    layout2.PushFloat(2);
    vao2.AddBuffer(vbo2, layout2);
    ibo.Bind();

    vao3.Bind();
    vbo3.Bind();
    layout3.PushFloat(2);
    layout3.PushFloat(2);
    vao3.AddBuffer(vbo3, layout3);
    ibo.Bind();
        
    vao4.Bind();
    vbo4.Bind();
    layout4.PushFloat(2);
    layout4.PushFloat(2);
    vao4.AddBuffer(vbo4, layout4);
    ibo.Bind();
    
    vao5.Bind();
    vbo5.Bind();
    layout5.PushFloat(2);
    layout5.PushFloat(2);
    vao5.AddBuffer(vbo5, layout5);
    ibo.Bind();
    
    vao6.Bind();
    vbo6.Bind();
    layout6.PushFloat(2);
    layout6.PushFloat(2);
    vao6.AddBuffer(vbo6, layout6);
    ibo.Bind();
        
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));   // aspect ratio, prespective
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // camera position, orientation
        
    Shader shader("OpenGL_tutorial/basic.shader");
    Shader shader_maze("OpenGL_tutorial/maze.shader");
    shader_maze.Bind();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WIDTH), 0.0f, static_cast<float>(HEIGHT));
    glUniformMatrix4fv(glGetUniformLocation(shader_maze.GetShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    shader.Bind();
        
    Texture texture_player("OpenGL_tutorial/pepega.png");
    Texture texture_end("OpenGL_tutorial/end.png");
    Texture Y("OpenGL_tutorial/abeceda/y.png");
    Texture O("OpenGL_tutorial/abeceda/o.png");
    Texture U("OpenGL_tutorial/abeceda/u.png");
    Texture W("OpenGL_tutorial/abeceda/w.png");
    Texture I("OpenGL_tutorial/abeceda/i.png");
    Texture N("OpenGL_tutorial/abeceda/n.png");
        
    Texture texture_white("OpenGL_tutorial/white.jpg");
    Texture texture_blue("OpenGL_tutorial/blue.png");
    Texture texture_red("OpenGL_tutorial/red.png");

    texture_player.Bind();
    shader.SetUniform1i("u_Texture", 0);

    Renderer renderer;
        
    renderer.InitializeFreetype();
        
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
        
    glm::vec3 translation(0.0f, 0, 0);
    
// MAZE
        Maze maze(644, 483);
        
        maze.MakeMaze();
// END OF MAZE
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation); // object position, rotation, scale
        glm::mat4 mvp = proj * view * model;
        
        float player_triangles[] = {
            0.0f + (posX), 0.0f + (posY), 0.0f, 0.0f,
            20.0f + (posX), 0.0f + (posY), 1.0f, 0.0f,
            20.0f + (posX), 20.0f + (posY), 1.0f, 1.0f,
            0.0f + (posX), 20.0f + (posY), 0.0f, 1.0f
        };
        
        shader_maze.Bind();
        
        shader.Bind();
        
        vbo1.Bind();
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * 4 * sizeof(float), player_triangles));
        
        shader.SetUniformMat4f("u_MVP", mvp);
// MAZE
        maze.DrawMaze(texture_white, texture_blue, vao3, ibo);
        
// PLAYER AND END
        shader.SetUniformMat4f("u_MVP", mvp);
        model = glm::translate(glm::mat4(1.0f), translation); // object position, rotation, scale
        mvp = proj * view * model;
        
        texture_end.Bind();
        renderer.Draw(vao2, ibo, shader);
        
        texture_player.Bind();
        renderer.Draw(vao1, ibo, shader);

        // Win
        if(posX >= 617.0f && posY >= 457.0f)
        {
            renderer.ClearWholeScreen();
            glfwSetKeyCallback(window, key_callback_end_state);
            
        // "YOU WIN" text

            model = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 0.0f, 0));
            glm::mat4 mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            Y.Bind();
            renderer.Draw(vao4, ibo, shader);

            model = glm::translate(glm::mat4(1.0f), glm::vec3(130.0f, 0.0f, 0));
            mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            O.Bind();
            renderer.Draw(vao4, ibo, shader);

            model = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 0.0f, 0));
            mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            U.Bind();
            renderer.Draw(vao4, ibo, shader);

            model = glm::translate(glm::mat4(1.0f), glm::vec3(310.0f, 20.0f, 0));
            mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            W.Bind();
            renderer.Draw(vao4, ibo, shader);

            model = glm::translate(glm::mat4(1.0f), glm::vec3(390.0f, 20.0f, 0));
            mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            I.Bind();
            renderer.Draw(vao4, ibo, shader);

            model = glm::translate(glm::mat4(1.0f), glm::vec3(470.0f, 20.0f, 0));
            mvp_text = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp_text);
            N.Bind();
            renderer.Draw(vao4, ibo, shader);
        }
        else
        {
            glfwSetKeyCallback(window, key_callback);
        }
        
        {
            ImGui::SliderFloat3("Translation", &translation.x, 0, 640.0f);
            ImGui::SliderFloat2("PosX", &posX, 0.0f, 640.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    return 0;
}
