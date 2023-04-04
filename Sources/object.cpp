#include "../Headers/object.hpp"
#include "../Headers/scene.hpp"
#include "../Headers/CImg.h"
#include <algorithm>

/// Object class
Vector3 object::getPosition() const
{
    return m_position;
}

Vector3 object::getColor() const
{
    return m_color;
}

Vector3 object::getRotation() const
{
    return m_rotation;
}

void object::setColor(const double &r, const double &g, const double &b)
{
    m_color = {r, g, b};
}

void object::setPosition(const double &x, const double &y, const double &z)
{
    m_position = {x, y, z};
}

void object::setRotation(const double &x, const double &y, const double &z)
{
    m_rotation = {x, y, z};
}
double object::getReflection() const
{
    return m_reflection;
}
void object::setReflection(double const value)
{
    m_reflection = value;
}

/// Camera
int camera::getWidth() const
{
    return m_width;
}
void camera::setWidth(int width)
{
    m_width = width;
}

int camera::getHeight() const
{
    return m_height;
}
void camera::setHeight(int height)
{
    m_height = height;
}

double camera::getResolution() const
{
    return m_resolution;
}
void camera::setResolution(double resolution)
{
    m_resolution = resolution;
}

double camera::getScreenDistance() const
{
    return m_screen_distance;
}
void camera::setScreenDistance(double screen_distance)
{
    m_screen_distance = screen_distance;
}

const Vector3 &camera::getCameraDirection() const
{
    return m_camera_direction;
}
void camera::setCameraDirection(const Vector3 &camera_direction)
{
    m_camera_direction = camera_direction;
}
const Vector3 &camera::getCameraOrtho() const
{
    return m_camera_orth;
}
void camera::setCameraOrtho(const Vector3 &camera_direction)
{
    m_camera_orth = camera_direction;
}
int camera::getReflectionLevel() const
{
    return m_reflection_level;
}
void camera::setReflectionLevel(const int reflection_level)
{
    m_reflection_level = reflection_level;
}

void camera::draw(scene scene, int reflection_level)
{

    /// Affichage des objets dans la console
    for (unsigned int i_object = 0; i_object < scene.getObjects().size(); i_object++)
    {
        Vector3 v = scene.getObjects()[i_object]->getPosition();
        std::cout << "Objet " << i_object << " x " << v.x << " y " << v.y << " z " << v.z << std::endl;
    }
    for (unsigned int i_light = 0; i_light < scene.getLights().size(); i_light++)
    {
        Vector3 v = scene.getLights()[i_light].getPosition();
        std::cout << "Light " << i_light << " x " << v.x << " y " << v.y << " z " << v.z << std::endl;
    }
    // Création de l'image
    cimg_library::CImg<double> img(m_width, m_height, 1, 3);
    img.fill(0);

    m_camera_direction.normalize();
    m_camera_orth.normalize();
    Vector3 camera_normal = m_camera_orth.cross(m_camera_direction);

    Vector3 centerOfScreen = this->getPosition() + m_screen_distance * m_camera_direction;
    double coef = 1.;
    int level;
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            Vector3 color(0, 0, 0);

            Vector3 M = centerOfScreen + (-m_width / 2 + i) * m_resolution * m_camera_orth + (m_height / 2 - j) * m_resolution * camera_normal;
            level = 0;
            coef = 1.;
            // Ray
            Ray cameraRay;
            cameraRay.direction = (M - this->getPosition());
            cameraRay.direction.normalize();
            cameraRay.origin = M;
            // Hit objects
            Vector3 intersectionPoint;
            Vector3 objectHitPoint;
            int objectID = -1;
            do
            {
                // Object hit id
                objectID = -1;
                double distanceHit = INFINITY;

                for (unsigned int i_object = 0; i_object < scene.getObjects().size(); i_object++)
                {

                    if (scene.getObjects()[i_object]->intersect(cameraRay, &intersectionPoint))
                    {
                        if (((intersectionPoint - cameraRay.origin).norm() < distanceHit))
                        {
                            objectID = i_object;
                            distanceHit = (intersectionPoint - cameraRay.origin).norm();
                            objectHitPoint = intersectionPoint;
                        }
                    }
                }
                if (objectID != -1)
                {
                    for (unsigned int i_light = 0; i_light < scene.getLights().size(); i_light++)
                    {
                        bool isDirect = true;
                        Ray lightRay;
                        lightRay.origin = objectHitPoint;
                        lightRay.direction = scene.getLights()[i_light].getPosition() - objectHitPoint;
                        Vector3 intersectionLightPoint;

                        for (unsigned int i_object = 0; i_object < scene.getObjects().size(); i_object++)
                        {

                            if ((int)i_object != objectID)
                            {
                                if (scene.getObjects()[i_object]->intersect(lightRay, &intersectionLightPoint))
                                {
                                    if (((intersectionLightPoint - scene.getLights()[i_light].getPosition()).norm() < lightRay.direction.norm()) & ((intersectionLightPoint - scene.getLights()[i_light].getPosition()).dot(lightRay.direction) < 0))
                                    {
                                        isDirect = false;
                                        break;
                                    }
                                }
                            }
                        }

                        if (isDirect)
                        {

                            Vector3 surfaceNormal = scene.getObjects()[objectID]->getSurfaceNormal(objectHitPoint);

                            if (surfaceNormal.dot(lightRay.direction) > 0)
                            {
                                //   std::cout<<"Biiiiim"<<std::endl;
                                lightRay.direction.normalize();
                                surfaceNormal.normalize();

                                color = color + coef * scene.getLights()[i_light].getColor() * scene.getObjects()[objectID]->getColor() * scene.getLights()[i_light].getIntensity() * surfaceNormal.dot(lightRay.direction);
                            }
                        }
                    }

                    // on itére sur la prochaine reflexion
                    Vector3 surfaceNormal = scene.getObjects()[objectID]->getSurfaceNormal(objectHitPoint);
                    surfaceNormal.normalize();
                    cameraRay.direction.normalize();
                    coef *= scene.getObjects()[objectID]->getReflection();
                    double reflet = 2. * (cameraRay.direction.dot(surfaceNormal));
                    cameraRay.origin = objectHitPoint;
                    cameraRay.direction = cameraRay.direction - reflet * surfaceNormal;
                    cameraRay.direction.normalize();
                    level++;
                }

            } while ((coef > 0.0f) && (level < reflection_level) && objectID != -1);
            char color3[3];
            if (color.x > 255)
            {
                color3[0] = 255;
            }
            else
            {
                color3[0] = color.x;
            }

            if (color.y > 255)
            {
                color3[1] = 255;
            }
            else
            {
                color3[1] = color.y;
            }
            if (color.z > 255)
            {
                color3[2] = 255;
            }
            else
            {
                color3[2] = color.z;
            }

            img.draw_point(i, j, color3);
        }
    }

    img.save("Img.bmp");
}

