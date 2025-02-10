#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities.h"  
#include <cmath>
#include <iostream>

struct Frustum{
    Vector3 far_corners[4];
    Vector3 close_corners[4];
};

class Camera {
private:
    Vector3 position;   //position in world space 
    Vector3 up;          //direction in world space
    Vector3 forward;     //vector in which the camera is looking
    Vector3 right;       
    Matrix4 view_matrix;      //the view in which objects are bigger the closer they are to the camera
    Matrix4 projection_matrix; //adjusts the frusturm so it appears to look like a cube and that objects are not distorted
    float fov;           
    float aspect_ratio;   
    float near_plane;     
    float far_plane;   
    Frustum viewing_volume;   

public:
    Camera();
    Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspectRatio, float nearPlane, float farPlane);

    const Vector3& get_position() const { return position; }
    const Vector3& get_up() const { return up; }
    const Vector3& get_forward() const { return forward; }
    const Vector3& get_right() const { return right; }
    Matrix4 get_view_matrix() const{return view_matrix;}
    Matrix4 get_projection_matrix() const {return projection_matrix;}
    float get_FOV() const { return fov; }
    float get_aspect_ratio() const { return aspect_ratio; }
    float get_near_plane() const { return near_plane; }
    float get_far_plane() const { return far_plane; }
    Frustum get_viewing_volume() const{return viewing_volume;}

    void set_position(const Vector3& new_position) { position = new_position; }
    void set_forward(const Vector3& new_forward) { forward = new_forward; }
    void set_up(const Vector3& new_up) { up = new_up; }
    void set_right(const Vector3& new_right) { right = new_right; }

    void define_viewing_volume(); 
    void set_view_matrix();
    void set_projection_matrix();
    void update_views();

    void print_frustum_world_bounds();
};

#endif
