#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities.h"  

class Camera {
private:
    Vector3 position;    
    Vector3 up;          
    Vector3 forward;     
    Vector3 right;       
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
    float fov;           
    float aspectRatio;   
    float nearPlane;     
    float farPlane;      

public:
    Camera(Vector3 position, Vector3 up, Vector3 forward, float fov, float aspectRatio, float nearPlane, float farPlane);

    const Vector3& getPosition() const { return position; }
    const Vector3& getForward() const { return forward; }
    const Vector3& getUp() const { return up; }
    const Vector3& getRight() const { return right; }
    float getFOV() const { return fov; }
    float getAspectRatio() const { return aspectRatio; }
    float getNearPlane() const { return nearPlane; }
    float getFarPlane() const { return farPlane; }

    void setPosition(const Vector3& newPosition) { position = newPosition; }
    void setForward(const Vector3& newForward) { forward = newForward; }
    void setUp(const Vector3& newUp) { up = newUp; }
    void setRight(const Vector3& newRight) { right = newRight; }

    void printFrustumWorldBounds();

    void setViewMatrix();
    void setProjectionMatrix();
    void lookAt(const Vector3& target);
    Matrix4 getViewMatrix() const{return viewMatrix;}
    Matrix4 getProjectionMatrix() const {return projectionMatrix;}

};

#endif
