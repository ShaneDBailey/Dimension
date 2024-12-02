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
            zBuffer[i][j] = std::numeric_limits<float>::min();
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





void Screen::render_model(const Model& model) {
    // Step 1: Get the model's vertices
    const std::vector<Vector3>& vertices = model.getVertices();
    
    // Iterate through all vertices in the model
    for (const Vector3& vertex : vertices) {
        
        // Step 2: Transform the vertex using the camera's view matrix
        Vector3 transformedVertex = vertex;
        
        // Apply the view matrix (transform to camera space)
        transformedVertex = matrix_transform(camera.getViewMatrix(), transformedVertex);
        
        // Step 3: Apply the projection matrix (transform to normalized device coordinates)
        Vector3 projectedVertex = matrix_transform(camera.getProjectionMatrix(), transformedVertex);

        // Step 4: Perform the perspective divide to map to NDC (Normalized Device Coordinates)
        // Perspective divide: Divide by w (we assume w is stored in `projectedVertex.z` after projection)
        if (projectedVertex.z != 0.0f) {
            projectedVertex.x /= projectedVertex.z;  // Normalize x
            projectedVertex.y /= projectedVertex.z;  // Normalize y
            // The z value can be used for depth testing, but we won't need it for this basic rendering
        }

        // Step 5: Map the 3D NDC to 2D screen coordinates
        int screenX = static_cast<int>((transformedVertex.x * 0.5f + 0.5f) * SCREEN_WIDTH);  // X mapped to screen width
        int screenY = static_cast<int>((-transformedVertex.y * 0.5f + 0.5f) * SCREEN_HEIGHT); // Y mapped to screen height

        // Step 6: Draw the point on the screen
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set color to red
        SDL_RenderDrawPoint(renderer, screenX, screenY);   // Draw the point
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
