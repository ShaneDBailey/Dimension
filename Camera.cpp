#include "Camera.h"

Camera::Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspectRatio, float nearPlane, float farPlane)
    : position(position), up(up), forward(forward), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
    
    // Normalize the forward and up directions first
    forward = normalize(forward);  // Make sure forward is normalized
    up = normalize(up);  // Normalize the up vector
    
    // Calculate the right direction based on forward and up vectors
    right = cross_product(forward, up);
    right = normalize(right);  // Normalize the right vector
    
    // Recalculate up vector to ensure orthogonality
    up = cross_product(right, forward);
    up = normalize(up);  // Ensure the up vector is orthogonal to right and forward
    setViewMatrix();
    //setProjectionMatrix();
}

void Camera::setViewMatrix() {
    // Normalize the direction vectors
    right = normalize(cross_product(forward, up));
    up = normalize(cross_product(right, forward));
    forward = normalize(forward);

    // Calculate the translation values
    float tx = -dot_product(right, position);
    float ty = -dot_product(up, position);
    float tz = dot_product(forward, position);  // Inverted

    // Construct the view matrix
    this->viewMatrix = Matrix4(
        right.x, up.x, -forward.x, 0.0f,
        right.y, up.y, -forward.y, 0.0f,
        right.z, up.z, -forward.z, 0.0f,
        tx, ty, -tz, 1.0f
    );
}

void Camera::setProjectionMatrix() {
    float tanHalfFov = tanf(fov / 2.0f);  // Half of the FOV in radians
    float zRange = farPlane - nearPlane;
    
    // Construct the projection matrix
    projectionMatrix = Matrix4(
        1.0f / (aspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(farPlane + nearPlane) / zRange, -1.0f,
        0.0f, 0.0f, -2.0f * farPlane * nearPlane / zRange, 0.0f
    );
}


void Camera::lookAt(const Vector3& target) {
    forward = normalize((target - position));
    right = normalize(cross_product(forward,up));
    up = normalize(cross_product(right,forward));
    setViewMatrix();
}




#include <cmath>
#include <iostream>

void Camera::printFrustumWorldBounds() {
    // Calculate the half-width and half-height of the frustum at the near plane
    float tanHalfFOV = std::tan(fov * 0.5f * M_PI / 180.0f);
    float nearHeight = nearPlane * tanHalfFOV;
    float nearWidth = nearHeight * aspectRatio;

    // Calculate the half-width and half-height of the frustum at the far plane
    float farHeight = farPlane * tanHalfFOV;
    float farWidth = farHeight * aspectRatio;

    // Camera's position is at (0, 0, -10) and near plane at z = -9.9
    float nearPlanePosition = -10.0f + nearPlane;

    // Frustum corners in camera space (relative to camera position)

    // Near plane (at nearPlanePosition distance)
    Vector3 nearTopLeft(-nearWidth, nearHeight, nearPlanePosition);
    Vector3 nearTopRight(nearWidth, nearHeight, nearPlanePosition);
    Vector3 nearBottomLeft(-nearWidth, -nearHeight, nearPlanePosition);
    Vector3 nearBottomRight(nearWidth, -nearHeight, nearPlanePosition);

    // Far plane (at farPlane distance)
    Vector3 farTopLeft(-farWidth, farHeight, -farPlane);
    Vector3 farTopRight(farWidth, farHeight, -farPlane);
    Vector3 farBottomLeft(-farWidth, -farHeight, -farPlane);
    Vector3 farBottomRight(farWidth, -farHeight, -farPlane);

    // Print the 8 frustum corners in camera space
    std::cout << "Frustum Corners in Camera Space:" << std::endl;
    std::cout << "Near Plane:" << std::endl;
    std::cout << "Top Left: " << nearTopLeft << std::endl;
    std::cout << "Top Right: " << nearTopRight << std::endl;
    std::cout << "Bottom Left: " << nearBottomLeft << std::endl;
    std::cout << "Bottom Right: " << nearBottomRight << std::endl;

    std::cout << "Far Plane:" << std::endl;
    std::cout << "Top Left: " << farTopLeft << std::endl;
    std::cout << "Top Right: " << farTopRight << std::endl;
    std::cout << "Bottom Left: " << farBottomLeft << std::endl;
    std::cout << "Bottom Right: " << farBottomRight << std::endl;

    // Transform the corners to world space using the camera's view matrix
    Vector3 nearTopLeftWorld = matrix_transform(viewMatrix, nearTopLeft);
    Vector3 nearTopRightWorld = matrix_transform(viewMatrix, nearTopRight);
    Vector3 nearBottomLeftWorld = matrix_transform(viewMatrix, nearBottomLeft);
    Vector3 nearBottomRightWorld = matrix_transform(viewMatrix, nearBottomRight);

    Vector3 farTopLeftWorld = matrix_transform(viewMatrix, farTopLeft);
    Vector3 farTopRightWorld = matrix_transform(viewMatrix, farTopRight);
    Vector3 farBottomLeftWorld = matrix_transform(viewMatrix, farBottomLeft);
    Vector3 farBottomRightWorld = matrix_transform(viewMatrix, farBottomRight);

    // Print the 8 frustum corners in world space
    std::cout << "Frustum Corners in World Space:" << std::endl;
    std::cout << "Near Plane:" << std::endl;
    std::cout << "Top Left: " << nearTopLeftWorld << std::endl;
    std::cout << "Top Right: " << nearTopRightWorld << std::endl;
    std::cout << "Bottom Left: " << nearBottomLeftWorld << std::endl;
    std::cout << "Bottom Right: " << nearBottomRightWorld << std::endl;

    std::cout << "Far Plane:" << std::endl;
    std::cout << "Top Left: " << farTopLeftWorld << std::endl;
    std::cout << "Top Right: " << farTopRightWorld << std::endl;
    std::cout << "Bottom Left: " << farBottomLeftWorld << std::endl;
    std::cout << "Bottom Right: " << farBottomRightWorld << std::endl;
}