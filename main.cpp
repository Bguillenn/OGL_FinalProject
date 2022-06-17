#include <iostream>
#include <vector>

//LIBRERIAS OPENGL
#include <GL/glew.h> //Magia de OpenGL
#include <GLFW/glfw3.h> //Manejo de ventanas y teclad
#include <glm/glm.hpp> //Matematicas 3D

//LIBRERIAS PROPIAS
#include "objfile.h"

int main()
{
    //INICIALIZAMOS GLFW
    if(!glfwInit())
    {
        std::cerr << "ERROR: Failed to initialize GLFW \n";
        return -1;
    }




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
        glfwTerminate();
        return -1;
    }

    /*
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID); */

    //Arreglo de 3 vectores que representan 3 vertices
    /*std::vector<glm::vec3> vertices = {
                                        glm::vec3(0.0f, 0.5f, 0.0f),
                                        glm::vec3(0.5f, -0.5f, 0.0f),
                                        glm::vec3(-0.5f, -0.5f, 0.0f)
                                        }; */

    //LEEMOS ARCHIVO OBJ Y ESTABLECEMOS LOS VERTICES EN BASE A LOS INDICES
    std::string filename = "/home/kali/stanford-bunny.obj";
    OBJFile model{filename};
    OBJFile::Vertices all_vertices = model.GetVertices();
    OBJFile::Indices faces = model.GetIndices();

    OBJFile::Vertices vertices{};

    //CREAMOS LOS VERTICES

    for(int i=0; i<faces.size(); i++){

        int index = faces[i][0] - 1;

        vertices.push_back(all_vertices[index] * 10.0f);
    }

    const unsigned int n_ver = vertices.size();
    std::cout << n_ver << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * n_ver, &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, nullptr);

    //SHADERS

    //Realizamos la creacion de los shader VertexShader y FragmentShader
    const char* vertexShaderSrc =
                "#version 400\n"
                "in vec4 position;"
                "void main()"
                "{"
                     "gl_Position = position;"
                "}";

    const char* fragmentShaderSrc =
                "#version 400\n"
                "out vec4 color;"
                "void main()"
                "{"
                     "color = vec4(0.5, 0.0, 0.0, 1.0);"
                "}";

    //Creamos los objetos que compilaran los shaders con la funcion CreateShader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //Establecemos el codigo fuente al objeto
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //Establecemos el codigo fuente al objeto
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
    glCompileShader(fragmentShader);


    //Creamos un objeto de programa donde se uniran todos los shaders
    GLuint shaderProgram = glCreateProgram();

    //Utilizando la funcion attach unimos los shaders al programa creado
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);


    //Enlazamos el programa con los shaders e indicamos que sea utilizado
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    //END SHADERS


    //FISICAS
    std::vector<glm::vec3> velocities(n_ver, glm::vec3(0.0f));
    std::vector<float> mass(n_ver, 1.0f);

    glm::vec3 gravity(0.0f, -0.98f, 0.0f);

    float h = 0.0002;
    float time = 0;




    //Capturamos la tecla escape para cerrar la ventana
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {


        glClearColor(0.25, 0.25, 0.25, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        //ACTUALIZAMOS LAS FISICAS
        for(unsigned int i=0; i <velocities.size(); i++)
            velocities[i] = velocities[i] + h*(gravity/mass[i]);
        for(unsigned int i=0; i<vertices.size(); i++)
            vertices[i] = vertices[i] + velocities[0] * time;

        time+=h;



        glBufferData(GL_ARRAY_BUFFER, n_ver * 3 * sizeof(float)  , &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, n_ver);


        //Intercambiamos buffers y escuchamos eventos de la ventana
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
