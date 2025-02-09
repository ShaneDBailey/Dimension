#include "Screen.h"
#include "Model.h"

Screen::Screen() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        for (int j = 0; j < SCREEN_HEIGHT; ++j) {
            z_buffer[i][j] = std::numeric_limits<float>::max();
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
            z_buffer[i][j] = std::numeric_limits<float>::max();
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
}

void Screen::render_model(const Model& model) {

    for(const auto& face : model.get_faces()){
        //grab vertices, and transform them so that the camera is at 0,0,0, converted to homenzgous vector 4 for matrix math
        Vector4 vertex_0_4 = matrix_transform(camera.get_view_matrix(), to_vector4(model.get_vertices()[face.vertex_index[0]])); 
        Vector4 vertex_1_4 = matrix_transform(camera.get_view_matrix(), to_vector4(model.get_vertices()[face.vertex_index[1]])); 
        Vector4 vertex_2_4 = matrix_transform(camera.get_view_matrix(), to_vector4(model.get_vertices()[face.vertex_index[2]])); 

        // project the coned frustrum so it is in a cube shape, this will give the appearance of objects closer to camera being bigger
        //and objects farther away being smaller
        vertex_0_4 = matrix_transform(camera.get_projection_matrix(), vertex_0_4);
        vertex_1_4 = matrix_transform(camera.get_projection_matrix(), vertex_1_4);
        vertex_2_4 = matrix_transform(camera.get_projection_matrix(), vertex_2_4);

        // normalizes the cube to be a 1 by 1 by 1
        Vector3 vertex_0 = {vertex_0_4.x / vertex_0_4.w, vertex_0_4.y / vertex_0_4.w, vertex_0_4.z / vertex_0_4.w};
        Vector3 vertex_1 = {vertex_1_4.x / vertex_1_4.w, vertex_1_4.y / vertex_1_4.w, vertex_1_4.z / vertex_1_4.w};
        Vector3 vertex_2 = {vertex_2_4.x / vertex_2_4.w, vertex_2_4.y / vertex_2_4.w, vertex_2_4.z / vertex_2_4.w};

        //maps the 1 by 1 by cuber to the screen
        float z_0 = dot_product(camera.get_forward(), vertex_0); 
        float z_1 = dot_product(camera.get_forward(), vertex_1);
        float z_2 = dot_product(camera.get_forward(), vertex_2);

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
        const Vector3& face_normal = model.get_normals()[face.normal_index[0]];
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
                    float z = barycentric_interpolation_z_value(x, y, vertex_0, vertex_1, vertex_2);//determine z depth on all points as only the vertexes have a z value
                    if (z < z_buffer[x][y]) {//check the z_buffer if its on top render that pixel and store it
                        z_buffer[x][y] = z;
                        SDL_RenderDrawPointF(renderer, x, y);
                    }
                }
            }
         }
    }
}


