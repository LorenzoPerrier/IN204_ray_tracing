#include "../Headers/scene.hpp"

void scene::addObject(object *const &obj)
{
    m_objects.push_back(obj);
}

void scene::addLight(light *const &light)
{
    m_lights.push_back(light);
}

std::vector<light *> scene::getLights() const
{
    return m_lights;
}
std::vector<object *> scene::getObjects() const
{
    return m_objects;
}