#include "Screen.h"
#include "Model.h"
#include "Loader.h"
int main(){

    Screen screen;
    std::string object_path = "./assets/test.obj";
    std::string material_path = "./assets/test.mtl";
    Model model =  Loader::load_obj(object_path,material_path);
    model.find_origin();

    //screen.camera.lookAt(model.getCenterOfOrigin());
    screen.camera.setForward(model.get_center_of_origin()-screen.camera.getPosition());
    screen.camera.updateViews();
    //screen.camera.setProjectionMatrix();
    
    screen.camera.printFrustumWorldBounds();
    Vector3 teast = Vector3(1,5,10);

    while(true){
        screen.clear_display();
        model.rotate_around_point(0.01,0.02,0.03,teast);
        screen.render_model(model);
        SDL_RenderPresent(screen.renderer);
        screen.input();
        SDL_Delay(30);
    }

    return 0;
}