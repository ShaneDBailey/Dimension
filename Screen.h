#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "Model.h"
#include "Utilities.h"
#include "Camera.h"


float dot_product(const Vector3& v1, const Vector3& v2);
Vector3 cross_product(const Vector3& v1, const Vector3& v2);
bool is_point_inside_triangle(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2);
float barycentric_interpolation(int x, int y, const Vector3& v0, const Vector3& v1, const Vector3& v2);

class Screen {
private:
    SDL_Event event;
    SDL_Window* window;
    std::vector<SDL_FPoint> points;
    float zBuffer[SCREEN_WIDTH][SCREEN_HEIGHT];

public:
    Camera camera = Camera(Vector3(15, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1), 45.0f, 1.0f , 0.5f, 10.0f);
    SDL_Renderer* renderer;
    Screen();
    ~Screen();
    void render_model(const Model& model);
    
    void clear_display();
    void input();
    void lookAround();
};

#endif // SCREEN_H
