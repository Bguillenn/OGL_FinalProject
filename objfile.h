#ifndef OBJFILE_H
#define OBJFILE_H

#include <vector>
#include <string>
#include <glm/glm.hpp>


class OBJFile
{
public:
    using Vertices = std::vector<glm::vec3>;
    using Normals = std::vector<glm::vec3>;
    using Indices = std::vector<std::vector<unsigned short>>;

    OBJFile(const std::string& filename);
    const Vertices& GetVertices() const;
    const Normals& GetNormals() const;
    const Indices& GetIndices() const;
private:
    Vertices m_VertexPositions;
    Normals m_Normals;
    Indices m_Indices;
};

#endif // OBJFILE_H