void Screen::render_model_gourand(const Model& model){

    Matrix4 all_transforms = camera.get_projection_matrix() * camera.get_view_matrix();
    for(const auto& face : model.get_faces()){
        //grab vertices, and transform them so that the camera is at 0,0,0, converted to homenzgous vector 4 for matrix math
        Vector4 vertex_0_4 = matrix_transform(all_transforms, to_vector4(model.get_vertices()[face.vertex_index[0] ])); 
        Vector4 vertex_1_4 = matrix_transform(all_transforms, to_vector4(model.get_vertices()[face.vertex_index[1] ])); 
        Vector4 vertex_2_4 = matrix_transform(all_transforms, to_vector4(model.get_vertices()[face.vertex_index[2] ])); 

        // normalizes the cube to be a 1 by 1 by 1
        Vector3 vertex_0 = {vertex_0_4.x / vertex_0_4.w, vertex_0_4.y / vertex_0_4.w, vertex_0_4.z / vertex_0_4.w};
        Vector3 vertex_1 = {vertex_1_4.x / vertex_1_4.w, vertex_1_4.y / vertex_1_4.w, vertex_1_4.z / vertex_1_4.w};
        Vector3 vertex_2 = {vertex_2_4.x / vertex_2_4.w, vertex_2_4.y / vertex_2_4.w, vertex_2_4.z / vertex_2_4.w};

        //maps the 1 by 1 by cuber to the screen
        float z_0 = dot_product(camera.get_forward(), vertex_0); 
        float z_1 = dot_product(camera.get_forward(), vertex_1);
        float z_2 = dot_product(camera.get_forward(), vertex_2);

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

        Vector3 normal_0 = Vector3();
        for (int face_index : model.get_vertex_info()[face.vertex_index[0] ]) {
            normal_0 = normal_0 + model.get_normals()[face_index];
        }
        normal_0 = normal_0 / model.get_vertex_info()[face.vertex_index[0] ].size();
        //normal_0 = normalize(normal_0);

        Vector3 normal_1 = Vector3();
        for (int face_index : model.get_vertex_info()[face.vertex_index[1] ]) {
            normal_1 = normal_1 + model.get_normals()[face_index];
        }
        normal_1 = normal_1 / model.get_vertex_info()[face.vertex_index[1] ].size();
        //normal_1 = normalize(normal_1);

        Vector3 normal_2 = Vector3();
        for (int face_index : model.get_vertex_info()[face.vertex_index[2] ]) {
            normal_2 = normal_2 + model.get_normals()[face_index];
        }
        normal_2 = normal_2 / model.get_vertex_info()[face.vertex_index[2] ].size();
        //normal_2 = normalize(normal_2);



        float brightness_0 = std::min(1.0f,std::max(0.0f,dot_product(light_direction, normal_0)));
        float brightness_1 = std::min(1.0f,std::max(0.0f,dot_product(light_direction, normal_1)));
        float brightness_2 = std::min(1.0f,std::max(0.0f,dot_product(light_direction, normal_2)));

        // Calculate color at each vertex based on brightness and diffuse color
        Color color_0 = face.face_material.diffuse_color * brightness_0;
        Color color_1 = face.face_material.diffuse_color * brightness_1;
        Color color_2 = face.face_material.diffuse_color * brightness_2;

        //if minimizes the check area to be on the screen and no bigger than the triangle this is for efficenacy
        int min_x = std::max(0, std::min({static_cast<int>(vertex_0.x), static_cast<int>(vertex_1.x), static_cast<int>(vertex_2.x)}));
        int min_y = std::max(0, std::min({static_cast<int>(vertex_0.y), static_cast<int>(vertex_1.y), static_cast<int>(vertex_2.y)}));
        int max_x = std::min(SCREEN_WIDTH - 1, std::max({static_cast<int>(vertex_0.x), static_cast<int>(vertex_1.x), static_cast<int>(vertex_2.x)}));
        int max_y = std::min(SCREEN_HEIGHT - 1, std::max({static_cast<int>(vertex_0.y), static_cast<int>(vertex_1.y), static_cast<int>(vertex_2.y)}));

         for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                if (is_point_inside_triangle(x, y, vertex_0, vertex_1, vertex_2)) {//check if the pixel from the area to render is in the triangle
                    float z = barycentric_interpolation_z_value(x, y, vertex_0, vertex_1, vertex_2);
                    if (z < z_buffer[x][y]) {//check the z_buffer if its on top render that pixel and store it
                        z_buffer[x][y] = z;

                        Vector3 weights = barycentric_interpolation_weights(x, y, vertex_0, vertex_1, vertex_2);
                        Color interpolated_color = color_0 * weights.z + color_1 * weights.x + color_2 * weights.y;

                        SDL_SetRenderDrawColor(renderer, interpolated_color.r * 255, interpolated_color.g * 255, interpolated_color.b * 255, interpolated_color.a*255);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }
         }
    }
}