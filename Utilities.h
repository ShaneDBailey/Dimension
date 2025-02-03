#ifndef UTILITIES_H
#define UTILITIES_H

#include <tuple>
#include <cmath>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

struct Color {
    float r, g, b, a;

    Color operator+(const Color& other) const {
        return Color{
            r + other.r, 
            g + other.g, 
            b + other.b, 
            a + other.a
        };
    }

    Color operator*(float scalar) const {
        return Color{
            r * scalar, 
            g * scalar, 
            b * scalar, 
            a * scalar
        };
    }
    Color operator/(float scalar) const {
        return Color{
            r / scalar, 
            g / scalar, 
            b / scalar, 
            a / scalar
        };
    }
    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "(" << color.r << ", " << color.g << ", " << color.b << ")";
        return os;
    }
};



struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator-() const {
        return {-x, -y, -z};
    }
    Vector3 operator+() const {
        return {x, y, z};
    }
    Vector3 operator+(const Vector3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    Vector3 operator-(const Vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    Vector3 operator/(float scalar) const {
        // Avoid division by zero
        if (scalar != 0) {
            float invScalar = 1.0f / scalar;
            return {x * invScalar, y * invScalar, z * invScalar};
        } else {
            return {0, 0, 0};
        }
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
};

struct Vector4 {
    float x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(1) {}  // Default constructor, w = 1 for normal 3D points
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vector4 operator+(const Vector4& other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    Vector4 operator-(const Vector4& other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    Vector4 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
        os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
        return os;
    }
};

struct Matrix4{
    float matrix[4][4];

    Matrix4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        matrix[0][0] = m00; matrix[0][1] = m01; matrix[0][2] = m02; matrix[0][3] = m03;
        matrix[1][0] = m10; matrix[1][1] = m11; matrix[1][2] = m12; matrix[1][3] = m13;
        matrix[2][0] = m20; matrix[2][1] = m21; matrix[2][2] = m22; matrix[2][3] = m23;
        matrix[3][0] = m30; matrix[3][1] = m31; matrix[3][2] = m32; matrix[3][3] = m33;
    }

    Matrix4() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                matrix[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
            }
        }
    }
    
};



Vector4 matrix_transform(const Matrix4& mat, const Vector4& vec);
Vector4 to_vector4(const Vector3& vec);

float barycentric_interpolation_z_value(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2);
Vector3 barycentric_interpolation_weights(int x, int y, Vector3 vertex_0, Vector3 vertex_1, Vector3 vertex_2);

float dot_product(const Vector3& v1, const Vector3& v2);
Vector3 cross_product(const Vector3& v1, const Vector3& v2);
Vector3 normalize(const Vector3& v);
bool is_point_inside_triangle(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2);
Vector3 reflect(const Vector3& incident, const Vector3& normal);



#endif