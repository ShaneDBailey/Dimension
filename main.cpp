#include "Screen.h"
#include "Model.h"
#include "Loader.h"
int main(){

    Screen screen;
    std::string object_path = "./assets/test.obj";
    std::string material_path = "./assets/test.mtl";
    Model model =  Loader::load_obj(object_path,material_path);
    model.translate(0,0,0);
    model.find_origin();

    Camera camera = Camera(Vector3(-20, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, -1), 45.0f, 1.0f , 0.5f, 200.0f);
    screen.camera = camera;
    Vector3 light_direction{-1, 0, 0};
    light_direction = normalize(light_direction);
    screen.light_direction = light_direction;

    
    screen.camera.set_forward(model.get_center_of_origin()-screen.camera.get_position());
    screen.camera.update_views();
    screen.camera.print_frustum_world_bounds();

    while(true){
        screen.clear_display();
        model.rotate(0.01,0.02,0.03);

        screen.render_model_gourand(model);
        SDL_RenderPresent(screen.renderer);
        screen.input();
        SDL_Delay(30);
    }

    return 0;
}