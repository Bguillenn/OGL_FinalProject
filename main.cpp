#include <iostream>
#include <vector>

//LIBRERIAS OPENGL
#include <GL/glew.h> //Magia de OpenGL
#include <GLFW/glfw3.h> //Manejo de ventanas y teclad
#include <glm/glm.hpp> //Matematicas 3D
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//LIBRERIAS PROPIAS
#include "objfile.h"

void drawWalls()
{

    glColor3f(1,0,0);
    glPushMatrix();
    //glRotatef(0,0,0,1);
    //glScalef(2,1,2);
    glBegin(GL_QUADS);
    /* Floor */
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1,1);
    glVertex3f(1,-1,1);
    glVertex3f(1,-1,-1);

    glEnd();

    glPopMatrix();

}

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


    //LEEMOS ARCHIVO OBJ Y ESTABLECEMOS LOS VERTICES EN BASE A LOS INDICES
    std::string filename = "/home/denis/stanfordBunny.obj";
    OBJFile model{filename};
    OBJFile::Vertices all_vertices = model.GetVertices();
    OBJFile::Normals all_normals = model.GetNormals();
    OBJFile::Indices faces = model.GetIndices();

    OBJFile::Vertices vertices{};
    OBJFile::Normals normals{};

    //CARGAMOS LOS VERTICES Y NORMALES
    for(int i=0; i<faces.size(); i++){
        int indexVertex = faces[i][0] - 1;
        int indexNormal = faces[i][2] - 1;
        vertices.push_back(all_vertices[indexVertex] * 30.0f);
        normals.push_back(all_normals[indexNormal] * 30.0f);
    }



    const unsigned int n_ver = vertices.size();
    const unsigned int n_nor = normals.size();
    std::cout <<"N. VERTICES: " << n_ver << std::endl;
    std::cout <<"N. NORMALES: " << n_nor << std::endl;

    GLuint vbo[2];
    glGenBuffers(1, &vbo[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * n_ver, &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vbo[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * n_nor, &normals[0], GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, nullptr);

    //SHADERS

    //Realizamos la creacion de los shader VertexShader y FragmentShader
    const char* vertexShaderSrc =
                        "#version 400\n"
                        "uniform mat4 projMat;" //projection
                        "uniform mat4 mvMat;" // model-view

                        "layout (location=0) in vec3 vertPos;"
                        "layout (location=1) in vec3 vertNormal;"

                        "out vec4 varyingColor;"

                        "struct PositionalLight"
                        "{ vec4 ambient;"
                        "vec4 diffuse;"
                        "vec4 specular;"
                        "vec3 position;"
                        "};"

                        "struct Material"
                        "{ vec4 ambient;"
                        "vec4 diffuse;"
                        "vec4 specular;"
                        "float shininess;"
                        "};"


                        "uniform vec4 globalAmbient;"
                        "uniform PositionalLight light;"
                        "uniform Material material;"
                        "uniform mat4 norm_matrix;" // for transforming normals

                        "void main()"
                        "{"

                        "vec4 P = mvMat * vec4(vertPos,1.0);"

                        "vec3 N = normalize((norm_matrix * vec4(vertNormal,1.0)).xyz);"
                        "vec3 L = normalize(light.position - P.xyz);"
                        "vec3 V = normalize(-P.xyz);"
                        "vec3 R = reflect(-L,N);"

                        "vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;"
                        "vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(dot(N,L), 0.0);"
                        "vec3 specular= material.specular.xyz * light.specular.xyz * pow(max(dot(R,V), 0.0f), material.shininess);"
                        "varyingColor = vec4((ambient + diffuse + specular), 1.0);"
                        "gl_Position = projMat*mvMat*vec4(vertPos, 1.0);"

                        "}";

    const char* fragmentShaderSrc =
                        "#version 400\n"
                        "out vec4 color;"
                        "in vec4 varyingColor;"
                        "void main()"
                        "{"
                        "color = varyingColor;"
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

    //END SHADER

    //Valores de giro de la cámara
    float cameraX = 2.f;
    float cameraY = 0.f;
    float cameraZ = 7.f;

    glm::mat4 mMat, vMat, mvMat, pMat;
    float angle=0;

    //FISICAS
    std::vector<glm::vec3> velocities(n_ver, glm::vec3(0.0f));
    std::vector<float> mass(n_ver, 1.0f);

    glm::vec3 gravity(0.0f, -0.98f, 0.0f);

    float h = 0.0002;
    float time = 0;

    int count=0;
    int count2=0;

    // LUCES

    //projection

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect =static_cast<float>(width)/static_cast<float>(height);
    glm::mat4 projMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    GLuint projLoc = glGetUniformLocation(shaderProgram, "projMat");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    //light


    // SILVER material - ambient, diffuse, specular, and shininess
    //material
    float matAmbSilver[4] = {0.1923f, 0.1923f, 0.1923f, 1}; //RGB-A
    float matDifSilver[4] = {0.5075f, 0.5075f, 0.5075f, 1};
    float matSpeSilver[4] = {0.5083f, 0.5083f, 0.5083f, 1};

    // GOLD
    /*
    float matAmbSilver[4] = {0.24725f,  0.1995f,   0.0745f, 1}; //RGB-A
    float matDifSilver[4] = {0.75164f,  0.60648f,  0.22648f, 1};
    float matSpeSilver[4] = {0.628281f, 0.555802f, 0.366065f, 1};
    */
    float matShiSilver = 51.2f;

    //light source
    // white light properties
    float globalAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f };

    float lightAmbient[4]  = {0.0f, 0.0f, 0.0f, 1.0f };
    float lightDiffuse[4]  = {1.0f, 1.0f, 1.0f, 1.0f };
    float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


    glm::vec3 currentLightPos, lightPosV; // light position as Vector3f, in both model and view space
    float lightPos[3];


    //Capturamos la tecla escape para cerrar la ventana
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window))
    {


        glClearColor(0.25, 0.25, 0.25, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);


        //TRATAMOS LAS LUCES

        GLuint mvLoc = glGetUniformLocation(shaderProgram, "mvMat");

        glm::mat4 viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -7.f)); //cam/>for all objects (scene)
        auto modelMatPy = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -2.f, 0.f)); //obj->pyramid
        auto mvMat = viewMat*modelMatPy;

        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));



        //light pyramid


        GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
        globalAmbLoc = glGetUniformLocation(shaderProgram, "globalAmbient");
        ambLoc = glGetUniformLocation(shaderProgram, "light.ambient");
        diffLoc = glGetUniformLocation(shaderProgram, "light.diffuse");
        specLoc = glGetUniformLocation(shaderProgram, "light.specular");
        posLoc = glGetUniformLocation(shaderProgram, "light.position");
        mAmbLoc = glGetUniformLocation(shaderProgram, "material.ambient");
        mDiffLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
        mSpecLoc = glGetUniformLocation(shaderProgram, "material.specular");
        mShiLoc = glGetUniformLocation(shaderProgram, "material.shininess");
        GLuint nLoc = glGetUniformLocation(shaderProgram, "norm_matrix");

        currentLightPos = glm::vec3(5.0f, 5.0f, 2.0f);
        lightPosV = glm::vec3(viewMat * glm::vec4(currentLightPos, 1.0));
        lightPos[0] = lightPosV.x;
        lightPos[1] = lightPosV.y;
        lightPos[2] = lightPosV.z;

        glm::mat4 invTrMat = glm::transpose(glm::inverse(mvMat)); //model-view for normals



        glProgramUniform4fv(shaderProgram, globalAmbLoc, 1, globalAmbient);
        glProgramUniform4fv(shaderProgram, ambLoc, 1, lightAmbient);
        glProgramUniform4fv(shaderProgram, diffLoc, 1, lightDiffuse);
        glProgramUniform4fv(shaderProgram, specLoc, 1, lightSpecular);
        glProgramUniform3fv(shaderProgram, posLoc, 1, lightPos);
        glProgramUniform4fv(shaderProgram, mAmbLoc, 1, matAmbSilver);
        glProgramUniform4fv(shaderProgram, mDiffLoc, 1, matDifSilver);
        glProgramUniform4fv(shaderProgram, mSpecLoc, 1, matSpeSilver);
        glProgramUniform1f(shaderProgram, mShiLoc, matShiSilver);
        glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));



        //ACTUALIZAMOS LAS FISICAS
        /*
        for(unsigned int i=0; i <velocities.size(); i++)
            velocities[i] = velocities[i] + h*(gravity/mass[i]);
        for(unsigned int i=0; i<vertices.size(); i++)
            vertices[i] = vertices[i] + velocities[0] * time;
        */

        glm::vec3 velocity;
        float k = 1.0f;
        float damping = 0.1f;
        float dt = 0.01f;

        //spring and dampers
        if (count < 6){
            count++;
            for (int i=0;i<vertices.size();i++){

                glm::vec3 gravity = glm::vec3(0, -0.98, 0)*dt;
                glm::vec3 spring = gravity*(glm::vec3(0,0,0))*k*dt;
                glm::vec3 dampingForce = velocity*damping;

                //Calculate velocity
                glm::vec3 acceleration = (gravity+spring-dampingForce)/mass[1];
                velocity += acceleration;
                vertices[i] += velocity;
            }
        }
        else if (count >=6){
            count2++;

            for (int i=0;i<vertices.size();i++){

                glm::vec3 gravity = glm::vec3(0, -0.98, 0)*dt;
                glm::vec3 spring = gravity*(glm::vec3(0,0,0))*k*dt;
                glm::vec3 dampingForce = velocity*damping;

                //Calculate velocity
                glm::vec3 acceleration = (gravity+spring-dampingForce)/mass[1];
                velocity += acceleration;
                vertices[i] -= velocity;
            }
            if (count2>=6){
                count=0;
            }
        }
        //springs and dampers

        time+=h;

        //Rotate camera

        //Obtener las matrices

        GLuint projLoc  = glGetUniformLocation(shaderProgram, "projMat");


        //Obtener la perspectiva actual
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = static_cast<float>(width)/static_cast<float>(height);
        pMat =  glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

        //Obtener matriz modelo-vista
        vMat = glm::translate(glm::mat4(1.f), glm::vec3(-cameraX, -cameraY, -cameraZ));

        //Rotar cámara
        glm::mat4 vRot  = glm::rotate(glm::mat4(1.f) , angle , glm::vec3(0, -1,0));
        vMat =  vMat*vRot;

        glm::mat4 mOrg = glm::translate(glm::mat4(1.f), glm::vec3(1.0f, -1.0f, 1.0f)); //offset
        glm::mat4 mBack = glm::translate(glm::mat4(1.f), glm::vec3(-1.0f, 1.0f, -1.0f)); //offset

        glm::mat4 mRot  = mBack*glm::rotate(mOrg, angle , glm::vec3(1  , 0, 0 ));

        //aumentar el angulo y volver a 0 si se llega a 360
        angle+=0.05;
        if(angle>360) angle=0;

        mMat = glm::translate(glm::mat4(1.f), glm::vec3(0.0, -2.0, 0.0))*mRot; //offset

        mvMat = vMat * mMat;

        glUniformMatrix4fv(mvLoc, 1, GL_FALSE,  glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE,  glm::value_ptr(pMat));
        //Rotate camera

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //VERTICES POSITIONS
        glBufferData(GL_ARRAY_BUFFER, n_ver*3 * sizeof(float)  , &vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); //NORMALS POSITIONS
        glBufferData(GL_ARRAY_BUFFER, n_nor*3 * sizeof(float)  , &normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);


        glDrawArrays(GL_TRIANGLES, 0, n_ver);


        drawWalls();

        //Intercambiamos buffers y escuchamos eventos de la ventana
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
