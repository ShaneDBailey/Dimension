#include "Screen.h"
#include "Model.h"

int main(){

    Screen screen;
    std::string object_path = "./assets/test.obj";
    std::string material_path = "./assets/test.mtl";
    Model model(object_path,material_path);
    //Vector3 center = model.getCenterOfOrigin();
    //model.translate(-center.x,-center.y,-center.z);
    std::cout << model.getCenterOfOrigin() << std::endl;
    screen.camera.printFrustumWorldBounds();
    screen.camera.lookAt(Vector3(0,0,20));
    model.translate(0,0,5);

    while(true){
        screen.clear_display();
        model.rotate(0.002,0.001,0.003);
        screen.render_model(model);
        SDL_RenderPresent(screen.renderer);
        screen.input();
        //SDL_Delay(30);
    }

    return 0;
}