#include "objmodel.h"
#include<string.h>
OBJModel::OBJModel(const std::string& filename)
{
    //Leemos y guardamos los valores del archivo OBJ
    FILE * file = fopen(filename.c_str(), "r");
    if( file != NULL ) {
        while( 1 ){

            char lineHeader[128];
            // Lee la primera palabra de la línea
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break; // EOF = End Of File, es decir, el final del archivo. Se finaliza el ciclo.

            // else : analizar el lineHeader

            if ( strcmp(lineHeader, "v") == 0){
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertices.push_back(vertex);
            }else if( strcmp( lineHeader, "vt" ) == 0 ){
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y );
                temp_uvs.push_back(uv);
            }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches == 9){
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    uvIndices    .push_back(uvIndex[0]);
                    uvIndices    .push_back(uvIndex[1]);
                    uvIndices    .push_back(uvIndex[2]);
                    normalIndices.push_back(normalIndex[0]);
                    normalIndices.push_back(normalIndex[1]);
                    normalIndices.push_back(normalIndex[2]);
                }

            }
        }
    }
}


const OBJModel::Vertices& OBJModel::GetVertices() const
{
    return temp_vertices;
}

const OBJModel::Normals& OBJModel::GetNormals() const
{
    return temp_normals;
}

const OBJModel::Textures& OBJModel::GetTextures() const
{
    return temp_uvs;
}

const OBJModel::Indices& OBJModel::GetVertexIndices() const
{
    return vertexIndices;
}

const OBJModel::Indices& OBJModel::GetTextureIndices() const
{
    return uvIndices;
}

const OBJModel::Indices& OBJModel::GetNormalIndices() const
{
    return normalIndices;
}


OBJModel OBJModel::loadOBJ(const std::string& filename, Vertices& vertices, Normals& normals, Textures& textures)
{
    OBJModel obj{filename};
    Indices vertexIndices = obj.GetVertexIndices();
    Indices normalsIndices = obj.GetNormalIndices();
    Indices texturesIndices = obj.GetTextureIndices();
    Vertices temp_vertices = obj.GetVertices();
    Normals temp_normals = obj.GetNormals();
    Textures temp_textures = obj.GetTextures();

    for(unsigned int i=0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalsIndices[i];
        unsigned int textureIndex = texturesIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];
        glm::vec2 texture = temp_textures[ textureIndex-1 ];

        vertices.push_back(vertex);
        normals.push_back(normal);
        textures.push_back(texture);
    }
    return obj;
}
