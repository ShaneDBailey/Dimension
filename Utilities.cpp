#include "Utilities.h"

float dot_product(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 cross_product(const Vector3& v1, const Vector3& v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

Vector3 normalize(const Vector3& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / length, v.y / length, v.z / length};
}

bool is_point_inside_triangle(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2) { 
    //barycentric coordinates:: found on internet somewhere
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float gamma = 1.0f - alpha - beta;
    return alpha > 0 && beta > 0 && gamma > 0; 
}

float barycentric_interpolation_z_value(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float gamma = 1.0f - alpha - beta;
    return alpha * v0.z + beta * v1.z + gamma * v2.z;
}

Vector3 barycentric_interpolation_weights(int x, int y, Vector3 vertex_0, Vector3 vertex_1, Vector3 vertex_2){
    //barycentric points, poitns mid way between other points
    Vector3 v0 = vertex_1 - vertex_0;
    Vector3 v1 = vertex_2 - vertex_0;
    Vector3 v2 = Vector3(x, y, 0) - vertex_0;
    float dot_00 = dot_product(v0, v0);
    float dot_01 = dot_product(v0, v1);
    float dot_11 = dot_product(v1, v1);
    float dot_20 = dot_product(v2, v0);
    float dot_21 = dot_product(v2, v1);
    float denominator = dot_00 * dot_11 - dot_01 * dot_01;
    float alpha = (dot_11 * dot_20 - dot_01 * dot_21) / denominator;
    float beta = (dot_00 * dot_21 - dot_01 * dot_20) / denominator;
    float gamma = 1.0f - alpha - beta;

    return Vector3(alpha,beta,gamma);
}

Vector4 matrix_transform(const Matrix4& mat, const Vector4& vec) {
    float x = vec.x * mat.matrix[0][0] + vec.y * mat.matrix[0][1] + vec.z * mat.matrix[0][2] + vec.w * mat.matrix[0][3];
    float y = vec.x * mat.matrix[1][0] + vec.y * mat.matrix[1][1] + vec.z * mat.matrix[1][2] + vec.w * mat.matrix[1][3];
    float z = vec.x * mat.matrix[2][0] + vec.y * mat.matrix[2][1] + vec.z * mat.matrix[2][2] + vec.w * mat.matrix[2][3];
    float w = vec.x * mat.matrix[3][0] + vec.y * mat.matrix[3][1] + vec.z * mat.matrix[3][2] + vec.w * mat.matrix[3][3];
    return Vector4(x, y, z, w);
}

// Optional: Function to convert Vector3 to Vector4
Vector4 to_vector4(const Vector3& vec) {
    return Vector4(vec.x, vec.y, vec.z, 1.0f); 
}


Vector3 reflect(const Vector3& incident, const Vector3& normal) {
    // Calculate the dot product of the incident vector and the normal vector
    float dot_product = incident.dot(normal);

    // Compute the reflection vector
    return incident - normal * (2.0f * dot_product);
}



