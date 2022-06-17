#include "objfile.h"
#include <fstream>
#include <sstream>

OBJFile::OBJFile(const std::string& filename)
{
    std::ifstream fileStream{filename, std::ios_base::in};

    if(fileStream.is_open())
    {
        while (!fileStream.eof())
        {
            std::string line;
            getline(fileStream, line);

            std::stringstream lineStream{ line };

            std::string firstSymbol;
            lineStream >> firstSymbol;

            //VERTICES
            if(firstSymbol == "v")
            {
                std::vector<float> vertex{};
                float vertexPosition{};

                for(unsigned int i=0; i<3; i++)
                {
                    lineStream >> vertexPosition;
                    vertex.push_back(vertexPosition);
                }

                glm::vec3 vertexVec{vertex[0], vertex[1], vertex[2]};
                m_VertexPositions.push_back(vertexVec);
            }
            //NORMALS
            else if(firstSymbol == "vn")
            {
                std::vector<float> normal{};
                float normalPosition{};

                for(unsigned i=0; i<3; i++)
                {
                    lineStream >> normalPosition;
                    normal.push_back(normalPosition);
                }
                glm::vec3 normalVec{normal[0], normal[1], normal[2]};
                m_Normals.push_back(normalVec);
            }
            //Indices
            else if(firstSymbol == "f")
            {

                unsigned short index{};

                char separator;

                for(unsigned int i=0; i<3; i++)
                {
                    std::vector<unsigned short> indices{};
                    for(unsigned int j=0; j<3; j++)
                    {
                        lineStream >> index;
                        indices.push_back(index);

                        if(j<2) lineStream >> separator;
                    }
                    //Append to list indices
                    m_Indices.push_back(indices);
                }
            }
        }
        fileStream.close();
    }
}

const OBJFile::Vertices& OBJFile::GetVertices() const
{
    return m_VertexPositions;
}


const OBJFile::Normals& OBJFile::GetNormals() const
{
    return m_Normals;
}

const OBJFile::Indices& OBJFile::GetIndices() const
{
    return m_Indices;
}
