#ifndef sceneHPP
#define sceneHPP

#include "../Headers/object.hpp"

class scene
{

private:
    std::vector<object *> m_objects = {};
    std::vector<light *> m_lights = {};
    // std::vector<camera> m_cameras = {};

public:
    scene() {}
    void addObject(object *const &obj);
    void addLight(light *const &light);

    std::vector<object *> getObjects() const;
    std::vector<light *> getLights() const;
};

#endif