/*
Author: Shane Bailey

File Description:
- This file is a layout of the model class, it contains all the components
- a 3D model would have, and associated functions for the 3D model/material

Library Resources:

Important References:
- https://en.wikipedia.org/wiki/Wavefront_.obj_file
*/


#ifndef MODEL_H
#define MODEL_H
//Standard C Libraries
#include <vector>   //lists
#include <cmath>    //additional math functions
#include <iostream> //inpupt and output streams
#include <fstream>  //hangles reading files
#include <sstream>  //operations for strings
#include <string>   //strings
//Created Files
#include "Utilities.h"
//-----------------------------------Data_Structures----------------------
struct Vertex_Texture {
    float start, end;
};

struct Material{
    std::string name;
    Color ambient_color;
    Color diffuse_color;
    Color specular_color;
    Color emissive_color;

    float specular_exponent;
    float optical_density;
    float dissolve_factor;
    int illumination_model;
};

struct Face { 
    int vertex_index[3]; 
    int texture_index[3]; 
    int normal_index[3]; 
    Material& face_material;

    Face(Material& material) : face_material(material) {}
};

//----------------------------------------Model_Class-----------------------------
class Model {
    private:
        std::vector<Vector3> vertices;
        std::vector<std::vector<int>> vertices_info;
        std::vector<Face> faces;
        std::vector<Vector3> normals;
        std::vector<Material> materials;
        std::vector<Vertex_Texture> textures;
        std::string texture_file_path;

        Vector3 center_of_origin;

    public:
        void find_origin();
        
        void rotate(float x, float y, float z);
        void rotate_around_point(float x, float y, float z, Vector3 point);
        void translate(float x, float y, float z) ;
        void scale(float scalar);

        const std::vector<Vector3>& get_vertices() const;
        const std::vector<std::vector<int>>& get_vertex_info() const;
        const std::vector<Face>& get_faces() const;
        const std::vector<Vector3>& get_normals() const;
        std::vector<Material> get_materials() const;
        const std::vector<Vertex_Texture>& get_textures() const;

        const Vector3& get_center_of_origin() const;

        void add_vertex(Vector3 vertex);
        void add_vertex_face_info(int vertex_index, int face_index);
        void add_face(Face face);
        void add_normal(Vector3 normal);
        void add_material(Material material);
        void add_texture(Vertex_Texture vertex_texture);
};

#endif 
