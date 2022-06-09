#include <iostream>

//LIBRERIAS OPENGL
#include <GL/glew.h> //Magia de OpenGL
#include <GLFW/glfw3.h> //Manejo de ventanas y teclad
#include <glm/glm.hpp> //Matematicas 3D

//LIBRERIAS PROPIAS

int main()
{
    //INICIALIZAMOS GLFW
    if(!glfwInit())
    {
        std::cerr << "ERROR: Failed to initialize GLFW \n";
        return -1;
    }

    //Configuracion de la ventana GLFW
    glfwWindowHint(GLFW_SAMPLES, 4); //Antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL version mayor
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //OpenGL version menor
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Para que no utilize el viejo OpenGL


    //Creacion de una ventana y su contexto

    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "Trabajo Final", nullptr, nullptr);

    if(window == nullptr)
    {
        std::cerr << "Falla al abrir una ventana con GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        std::cerr << "Fallo al inicializar GLEW \n";
        return -1;
    }

    /*
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID); */

    //Arreglo de 3 vectores que representan 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    //Capturamos la tecla escape para cerrar la ventana
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {

        //Dibujamos el triangulo
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                        0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        (void*)0
                    );

        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        //Intercambiamos buffers y escuchamos eventos de la ventana
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
