
#include "Loader.h"

Model Loader::load_obj(const std::string& obj_file_path, const std::string& mtl_file_path) {
    std::ifstream objFile(obj_file_path);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << obj_file_path << std::endl;
        return Model(); 
    }

    std::ifstream mtlFile(mtl_file_path);
    if (!mtlFile.is_open()) {
        std::cerr << "Failed to open MTL file: " << mtl_file_path << std::endl;
        objFile.close(); 
        return Model();
    }

    Model parsing_model;


    Material current_material;
    std::string line;
    while (std::getline(mtlFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "newmtl") {
            if (!current_material.name.empty()) {
                parsing_model.add_material(current_material);
            }
            current_material = Material();
            iss >> current_material.name;
        } else if (token == "Kd") {
            float r, g, b;
            iss >> r >> g >> b;
            current_material.diffuse_color = Color{r, g, b, 1.0f}; 
        } else if (token == "Ka") {
            float r, g, b;
            iss >> r >> g >> b;
            current_material.ambient_color = Color{r, g, b, 1.0f}; 
        } else if (token == "Ks") {
            float r, g, b;
            iss >> r >> g >> b;
            current_material.specular_color = Color{r, g, b, 1.0f};
        } else if (token == "Ke") {
            float r, g, b;
            iss >> r >> g >> b;
            current_material.emissive_color = Color{r, g, b, 1.0f};
        } else if (token == "Ns") {
            iss >> current_material.specular_exponent;
        } else if (token == "Ni") {
            iss >> current_material.optical_density;
        } else if (token == "d") {
            iss >> current_material.dissolve_factor;
        } else if (token == "illum") {
            iss >> current_material.illumination_model;
        } 
    }
    if (!current_material.name.empty()) {
        parsing_model.add_material(current_material);
    }
    mtlFile.close();


    Material* current_material_pointer = nullptr; 
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "usemtl") {
            std::string materialName;
            iss >> materialName;

            for (auto& mat : parsing_model.get_materials()) {
                if (mat.name == materialName) {
                    current_material_pointer = &mat; 
                    break;
                }
            }
        } else if (token == "v") {
            Vector3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            parsing_model.add_vertex(vertex);
        } else if (token == "vt") {
            Vertex_Texture texture;
            iss >> texture.start >> texture.end;
            parsing_model.add_texture(texture);
        } else if (token == "vn") {
            Vector3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            parsing_model.add_normal(normal);
        } else if (token == "f") {
            if (!current_material_pointer) {
                std::cerr << "No material specified for face. Skipping." << std::endl;
                continue;
            }

            // Temporary vectors to store indices
            std::vector<int> temp_vertex_indexes;
            std::vector<int> temp_texture_indexes;
            std::vector<int> temp_normal_indexes;

            char separator;
            int vertex_index, texture_index, normal_index;

            while (iss >> vertex_index >> separator >> texture_index >> separator >> normal_index) {
                temp_vertex_indexes.push_back(vertex_index);
                temp_texture_indexes.push_back(texture_index);
                temp_normal_indexes.push_back(normal_index);
            }

            // Create a new face for each vertex index
            if (!temp_vertex_indexes.empty()) {
                int numVertices = temp_vertex_indexes.size();
                for (int i = 2; i < numVertices; ++i) {
                    Face face(*current_material_pointer);
                    face.vertex_index[0] = temp_vertex_indexes[0] - 1;
                    parsing_model.add_vertex_face_info(temp_vertex_indexes[0] - 1, temp_normal_indexes[0] - 1);

                    face.texture_index[0] = temp_texture_indexes[0] - 1;
                    face.normal_index[0] = temp_normal_indexes[0] - 1;

                    face.vertex_index[1] = temp_vertex_indexes[i - 1] - 1;
                    parsing_model.add_vertex_face_info(temp_vertex_indexes[i-1] - 1, temp_normal_indexes[i-1] - 1);
                    face.texture_index[1] = temp_texture_indexes[i - 1] - 1;
                    face.normal_index[1] = temp_normal_indexes[i - 1] - 1;
                    
                    face.vertex_index[2] = temp_vertex_indexes[i] - 1;
                    parsing_model.add_vertex_face_info(temp_vertex_indexes[i] - 1, temp_normal_indexes[i] - 1);
                    face.texture_index[2] = temp_texture_indexes[i] - 1;
                    face.normal_index[2] = temp_normal_indexes[i] - 1;
                    parsing_model.add_face(face);
                }
            }
        }

    }
    objFile.close();
    parsing_model.find_origin();

    return parsing_model;
}

