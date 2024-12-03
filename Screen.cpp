#include "Screen.h"
#include "Model.h"

Screen::Screen() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            zBuffer[i][j] = std::numeric_limits<float>::min();
        }
    }
}

Screen::~Screen() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Screen::clear_display() {
    SDL_SetRenderDrawColor(renderer, 115,155,155, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            zBuffer[i][j] = std::numeric_limits<float>::max();
        }
    }
}

void Screen::input() {
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            SDL_Quit();
            exit(0);
        }
    }
    //lookAround();
}



/**/

bool Screen::is_point_in_frustum(int x, int y, float z) {
    // Convert the screen-space coordinates (x, y) back to camera space or normalized device coordinates
    // (assuming your frustum checks are done in camera space or NDC).

    // Check if the z value is within the frustum's near and far planes
    if (z < camera.getNearPlane() || z > camera.getFarPlane()) {
        return false;
    }

    // Check the x and y values against the frustum's left, right, top, and bottom planes
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return false;
    }

    // If all checks pass, the point is inside the frustum
    return true;
}

void Screen::render_model(const Model& model) {

    Vector3 light_direction{1, 0, -1}; //must be between -1 and 1
    light_direction = normalize(light_direction);


    for(const auto& face : model.getFaces()){
        Vector3 vertex_0 = matrix_transform(camera.getViewMatrix(),model.getVertices()[face.vertexIndex[0] -1]); 
        Vector3 vertex_1 = matrix_transform(camera.getViewMatrix(),model.getVertices()[face.vertexIndex[1] -1]);
        Vector3 vertex_2 = matrix_transform(camera.getViewMatrix(),model.getVertices()[face.vertexIndex[2] -1]);

        vertex_0 = matrix_transform(camera.getProjectionMatrix(), vertex_0);
        vertex_1 = matrix_transform(camera.getProjectionMatrix(), vertex_1);
        vertex_2 = matrix_transform(camera.getProjectionMatrix(), vertex_2);

        float z_0 = dot_product(camera.getForward(), vertex_0); 
        float z_1 = dot_product(camera.getForward(), vertex_1);
        float z_2 = dot_product(camera.getForward(), vertex_2);

        vertex_0.z = z_0;
        vertex_1.z = z_1;
        vertex_2.z = z_2;

        vertex_0 = Vector3(
            (vertex_0.x * 0.5f + 0.5f) * SCREEN_WIDTH,
            (-vertex_0.y * 0.5f + 0.5f) * SCREEN_HEIGHT,
            vertex_0.z
        );
        vertex_1 = Vector3(
            (vertex_1.x * 0.5f + 0.5f) * SCREEN_WIDTH,
            (-vertex_1.y * 0.5f + 0.5f) * SCREEN_HEIGHT,
            vertex_1.z
        );
        vertex_2 = Vector3(
            (vertex_2.x * 0.5f + 0.5f) * SCREEN_WIDTH,
            (-vertex_2.y * 0.5f + 0.5f) * SCREEN_HEIGHT,
            vertex_2.z
        );

        const Vector3& face_normal = model.getNormals()[face.normalIndex[0] - 1];

        float brightness = dot_product(light_direction, face_normal);
        brightness = std::max(0.0f, brightness);  

        Color color = face.face_material.diffuseColor;
        color.r *= brightness;
        color.g *= brightness;
        color.b *= brightness;

        SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

        int min_x = std::min({vertex_0.x, vertex_1.x, vertex_2.x}); 
        int min_y = std::min({vertex_0.y, vertex_1.y, vertex_2.y}); 
        int max_x = std::max({vertex_0.x, vertex_1.x, vertex_2.x}); 
        int max_y = std::max({vertex_0.y, vertex_1.y, vertex_2.y}); 

        for (int y = min_y; y <= max_y; ++y) { 
            for (int x = min_x; x <= max_x; ++x) { 
                if (is_point_inside_triangle(x, y, vertex_0, vertex_1, vertex_2)) { 
                    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                         if (is_point_inside_triangle(x, y, vertex_0, vertex_1, vertex_2)) {
                            float z = barycentric_interpolation(x, y, vertex_0, vertex_1, vertex_2);
                            if(is_point_in_frustum(x,y,z)){
                                if (z < zBuffer[x][y]) {
                                    zBuffer[x][y] = z;
                                    SDL_RenderDrawPointF(renderer, x, y);
                                }
                            }
                        }
                    }
                } 
            }
        }
    }
}






























void Screen::lookAround() {
    static int lastX = SCREEN_WIDTH / 2;
    static int lastY = SCREEN_HEIGHT / 2;
    
    // Get the current mouse position
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Calculate the difference between the current and previous mouse positions
    float deltaX = mouseX - lastX;
    float deltaY = mouseY - lastY;

    // Update the last mouse position to the current one
    lastX = mouseX;
    lastY = mouseY;

    // Sensitivity to control the speed of rotation
    const float sensitivity = 0.005f;

    // Update camera rotation based on mouse movement
    // Rotate the camera's forward and up directions based on mouse movement
    Vector3 right = camera.getRight();
    Vector3 up = camera.getUp();
    
    // Horizontal rotation (yaw)
    camera.setForward(normalize(camera.getForward() + right * -deltaX * sensitivity));
    
    // Vertical rotation (pitch), making sure the camera can't rotate upside down
    Vector3 newUp = normalize(up + camera.getForward() * deltaY * sensitivity);
    if (dot_product(camera.getForward(), newUp) > 0.99f) {
        newUp = up;  // prevent flipping the camera upside down
    }
    camera.setUp(newUp);

    // Recalculate the camera's view matrix after rotation
    camera.setViewMatrix();
    
    // Optionally, hide the cursor to improve the experience
    SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2); // Center the mouse
}
