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
    Matrix4 viewMatrix;      //the view in which objects are bigger the closer they are to the camera
    Matrix4 projectionMatrix; //adjusts the frusturm so it appears to look like a cube and that objects are not distorted
    float fov;           
    float aspectRatio;   
    float nearPlane;     
    float farPlane;   
    Frustum ViewingVolume;   

public:
    Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspectRatio, float nearPlane, float farPlane);

    const Vector3& getPosition() const { return position; }
    const Vector3& getForward() const { return forward; }
    const Vector3& getUp() const { return up; }
    const Vector3& getRight() const { return right; }
    Matrix4 getViewMatrix() const{return viewMatrix;}
    Matrix4 getProjectionMatrix() const {return projectionMatrix;}
    float getFOV() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }
    Frustum getViewingVolume() const{return ViewingVolume;}

    void setPosition(const Vector3& newPosition) { position = newPosition; }
    void setForward(const Vector3& newForward) { forward = newForward; }
    void setUp(const Vector3& newUp) { up = newUp; }
    void setRight(const Vector3& newRight) { right = newRight; }

    void defineViewingVolume(); 
    void setViewMatrix();
    void setProjectionMatrix();
    void updateViews();


    bool is_point_in_viewing_volume(const Vector3& point) const;
    void printFrustumWorldBounds();
};

#endif
