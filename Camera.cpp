#include "Camera.h"

Camera::Camera(){}
Camera::Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspectRatio, float nearPlane, float farPlane)
    : position(position), up(up), forward(forward), fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
    
    right = normalize(cross_product(forward, up));
    updateViews();
}

void Camera::setViewMatrix() {
    forward = normalize(forward);
    right = normalize(cross_product(forward, up));
    up = normalize(cross_product(right, forward));

    // Calculate the translation values
    float tx = -dot_product(right, position);
    float ty = -dot_product(up, position);
    float tz = dot_product(forward, position);

    // Construct the view matrix
    this->viewMatrix = Matrix4(
        right.x,  right.y,  right.z,  tx,  // First column: Right vector and translation
        up.x,     up.y,     up.z,     ty,  // Second column: Up vector and translation
        -forward.x, -forward.y, -forward.z, tz,  // Third column: Forward vector and translation (negated for camera space)
        0.0f,     0.0f,     0.0f,     1.0f  // Fourth column: Homogeneous coordinate (for 3D transformation)
    );
}

void Camera::setProjectionMatrix() {
    float tanHalfFov = tanf(fov / 2.0f);  // Half of the FOV in radians
    float zRange = farPlane - nearPlane;
    
    // Construct the projection matrix
    projectionMatrix = Matrix4(
        1.0f / (aspectRatio * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, (farPlane + nearPlane) / zRange, -1.0f,
        0.0f, 0.0f, 2.0f * farPlane * nearPlane / zRange, 0.0f
    );
}

void Camera::defineViewingVolume() {
    // We calculate the corners of the frustum based on the camera's position, the view direction, and the near/far planes
    float tanHalfFov = tanf(fov / 2.0f);
    float nearHeight = 2.0f * tanHalfFov * nearPlane;
    float nearWidth = nearHeight * aspectRatio;
    float farHeight = 2.0f * tanHalfFov * farPlane;
    float farWidth = farHeight * aspectRatio;

    // Calculate the direction vectors for the near and far planes
    Vector3 centerNear = position + forward * nearPlane;
    Vector3 centerFar = position + forward * farPlane;

    // Calculate the four corners of the near plane
    ViewingVolume.close_corners[0] = centerNear + (right * nearWidth * 0.5f) - (up * nearHeight * 0.5f);
    ViewingVolume.close_corners[1] = centerNear + (right * nearWidth * 0.5f) + (up * nearHeight * 0.5f);
    ViewingVolume.close_corners[2] = centerNear - (right * nearWidth * 0.5f) + (up * nearHeight * 0.5f);
    ViewingVolume.close_corners[3] = centerNear - (right * nearWidth * 0.5f) - (up * nearHeight * 0.5f);

    // Calculate the four corners of the far plane
    ViewingVolume.far_corners[0] = centerFar + (right * farWidth * 0.5f) - (up * farHeight * 0.5f);
    ViewingVolume.far_corners[1] = centerFar + (right * farWidth * 0.5f) + (up * farHeight * 0.5f);
    ViewingVolume.far_corners[2] = centerFar - (right * farWidth * 0.5f) + (up * farHeight * 0.5f);
    ViewingVolume.far_corners[3] = centerFar - (right * farWidth * 0.5f) - (up * farHeight * 0.5f);
}

void Camera::updateViews(){
    setViewMatrix();
    setProjectionMatrix();
    defineViewingVolume();
}

void Camera::printFrustumWorldBounds() {
    std::cout << "Frustum Near Corners:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "Close Corner " << i << ": (" << ViewingVolume.close_corners[i].x << ", " 
                  << ViewingVolume.close_corners[i].y << ", " << ViewingVolume.close_corners[i].z << ")" << std::endl;
    }

    std::cout << "Frustum Far Corners:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "Far Corner " << i << ": (" << ViewingVolume.far_corners[i].x << ", " 
                  << ViewingVolume.far_corners[i].y << ", " << ViewingVolume.far_corners[i].z << ")" << std::endl;
    }
}