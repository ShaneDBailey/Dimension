#include "Camera.h"

Camera::Camera() {}
Camera::Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspect_ratio, float near_plane, float far_plane)
    : position(position), up(up), forward(forward), fov(fov), aspect_ratio(aspect_ratio), near_plane(near_plane), far_plane(far_plane) {

    right = normalize(cross_product(forward, up));
    update_views();
}

void Camera::set_view_matrix() {
    forward = normalize(forward);
    right = normalize(cross_product(forward, up));
    up = normalize(cross_product(right, forward));

    // Calculate the translation values
    float tx = -dot_product(right, position);
    float ty = -dot_product(up, position);
    float tz = dot_product(forward, position);

    // Construct the view matrix
    this->view_matrix = Matrix4(
        right.x, right.y, right.z, tx,  // First column: Right vector and translation
        up.x, up.y, up.z, ty,  // Second column: Up vector and translation
        -forward.x, -forward.y, -forward.z, tz,  // Third column: Forward vector and translation (negated for camera space)
        0.0f, 0.0f, 0.0f, 1.0f  // Fourth column: Homogeneous coordinate (for 3D transformation)
    );
}

void Camera::set_projection_matrix() {
    float tan_half_fov = tanf(fov / 2.0f);  // Half of the FOV in radians
    float z_range = far_plane - near_plane;

    // Construct the projection matrix
    projection_matrix = Matrix4(
        1.0f / (aspect_ratio * tan_half_fov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tan_half_fov, 0.0f, 0.0f,
        0.0f, 0.0f, (far_plane + near_plane) / z_range, -1.0f,
        0.0f, 0.0f, 2.0f * far_plane * near_plane / z_range, 0.0f
    );
}

void Camera::define_viewing_volume() {
    // We calculate the corners of the frustum based on the camera's position, the view direction, and the near/far planes
    float tan_half_fov = tanf(fov / 2.0f);
    float near_height = 2.0f * tan_half_fov * near_plane;
    float near_width = near_height * aspect_ratio;
    float far_height = 2.0f * tan_half_fov * far_plane;
    float far_width = far_height * aspect_ratio;

    // Calculate the direction vectors for the near and far planes
    Vector3 center_near = position + forward * near_plane;
    Vector3 center_far = position + forward * far_plane;

    // Calculate the four corners of the near plane
    viewing_volume.close_corners[0] = center_near + (right * near_width * 0.5f) - (up * near_height * 0.5f);
    viewing_volume.close_corners[1] = center_near + (right * near_width * 0.5f) + (up * near_height * 0.5f);
    viewing_volume.close_corners[2] = center_near - (right * near_width * 0.5f) + (up * near_height * 0.5f);
    viewing_volume.close_corners[3] = center_near - (right * near_width * 0.5f) - (up * near_height * 0.5f);

    // Calculate the four corners of the far plane
    viewing_volume.far_corners[0] = center_far + (right * far_width * 0.5f) - (up * far_height * 0.5f);
    viewing_volume.far_corners[1] = center_far + (right * far_width * 0.5f) + (up * far_height * 0.5f);
    viewing_volume.far_corners[2] = center_far - (right * far_width * 0.5f) + (up * far_height * 0.5f);
    viewing_volume.far_corners[3] = center_far - (right * far_width * 0.5f) - (up * far_height * 0.5f);
}

void Camera::update_views() {
    set_view_matrix();
    set_projection_matrix();
    define_viewing_volume();
}

void Camera::print_frustum_world_bounds() {
    std::cout << "Frustum Near Corners:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "Close Corner " << i << ": (" << viewing_volume.close_corners[i].x << ", " 
                  << viewing_volume.close_corners[i].y << ", " << viewing_volume.close_corners[i].z << ")" << std::endl;
    }

    std::cout << "Frustum Far Corners:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "Far Corner " << i << ": (" << viewing_volume.far_corners[i].x << ", " 
                  << viewing_volume.far_corners[i].y << ", " << viewing_volume.far_corners[i].z << ")" << std::endl;
    }
}
