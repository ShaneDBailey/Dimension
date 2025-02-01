#include "Screen.h"
#include "Model.h"

Screen::Screen() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            zBuffer[i][j] = std::numeric_limits<float>::max();
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



void Screen::render_model(const Model& model) {

    Vector3 light_direction{1, 0, -1}; //must be between -1 and 1
    light_direction = normalize(light_direction);


    for(const auto& face : model.get_faces()){
        //grab vertices, and transform them so that the camera is at 0,0,0, converted to homenzgous vector 4 for matrix math
        Vector4 vertex_0_4 = matrix_transform(camera.getViewMatrix(), to_vector4(model.get_vertices()[face.vertex_index[0] - 1])); 
        Vector4 vertex_1_4 = matrix_transform(camera.getViewMatrix(), to_vector4(model.get_vertices()[face.vertex_index[1] - 1])); 
        Vector4 vertex_2_4 = matrix_transform(camera.getViewMatrix(), to_vector4(model.get_vertices()[face.vertex_index[2] - 1])); 

        // project the coned frustrum so it is in a cube shape, this will give the appearance of objects closer to camera being bigger
        //and objects farther away being smaller
        vertex_0_4 = matrix_transform(camera.getProjectionMatrix(), vertex_0_4);
        vertex_1_4 = matrix_transform(camera.getProjectionMatrix(), vertex_1_4);
        vertex_2_4 = matrix_transform(camera.getProjectionMatrix(), vertex_2_4);

        //TODO:: Decide clip logic -> create the multiple triangles, and then for each new triangle do the following below

        // normalizes the cube to be a 1 by 1 by 1
        Vector3 vertex_0 = {vertex_0_4.x / vertex_0_4.w, vertex_0_4.y / vertex_0_4.w, vertex_0_4.z / vertex_0_4.w};
        Vector3 vertex_1 = {vertex_1_4.x / vertex_1_4.w, vertex_1_4.y / vertex_1_4.w, vertex_1_4.z / vertex_1_4.w};
        Vector3 vertex_2 = {vertex_2_4.x / vertex_2_4.w, vertex_2_4.y / vertex_2_4.w, vertex_2_4.z / vertex_2_4.w};

        //maps the 1 by 1 by cuber to the screen
        float z_0 = dot_product(camera.getForward(), vertex_0); 
        float z_1 = dot_product(camera.getForward(), vertex_1);
        float z_2 = dot_product(camera.getForward(), vertex_2);

        vertex_0.z = z_0;
        vertex_1.z = z_1;
        vertex_2.z = z_2;

        vertex_0 = Vector3(
            (vertex_0.x + 1.0f) * SCREEN_WIDTH / 2,
            (1.0f - vertex_0.y) * SCREEN_HEIGHT / 2,
            vertex_0.z
        );
        vertex_1 = Vector3(
            (vertex_1.x + 1.0f) * SCREEN_WIDTH / 2,
            (1.0f - vertex_1.y) * SCREEN_HEIGHT / 2,
            vertex_1.z
        );
        vertex_2 = Vector3(
            (vertex_2.x + 1.0f) * SCREEN_WIDTH / 2,
            (1.0f - vertex_2.y) * SCREEN_HEIGHT / 2,
            vertex_2.z
        );

        //decide the color for the face
        const Vector3& face_normal = model.get_normals()[face.normal_index[0] - 1];
        float brightness = dot_product(light_direction, face_normal);
        brightness = std::max(0.0f, brightness);  
        Color color = face.face_material.diffuse_color;
        color.r *= brightness;
        color.g *= brightness;
        color.b *= brightness;
        SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);


        //if minimizes the check area to be on the screen and no bigger than the triangle this is for efficenacy
        int min_x = std::max(0, std::min({static_cast<int>(vertex_0.x), static_cast<int>(vertex_1.x), static_cast<int>(vertex_2.x)}));
        int min_y = std::max(0, std::min({static_cast<int>(vertex_0.y), static_cast<int>(vertex_1.y), static_cast<int>(vertex_2.y)}));
        int max_x = std::min(SCREEN_WIDTH - 1, std::max({static_cast<int>(vertex_0.x), static_cast<int>(vertex_1.x), static_cast<int>(vertex_2.x)}));
        int max_y = std::min(SCREEN_HEIGHT - 1, std::max({static_cast<int>(vertex_0.y), static_cast<int>(vertex_1.y), static_cast<int>(vertex_2.y)}));

         for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                if (is_point_inside_triangle(x, y, vertex_0, vertex_1, vertex_2)) {//check if the pixel from the area to render is in the triangle
                    float z = barycentric_interpolation(x, y, vertex_0, vertex_1, vertex_2);//determine z depth on all points as only the vertexes have a z value
                    if (z < zBuffer[x][y]) {//check the zbuffer if its on top render that pixel and store it
                        zBuffer[x][y] = z;
                        SDL_RenderDrawPointF(renderer, x, y);
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
