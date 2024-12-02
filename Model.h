#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include "Utilities.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>


struct Vertex_Texture {
    float start, end;
};


struct Material{
    std::string name;
    Color ambientColor;
    Color diffuseColor;
    Color specularColor;
    Color emissiveColor;

    float specularExponent;
    float opticalDensity;
    float dissolveFactor;
    int illuminationModel;
};

struct Face { //triangles only
    int vertexIndex[3]; 
    int textureIndex[3]; 
    int normalIndex[3]; 
    Material& face_material;

    Face(Material& material) : face_material(material) {}
};

class Model {
private:
    std::vector<Vector3> vertices;
    std::vector<Vertex_Texture> textures;
    std::vector<Vector3> normals;
    std::vector<Material> materials;
    std::vector<Face> faces;

    Vector3 center_of_origin;

public:
    Model();
    Model(const std::string& objFilePath, const std::string& mtlFilePath) ;
    void normalize_vertices();
    void find_origin();
    void rotate(float x, float y, float z);
    void translate(float x, float y, float z) ;
    void scaleToRange(float range);
    void find_furthest_point();

    float furthest_point;

    const std::vector<Vector3>& getVertices() const;
    const std::vector<Vertex_Texture>& getTextures() const;
    const std::vector<Vector3>& getNormals() const;
    const std::vector<Face>& getFaces() const;
    const Vector3& getCenterOfOrigin() const;
};

#endif // MODEL_H
