#ifndef objetHPP
#define objetHPP

#include "../Headers/geometry.hpp"
#include <vector>
#include <iostream>

class scene;

class object
{
private:
    Vector3 m_position;
    Vector3 m_rotation;
    Vector3 m_color; // créer une classe Material
    double m_reflection;

public:
    object() {}
    object(const double &x, const double &y, const double &z) : m_position({x, y, z}) {}
    object(Vector3 pos, Vector3 col) : m_position(pos), m_color(col) {}
    explicit object(Vector3 pos) : m_position(pos) {}

    Vector3 getPosition() const;
    Vector3 getRotation() const;
    Vector3 getColor() const;

    void setColor(const double &r, const double &g, const double &b);
    void setPosition(const double &x, const double &y, const double &z);
    void setRotation(const double &x, const double &y, const double &z);
    double getReflection() const;
    void setReflection(double const value);

    virtual bool intersect(Ray ray, Vector3 *intersectionPoint)
    {
        std::cout << "Ihhhhhhh" << std::endl;
        return false;
    };
    virtual Vector3 getSurfaceNormal(Vector3 surfacePoint) { return Vector3(0, 0, 0); };
};

class camera : public object
{

private:
    int m_width;                // en pixel largeur de l'écran
    int m_height;               // en pixel hauteur de l'écran
    double m_resolution;        // largeur d'un pixel (carré)
    double m_screen_distance;   // distance à l'écran
    Vector3 m_camera_direction; // vecteur qui dit ou pointe la caméra
    Vector3 m_camera_orth;      // vecteur orthogonal à la direction pointée (donne info sur l inclinaison de la camera)

public:
    camera() : object() {}
    camera(Vector3 origin, Vector3 dir, Vector3 orth, const int &width, const int &height, const double &res, const double &dist) : object(origin), m_camera_direction(dir), m_camera_orth(orth),
                                                                                                                                    m_width(width), m_height(height), m_resolution(res), m_screen_distance(dist) {}

    int getWidth() const;
    void setWidth(int width);
    int getHeight() const;
    void setHeight(int height);
    double getResolution() const;
    void setResolution(double resolution);
    double getScreenDistance() const;
    void setScreenDistance(double screen_distance);
    const Vector3 &getCameraDirection() const;
    void setCameraDirection(const Vector3 &camera_direction);
    const Vector3 &getCameraOrtho() const;
    void setCameraOrtho(const Vector3 &camera_direction);

    void draw(scene scene);
};

class light : public object
{

private:
    double m_intensity = 1;

public:
    light(Vector3 pos, Vector3 col) : object(pos, col) {}

    void setIntensity(const double &i);
    double getIntensity() const;
};

class sphere : public object
{
private:
    double m_radius;

public:
    sphere() : object() {}
    sphere(const double &x, const double &y, const double &z) : object(x, y, z) {}
    sphere(const double &x, const double &y, const double &z, const double &r) : m_radius(r), object(x, y, z) {}
    sphere(Vector3 pos, double r) : object(pos), m_radius(r) {}
    sphere(Vector3 pos, Vector3 col, double r) : object(pos, col), m_radius(r) {}

    double getRadius() const;
    void setRadius(const double &r);

    bool intersect(Ray ray, Vector3 *intersectionPoint) override
    {

        Vector3 oc = ray.origin - this->getPosition();
        double a = ray.direction.norm() * ray.direction.norm();
        double b = 2. * oc.dot(ray.direction);
        double c = oc.norm() * oc.norm() - this->getRadius() * this->getRadius();
        double discriminant = b * b - 4. * a * c;
        if (discriminant < 0)
        {
            return false;
        }
        double t = (-b - sqrt(discriminant)) / (2. * a);
        if (t < 0)
        {
            return false;
        }

        *intersectionPoint = ray.origin + t * ray.direction;
        return true;
    }

    Vector3 getSurfaceNormal(Vector3 surfacePoint) override
    {
        Vector3 n = surfacePoint - this->getPosition();
        return n;
    }
};

class plan : public object
{

private:
    std::vector<double> m_cartesien;

public:
    plan();

    plan(const double &x, const double &y, const double &z) : object(x, y, z) {}
    plan(std::vector<double> cartesien, Vector3 Color) : object(Vector3(0., 0., 0.), Color), m_cartesien(cartesien) {}
    virtual bool intersect(Ray ray, Vector3 *intersectionPoint) override
    {

        double t;

        Vector3 Vdir = ray.direction;
        Vector3 normal(m_cartesien[0], m_cartesien[1], m_cartesien[2]);
        if (normal.dot(Vdir) == 0)
        {
            return false;
        }
        else
        {
            // supposons que la droite n'est ni confondu avec le plan ni parrallèle
            // le confondu avec le plan est encore à implémenter
            t = -(normal.dot(ray.origin) + m_cartesien[3]) / (normal.dot(ray.direction)); // à revoir

            if (t < 0)
            {
                return false;
            }

            *intersectionPoint = ray.origin + t * ray.direction; // à revoir
            //*I=(d.GetPoint(t));
            /*if(t<0){
                return false;
            }*/
            return true;
        }
    }
    Vector3 getSurfaceNormal(Vector3 const PointSurface) override
    {
        Vector3 normal(m_cartesien[0], m_cartesien[1], m_cartesien[2]);
        return normal;
    }
};

#endif