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
    model.translate(0,0,5);
    model.find_origin();
    //screen.camera.lookAt(model.getCenterOfOrigin());
    screen.camera.setForward(model.getCenterOfOrigin()-screen.camera.getPosition());
    screen.camera.setViewMatrix();
    screen.camera.setOrthographicProjectionMatrix(-10, 10, -10, 10, 0.1f, 1);
    //screen.camera.setProjectionMatrix();
    screen.camera.printFrustumWorldBounds();

    while(true){
        screen.clear_display();
        model.rotate(0,0,0.003);
        screen.render_model(model);
        SDL_RenderPresent(screen.renderer);
        screen.input();
        //SDL_Delay(30);
    }

    return 0;
}