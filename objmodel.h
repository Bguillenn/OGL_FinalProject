#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <string>
#include <vector>
#include "glm/glm.hpp"

class OBJModel
{
public:
    using Vertices = std::vector<glm::vec3>;
    using Textures = std::vector<glm::vec2>;
    using Normals = std::vector<glm::vec3>;
    using Indices = std::vector<unsigned int>;

    const Vertices& GetVertices() const;
    const Normals& GetNormals() const;
    const Textures& GetTextures() const;
    const Indices& GetVertexIndices() const;
    const Indices& GetTextureIndices() const;
    const Indices& GetNormalIndices() const;

    static OBJModel loadOBJ(const std::string& filename, Vertices& vertices, Normals& normals, Textures& textures);

    OBJModel(const std::string& filename);

private:
    Indices vertexIndices, uvIndices, normalIndices;
    Vertices temp_vertices;
    Textures temp_uvs;
    Normals temp_normals;
};

#endif // OBJMODEL_H
