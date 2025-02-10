#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "Model.h"
#include "Utilities.h"
#include "Camera.h"

class Screen {
private:
    SDL_Event event;
    SDL_Window* window;
    std::vector<SDL_FPoint> points;
    float z_buffer[SCREEN_WIDTH][SCREEN_HEIGHT];

public:
    Camera camera;
    Vector3 light_direction;
    SDL_Renderer* renderer;
    Screen();
    ~Screen();
    
    void clear_display();
    void input();

    void render_model(const Model& model);
    void render_model_gourand(const Model& model);
};

#endif // SCREEN_H