/// Light
void light::setIntensity(const double &i)
{
    m_intensity = i;
}

double light::getIntensity() const
{
    return m_intensity;
}
/// Sphere class
double sphere::getRadius() const
{
    return m_radius;
}
void sphere::setRadius(const double &r)
{
    m_radius = r;
}

/// Plan Class

Vector3 plan::getNormal()
{
    return m_normal;
}

/// Cube class

double cube::getEdge() const
{
    return m_edge;
}
void cube::setEdge(const double &e)
{
    m_edge = e;
}

void cube::createFaces(const double edge)
{
    m_planes.clear();
    Vector3 up(0, 0, 1);
    Vector3 down(0, 0, -1);
    Vector3 left(-1, 0, 0);
    Vector3 right(1, 0, 0);
    Vector3 forward(0, 1, 0);
    Vector3 backward(0, -1, 0);

    m_planes.push_back(plan(up, this->getPosition() + up * edge * 0.5, this->getColor()));
    m_planes.push_back(plan(down, this->getPosition() + down * edge * 0.5, this->getColor()));
    m_planes.push_back(plan(left, this->getPosition() + left * edge * 0.5, this->getColor()));
    m_planes.push_back(plan(right, this->getPosition() + right * edge * 0.5, this->getColor()));
    m_planes.push_back(plan(forward, this->getPosition() + forward * edge * 0.5, this->getColor()));
    m_planes.push_back(plan(backward, this->getPosition() + backward * edge * 0.5, this->getColor()));
}

bool cube::isInsideSquare(Vector3 point, plan plane, double edge)
{

    Vector3 center = plane.getPosition();
    Vector3 normal = plane.getNormal();

    // Generator vectors of face
    Vector3 up(0, 0, 1);
    Vector3 orth = normal.cross(up);

    Vector3 topLeftCorner = center + up * edge * 0.5 + orth * edge * 0.5;
    Vector3 topRightCorner = center + up * edge * 0.5 - orth * edge * 0.5;
    Vector3 botLeftCorner = center - up * edge * 0.5 + orth * edge * 0.5;
    Vector3 botRightCorner = center - up * edge * 0.5 - orth * edge * 0.5;

    Vector3 centerPoint = point - center;

    double a = (topLeftCorner - point).dot(centerPoint);
    double b = (topRightCorner - point).dot(centerPoint);
    double c = (botLeftCorner - point).dot(centerPoint);
    double d = (botRightCorner - point).dot(centerPoint);

    if ((a < 0) & (b < 0) & (c < 0) & (d < 0))
    {
        return false;
    }
    return true;
}
