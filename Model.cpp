#include "Model.h"

void Model::find_origin() {
    Vector3 origin;

    for (auto& vertex : this->vertices) {
        origin.x += vertex.x;
        origin.y += vertex.y;
        origin.z += vertex.z;
    }
    this->center_of_origin = {
        origin.x / this->vertices.size(),
        origin.y / this->vertices.size(),
        origin.z / this->vertices.size()
    };
}

//-------------------------------------Model_Transforms------------------------------------------
void Model::rotate(float x, float y, float z){
    for(auto& vertex : this->vertices){
        // X rotation
        float temp_y = vertex.y;
        vertex.y = std::cos(x) * vertex.y - std::sin(x) * vertex.z;
        vertex.z = std::sin(x) * temp_y + std::cos(x) * vertex.z;

        // Y rotation
        float temp_x = vertex.x;
        vertex.x = std::cos(y) * vertex.x + std::sin(y) * vertex.z;
        vertex.z = -std::sin(y) * temp_x + std::cos(y) * vertex.z;

        // Z rotation
        temp_x = vertex.x;
        vertex.x = std::cos(z) * vertex.x - std::sin(z) * vertex.y;
        vertex.y = std::sin(z) * temp_x + std::cos(z) * vertex.y;
    }

    for(auto& normal : this->normals){
        // X rotation
        float temp_y = normal.y;
        normal.y = std::cos(x) * normal.y - std::sin(x) * normal.z;
        normal.z = std::sin(x) * temp_y + std::cos(x) * normal.z;

        // Y rotation
        float temp_x = normal.x;
        normal.x = std::cos(y) * normal.x + std::sin(y) * normal.z;
        normal.z = -std::sin(y) * temp_x + std::cos(y) * normal.z;

        // Z rotation
        temp_x = normal.x;
        normal.x = std::cos(z) * normal.x - std::sin(z) * normal.y;
        normal.y = std::sin(z) * temp_x + std::cos(z) * normal.y;
    }
}

void Model::rotate_around_point(float x, float y, float z, Vector3 point){
    for(auto& vertex : this->vertices){
        vertex.x -= point.x;
        vertex.y -= point.y;
        vertex.z -= point.z;
    }
    rotate(x,y,z);
    for(auto& vertex : this->vertices){
        vertex.x += point.x;
        vertex.y += point.y;
        vertex.z += point.z;
    }


}

void Model::scale(float scalar){
    for (auto& vertex : this->vertices){
        vertex.x = vertex.x * scalar;
        vertex.y = vertex.y * scalar;
        vertex.z = vertex.z * scalar;
    }
    find_origin();
}

void Model::translate(float x, float y, float z) {
    for (auto& vertex : vertices) {
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
    }
    find_origin();
}

//-------------------------------------Getters----------------------------------------------------
const std::vector<Vector3>& Model::get_vertices() const { return vertices;}
const std::vector<std::vector<int>>& Model::get_vertex_info() const {return vertices_info;};
const std::vector<Vector3>& Model::get_normals() const { return normals;}
const std::vector<Face>& Model::get_faces() const { return faces;}
std::vector<Material> Model::get_materials() const {return materials;}
const std::vector<Vertex_Texture>& Model::get_textures() const { return textures;}

const Vector3& Model::get_center_of_origin() const { return center_of_origin;}

//--------------------------------------Adders----------------------------------------------------
void Model::add_vertex(Vector3 vertex){this->vertices.push_back(vertex);}
void Model::add_face(Face face){this->faces.push_back(face);}
void Model::add_normal(Vector3 normal){this->normals.push_back(normal);}
void Model::add_material(Material material){this->materials.push_back(material);}
void Model::add_texture(Vertex_Texture vertex_texture){this->textures.push_back(vertex_texture);}


void Model::add_vertex_face_info(int vertex_index, int face_index) {
    // If the vertex_index is greater than or equal to the current size,
    // resize the vector to make sure it can accommodate the new vertex_index
    if (vertex_index >= vertices_info.size()) {
        vertices_info.resize(vertex_index + 1); // Resize to accommodate the new index
    }

    // Add the face index to the specified vertex
    vertices_info[vertex_index].push_back(face_index);
}

