#include "Model.h"

Model::Model() {
    
}

Model::Model(const std::string& objFilePath, const std::string& mtlFilePath) {
    std::ifstream objFile(objFilePath);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << objFilePath << std::endl;
        return;
    }

    std::ifstream mtlFile(mtlFilePath);
    if (!mtlFile.is_open()) {
        std::cerr << "Failed to open MTL file: " << mtlFilePath << std::endl;
        objFile.close(); // Close the OBJ file before returning
        return;
    }

    // Parse the MTL file
    Material currentMaterial;
    std::string line;
    while (std::getline(mtlFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "newmtl") {
            if (!currentMaterial.name.empty()) {
                materials.push_back(currentMaterial);
            }
            currentMaterial = Material();
            iss >> currentMaterial.name;
        } else if (token == "Kd") {
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial.diffuseColor = Color{r, g, b, 1.0f}; // Assuming full opacity
        } else if (token == "Ka") {
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial.ambientColor = Color{r, g, b, 1.0f}; // Assuming full opacity
        } else if (token == "Ks") {
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial.specularColor = Color{r, g, b, 1.0f}; // Assuming full opacity
        } else if (token == "Ke") {
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial.emissiveColor = Color{r, g, b, 1.0f}; // Assuming full opacity
        } else if (token == "Ns") {
            iss >> currentMaterial.specularExponent;
        } else if (token == "Ni") {
            iss >> currentMaterial.opticalDensity;
        } else if (token == "d") {
            iss >> currentMaterial.dissolveFactor;
        } else if (token == "illum") {
            iss >> currentMaterial.illuminationModel;
        } // Add more properties as needed
    }
    if (!currentMaterial.name.empty()) {
        materials.push_back(currentMaterial);
    }
    mtlFile.close();

    // Parse the OBJ file
    Material* currentMaterialPtr = nullptr; // Pointer to the current material
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "usemtl") {
            std::string materialName;
            iss >> materialName;
            // Find the material with the specified name
            for (auto& mat : materials) {
                if (mat.name == materialName) {
                    currentMaterialPtr = &mat; // Set the pointer to the current material
                    break;
                }
            }
        } else if (token == "v") {
            Vector3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (token == "vt") {
            Vertex_Texture texture;
            iss >> texture.start >> texture.end;
            textures.push_back(texture);
        } else if (token == "vn") {
            Vector3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (token == "f") {
            if (!currentMaterialPtr) {
                std::cerr << "No material specified for face. Skipping." << std::endl;
                continue;
            }

            // Temporary vectors to store indices
            std::vector<int> temp_vertex_indexes;
            std::vector<int> temp_texture_indexes;
            std::vector<int> temp_normal_indexes;

            char separator;
            int vertexIndex, textureIndex, normalIndex;

            while (iss >> vertexIndex >> separator >> textureIndex >> separator >> normalIndex) {
                temp_vertex_indexes.push_back(vertexIndex);
                temp_texture_indexes.push_back(textureIndex);
                temp_normal_indexes.push_back(normalIndex);
            }

            // Create a new face for each vertex index
            if (!temp_vertex_indexes.empty()) {
                int numVertices = temp_vertex_indexes.size();
                for (int i = 2; i < numVertices; ++i) {
                    Face face(*currentMaterialPtr);
                    face.vertexIndex[0] = temp_vertex_indexes[0];
                    face.textureIndex[0] = temp_texture_indexes[0];
                    face.normalIndex[0] = temp_normal_indexes[0];

                    face.vertexIndex[1] = temp_vertex_indexes[i - 1];
                    face.textureIndex[1] = temp_texture_indexes[i - 1];
                    face.normalIndex[1] = temp_normal_indexes[i - 1];
                    
                    face.vertexIndex[2] = temp_vertex_indexes[i];
                    face.textureIndex[2] = temp_texture_indexes[i];
                    face.normalIndex[2] = temp_normal_indexes[i];
                    faces.push_back(face);
                }
            }
        }

    }
    objFile.close();
    find_origin();
}

void Model::find_furthest_point(){
    for(auto& vertex : this->vertices){
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.x));
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.y));
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.z));
    }
}

void Model::normalize_vertices(){
    for(auto& vertex : this->vertices){
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.x));
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.y));
        this->furthest_point = std::max(this->furthest_point, std::abs(vertex.z));
    }

    // Normalize each coordinate of the vertices
    for(auto& vertex : this->vertices){
        vertex.x /= this->furthest_point;
        vertex.y /= this->furthest_point;
        vertex.z /= this->furthest_point;

        vertex.x = vertex.x*SCREEN_WIDTH/4 + SCREEN_WIDTH/2;
        vertex.y = vertex.y*SCREEN_WIDTH/4 + SCREEN_WIDTH/2;
        vertex.z = vertex.z*SCREEN_WIDTH/4 + SCREEN_WIDTH/2;
    }
}

void Model::find_origin() {
    float origin_x = 0, origin_y = 0, origin_z = 0;

    for (auto& vertex : this->vertices) {
        origin_x += vertex.x;
        origin_y += vertex.y;
        origin_z += vertex.z;
    }
    this->center_of_origin = {
        origin_x / this->vertices.size(),
        origin_y / this->vertices.size(),
        origin_z / this->vertices.size()
    };
}

void Model::rotate(float x, float y, float z){
    for(auto& vertex : this->vertices){
        // Translate to the origin
        vertex.x -= this->center_of_origin.x;
        vertex.y -= this->center_of_origin.y;
        vertex.z -= this->center_of_origin.z;

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

        // Translate back
        vertex.x += this->center_of_origin.x;
        vertex.y += this->center_of_origin.y;
        vertex.z += this->center_of_origin.z;
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

void Model::scale(float scalar){
    for (auto& vertex : this->vertices){
        vertex.x = vertex.x * scalar;
        vertex.y = vertex.y * scalar;
        vertex.z = vertex.z * scalar;
    }
    find_origin();
}

void Model::translate(float x, float y, float z) {
    // Translate all vertices by the given offset
    for (auto& vertex : vertices) {
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
    }
    find_origin();
}



const std::vector<Vector3>& Model::getVertices() const { return vertices; }

const std::vector<Vertex_Texture>& Model::getTextures() const { return textures; }

const std::vector<Vector3>& Model::getNormals() const { return normals;}

const std::vector<Face>& Model::getFaces() const { return faces;}

const Vector3& Model::getCenterOfOrigin() const { return center_of_origin; }
