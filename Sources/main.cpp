#include <iostream>
#include "../Headers/pugixml.hpp"
#include "../Headers/object.hpp"
#include "../Headers/scene.hpp"
#include <cstdio>

#include "../Headers/CImg.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Vérifier que le fichier XML est fourni en argument
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " scene.xml" << std::endl;
        return 1;
    }

    // Charger le fichier XML
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(argv[1]);
    if (!result)
    {
        std::cerr << "Erreur de chargement du fichier XML : " << result.description() << std::endl;
        return 1;
    }

    // Créer la scène
    scene myScene;

    // Récupérer les paramètres de la caméra
    pugi::xml_node cameraNode = doc.child("scene").child("camera");
    Vector3 pos(cameraNode.attribute("pos_x").as_double(),
                cameraNode.attribute("pos_y").as_double(),
                cameraNode.attribute("pos_z").as_double());
    Vector3 dir(cameraNode.attribute("dir_x").as_double(),
                cameraNode.attribute("dir_y").as_double(),
                cameraNode.attribute("dir_z").as_double());
    Vector3 orth(cameraNode.attribute("orth_x").as_double(),
                 cameraNode.attribute("orth_y").as_double(),
                 cameraNode.attribute("orth_z").as_double());
    int width = cameraNode.attribute("width").as_int();
    int height = cameraNode.attribute("height").as_int();
    double resolution = cameraNode.attribute("resolution").as_double();
    double screen_distance = cameraNode.attribute("screen_distance").as_double();
    camera cam(pos, dir, orth, width, height, resolution, screen_distance);

    // Récupérer les objets de la scène
    pugi::xml_node objectsNode = doc.child("scene").child("objects");
    for (pugi::xml_node objectNode : objectsNode.children())
    {
        std::string objectType = objectNode.name();
        if (objectType == "sphere")
        {
            Vector3 center(objectNode.attribute("pos_x").as_double(),
                           objectNode.attribute("pos_y").as_double(),
                           objectNode.attribute("pos_z").as_double());
            double radius = objectNode.attribute("radius").as_double();
            // std::cout<<"Objet "<<" x "<<center.x<<" y "<<center.y<<" z "<<center.z<<std::endl;

            double reflection = objectNode.attribute("reflection").as_double();
            // sphere newSphere(center, radius);
            sphere *newSphere = new sphere(center, radius);
            newSphere->setColor(objectNode.attribute("color_r").as_double(),
                                objectNode.attribute("color_g").as_double(),
                                objectNode.attribute("color_b").as_double());
            newSphere->setReflection(reflection);
            myScene.addObject(newSphere);
        }
        else if (objectType == "plan")
        {
            Vector3 normal = {
                objectNode.attribute("n_x").as_double(),
                objectNode.attribute("n_y").as_double(),
                objectNode.attribute("n_z").as_double()};
            Vector3 position = {
                objectNode.attribute("pos_x").as_double(),
                objectNode.attribute("pos_y").as_double(),
                objectNode.attribute("pos_z").as_double()};

            Vector3 color(objectNode.attribute("color_r").as_double(),
                          objectNode.attribute("color_g").as_double(),
                          objectNode.attribute("color_b").as_double());
            double reflection = objectNode.attribute("reflection").as_double();
            normal.normalize();
            plan *p = new plan(normal, position, color);
            p->setReflection(reflection);
            myScene.addObject(p);
        }
        else
        {
            std::cerr << "Unknown object type: " << objectType << std::endl;
        }
    }

    // Loop over lights
    for (pugi::xml_node lightNode : doc.child("scene").child("lights"))
    {
        Vector3 position(lightNode.attribute("pos_x").as_double(),
                         lightNode.attribute("pos_y").as_double(),
                         lightNode.attribute("pos_z").as_double());
        Vector3 color(lightNode.attribute("color_r").as_double(),
                      lightNode.attribute("color_g").as_double(),
                      lightNode.attribute("color_b").as_double());
        // light *newLight = new light(position, color);
        myScene.addLight(light(position, color));
    }
    std::cout << "Scene created : " << std::endl;
    cam.draw(myScene);

    return 0;
}