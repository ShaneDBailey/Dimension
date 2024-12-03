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

float barycentric_interpolation(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y)); 
    float gamma = 1.0f - alpha - beta;
    return alpha * v0.z + beta * v1.z + gamma * v2.z;
}

std::tuple<float, float, float> barycentric_weights(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    float alpha = ((v1.y - v2.y) * (x - v2.x) + (v2.x - v1.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
    float beta = ((v2.y - v0.y) * (x - v2.x) + (v0.x - v2.x) * (y - v2.y)) / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
    float gamma = 1.0f - alpha - beta;
    return std::make_tuple(alpha, beta, gamma);
}

Vector3 matrix_transform(const Matrix4& mat, const Vector3& vec) {
    // Perform matrix-vector multiplication
    // Treat the vector as (x, y, z, 1) for affine transformations
    float x = vec.x * mat.matrix[0][0] + vec.y * mat.matrix[0][1] + vec.z * mat.matrix[0][2] + 1.0f * mat.matrix[0][3];
    float y = vec.x * mat.matrix[1][0] + vec.y * mat.matrix[1][1] + vec.z * mat.matrix[1][2] + 1.0f * mat.matrix[1][3];
    float z = vec.x * mat.matrix[2][0] + vec.y * mat.matrix[2][1] + vec.z * mat.matrix[2][2] + 1.0f * mat.matrix[2][3];
    float w = vec.x * mat.matrix[3][0] + vec.y * mat.matrix[3][1] + vec.z * mat.matrix[3][2] + 1.0f * mat.matrix[3][3];

    // If w is not zero, divide by w to handle perspective projection
    if (w != 0) {
        x /= w;
        y /= w;
        z /= w;
    }

    return Vector3(x, y, z);  // Return the transformed 3D vector
}