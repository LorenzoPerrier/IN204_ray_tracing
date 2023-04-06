#include <iostream>
#include "../Headers/pugixml.hpp"
#include "../Headers/object.hpp"
#include "../Headers/scene.hpp"
#include <cstdio>
#include <chrono>
#include <fstream>

#include "../Headers/CImg.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Vérifier que le fichier XML est fourni en argument
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " outputfilename.bmp"
                  << " scene.xml"
                  << " (optional)object.stl"
                  << std::endl;
        return 1;
    }

    // Charger le fichier XML
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(argv[2]);
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
    int reflection_level = cameraNode.attribute("reflection_level").as_int();
    camera cam(pos, dir, orth, width, height, resolution, screen_distance);
    cam.setReflectionLevel(reflection_level);

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

            double reflection = objectNode.attribute("reflection").as_double();
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
            Vector3 rotation(objectNode.attribute("rot_x").as_double(),
                             objectNode.attribute("rot_y").as_double(),
                             objectNode.attribute("rot_z").as_double());
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
            p->setRotation(rotation);
            myScene.addObject(p);
        }

        else if (objectType == "cube")
        {
            Vector3 position(objectNode.attribute("pos_x").as_double(),
                             objectNode.attribute("pos_y").as_double(),
                             objectNode.attribute("pos_z").as_double());
            Vector3 rotation(objectNode.attribute("rot_x").as_double(),
                             objectNode.attribute("rot_y").as_double(),
                             objectNode.attribute("rot_z").as_double());
            double edge = objectNode.attribute("edge").as_double();
            double reflection = objectNode.attribute("reflection").as_double();
            Vector3 color(objectNode.attribute("color_r").as_double(),
                          objectNode.attribute("color_g").as_double(),
                          objectNode.attribute("color_b").as_double());

            cube *newCube = new cube(position, color, edge);
            newCube->setRotation(rotation);
            newCube->setReflection(reflection);
            myScene.addObject(newCube);
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
        double intensity(lightNode.attribute("intensity").as_double());
        light *l = new light(position, color);
        l->setIntensity(intensity);
        myScene.addLight(l);
    }

    // Charger le fichier objet STL

    if (argc == 4)
    {
        std::ifstream file(argv[3], std::ios::in | std::ios::binary);
        if (!file)
        {
            std::cout << "Pas de fichier stl à ce nom" << std::endl;
        }
        else
        {

            // en-tête du fichier STL
            char header[80] = "";
            char ntriangles[4];
            file.read(header, 80);
            file.read(ntriangles, 4);
            int num_triangles = *(int *)ntriangles;

            // Lecture de chaque triangle
            for (int i = 0; i < num_triangles; i++)
            {

                float nx, ny, nz;
                file.read((char *)&nx, sizeof(float));
                file.read((char *)&ny, sizeof(float));
                file.read((char *)&nz, sizeof(float));

                float x1, y1, z1, x2, y2, z2, x3, y3, z3;
                file.read((char *)&x1, sizeof(float));
                file.read((char *)&y1, sizeof(float));
                file.read((char *)&z1, sizeof(float));
                file.read((char *)&x2, sizeof(float));
                file.read((char *)&y2, sizeof(float));
                file.read((char *)&z2, sizeof(float));
                file.read((char *)&x3, sizeof(float));
                file.read((char *)&y3, sizeof(float));
                file.read((char *)&z3, sizeof(float));

                triangle *newTriangle = new triangle(
                    Vector3(x1, y1, z1),
                    Vector3(x2, y2, z2),
                    Vector3(x3, y3, z3),
                    Vector3(nx, ny, nz),
                    Vector3(0, 200, 0));

                myScene.addObject(newTriangle);

                // attribut du triangle (inutile)
                char attribute[2];
                file.read(attribute, 2);
            }
        }
    }

    std::cout << "Scene created : " << std::endl;

    // Draw Scene
    const char *filename = argv[1];

    auto start = chrono::high_resolution_clock::now();
    cam.draw(myScene, reflection_level, filename);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    std::cout << "Time taken for drawing the scene : " << duration.count() / 1000. << "sec" << std::endl;
    return 0;
}