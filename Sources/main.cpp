#include "../Headers/object.hpp"
#include "../Headers/scene.hpp"
#include <cstdio>

#include "../Headers/CImg.h"

using namespace std;

int main_2()
{

    // Scene setup

    Vector3 pos = {0., -1., 0.};
    Vector3 camera_dir = {0., 1., 0.};
    Vector3 camera_orth = {1., 0., 0.};
    int width = 960;
    int height = 720;
    double resolution = 0.0035;
    double screen_distance = 1.4;
    camera cam(pos, camera_dir, camera_orth, width, height, resolution, screen_distance);

    scene scene1;

    sphere sphere1(0., 3., -1., 1.25);
    sphere1.setColor(255., 0., 0.);
    sphere1.setReflection(0.5);

    sphere sphere2(-1., 3., 1., 1.25);
    sphere2.setColor(0., 255., 0.);
    sphere2.setReflection(0.5);

    sphere sphere3(1., 3., 1., 1.25);
    sphere3.setColor(0., 0., 255.);
    sphere3.setReflection(0.5);
    scene1.addObject(&sphere3);
    scene1.addObject(&sphere1);
    scene1.addObject(&sphere2);

    std::vector<double> cartesien1 = {0., -1, 0, 3.};

    Vector3 Color(255., 255., 255.);
    plan plan1(cartesien1, Color);
    plan1.setColor(255., 255., 255.);
    plan1.setReflection(0.5);
    // scene1.addObject(&plan1);

    light pointLight({3., 0., 3.}, {1., 1., 1.});
    scene1.addLight(pointLight);
    light pointLight2({-3., 0., 3.}, {1., 1., 1.});
    scene1.addLight(pointLight2);

    cam.draw(scene1);

    return 0;
}
