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
    Vector3 m_rotation = Vector3(0., 0., 0.); // Vector indicating rotation allong x , y , z vectors // used for cube and plane only
    Vector3 m_color;                          // créer une classe Material
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
    void setRotation(const Vector3 rot);
    double getReflection() const;
    void setReflection(double const value);

    virtual bool intersect(Ray ray, Vector3 *intersectionPoint)
    {
        std::cout << "Ihhhhhhh" << std::endl;
        return false;
    };
    virtual Vector3 getSurfaceNormal(Vector3 surfacePoint) { return Vector3(0, 0, 0); };
    virtual void createObject() { return; };
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
    int m_reflection_level;     // number of reflection levels
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
    int getReflectionLevel() const;
    void setReflectionLevel(const int reflection_level);

    void draw(scene scene, int reflection_level, const char *filename);
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

        Vector3 oc = this->getPosition() - ray.origin;
        double t = oc.dot(ray.direction);
        Vector3 p = ray.origin + t * ray.direction;

        if ((p - this->getPosition()).norm() <= m_radius)
        {

            double length = (this->getPosition() - p).norm();
            double x = sqrt(pow(m_radius, 2) - pow(length, 2));
            double t1 = t - x;
            *intersectionPoint = ray.origin + t1 * ray.direction;
            return true;
        }
        else
        {
            return false;
        }
        /// Hard shadows :

        // Vector3 oc = ray.origin - this->getPosition();
        // double a = ray.direction.norm() * ray.direction.norm();
        // double b = 2. * oc.dot(ray.direction);
        // double c = oc.norm() * oc.norm() - this->getRadius() * this->getRadius();
        // double discriminant = b * b - 4. * a * c;
        // if (discriminant < 0)
        // {
        //     return false;
        // }
        // double t = (-b - sqrt(discriminant)) / (2. * a);
        // if (t < 0)
        // {
        //     return false;
        // }

        // *intersectionPoint = ray.origin + t * ray.direction;
        // return true;
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
    Vector3 m_normal;

public:
    void createObject() override
    {

        // Rotation matrix
        Matrix R_x;
        Matrix R_y;
        Matrix R_z;

        double x_angle = this->getRotation().x;
        R_x.row1 = Vector3(1, 0, 0);
        R_x.row2 = Vector3(0, cos(x_angle), -sin(x_angle));
        R_x.row3 = Vector3(0, sin(x_angle), cos(x_angle));

        double y_angle = this->getRotation().y;
        R_y.row1 = Vector3(cos(y_angle), 0, sin(y_angle));
        R_y.row2 = Vector3(0, 1, 0);
        R_y.row3 = Vector3(-sin(y_angle), 0, cos(y_angle));

        double z_angle = this->getRotation().z;
        R_z.row1 = Vector3(cos(z_angle), -sin(z_angle), 0);
        R_z.row2 = Vector3(sin(z_angle), cos(z_angle), 0);
        R_z.row3 = Vector3(0, 0, 1);

        m_normal = R_x * (R_y * (R_z * m_normal));
        // m_normal.printVector();
    }

    plan() : object() {}
    plan(const double &x, const double &y, const double &z) : object(x, y, z) { createObject(); }
    plan(Vector3 normal, Vector3 position, Vector3 Color) : object(position, Color), m_normal(normal)
    {
        createObject();
    }
    Vector3 getNormal();

    virtual bool intersect(Ray ray, Vector3 *intersectionPoint) override
    {

        double t;
        Vector3 p0 = this->getPosition();
        Vector3 l = ray.direction;
        Vector3 l0 = ray.origin;
        l.normalize();
        double product = l.dot(m_normal);
        if (fabs(product) < pow(10, -6))
        {
            return false;
        }
        t = (p0 - l0).dot(m_normal) / product;
        auto result = ray.origin + t * ray.direction;
        *intersectionPoint = result;

        return true;
    }

    Vector3 getSurfaceNormal(Vector3 const PointSurface) override
    {
        return m_normal;
    }
};

class cube : public object
{
private:
    double m_edge; // accessible variable

    std::vector<plan> m_planes;

    bool isInsideSquare(Vector3 point, plan plane, double edge);

public:
    double getEdge() const;
    void setEdge(const double &r);

    void createObject() override
    {

        // Rotation matrix
        Matrix R_x;
        Matrix R_y;
        Matrix R_z;

        double x_angle = this->getRotation().x;
        R_x.row1 = Vector3(1, 0, 0);
        R_x.row2 = Vector3(0, cos(x_angle), -sin(x_angle));
        R_x.row3 = Vector3(0, sin(x_angle), cos(x_angle));

        double y_angle = this->getRotation().y;
        R_y.row1 = Vector3(cos(y_angle), 0, sin(y_angle));
        R_y.row2 = Vector3(0, 1, 0);
        R_y.row3 = Vector3(-sin(y_angle), 0, cos(y_angle));

        double z_angle = this->getRotation().z;
        R_z.row1 = Vector3(cos(z_angle), -sin(z_angle), 0);
        R_z.row2 = Vector3(sin(z_angle), cos(z_angle), 0);
        R_z.row3 = Vector3(0, 0, 1);

        m_planes.clear();
        Vector3 up(0, 0, 1);
        Vector3 down(0, 0, -1);
        Vector3 left(-1, 0, 0);
        Vector3 right(1, 0, 0);
        Vector3 forward(0, -1, 0);
        Vector3 backward(0, 1, 0);

        // Apply rotation

        up = R_x * (R_y * (R_z * up));
        down = R_x * (R_y * (R_z * down));
        left = R_x * (R_y * (R_z * left));
        right = R_x * (R_y * (R_z * right));
        forward = R_x * (R_y * (R_z * forward));
        backward = R_x * (R_y * (R_z * backward));

        up.normalize();
        down.normalize();
        left.normalize();
        right.normalize();
        forward.normalize();
        backward.normalize();

        // printf("up :\n");
        // up.printVector();
        // printf("down :\n");
        // down.printVector();
        // printf("left :\n");
        // left.printVector();
        // printf("right :\n");
        // right.printVector();
        // printf("forward :\n");
        // forward.printVector();
        // printf("backward :\n");
        // backward.printVector();

        m_planes.push_back(plan(up, this->getPosition() + up * m_edge * 0.5, this->getColor()));
        m_planes.push_back(plan(down, this->getPosition() + down * m_edge * 0.5, this->getColor()));
        m_planes.push_back(plan(left, this->getPosition() + left * m_edge * 0.5, this->getColor()));
        m_planes.push_back(plan(right, this->getPosition() + right * m_edge * 0.5, this->getColor()));
        m_planes.push_back(plan(forward, this->getPosition() + forward * m_edge * 0.5, this->getColor()));
        m_planes.push_back(plan(backward, this->getPosition() + backward * m_edge * 0.5, this->getColor()));
    }
    cube() : object() {}
    cube(const double &x, const double &y, const double &z) : object(x, y, z) {}
    cube(const double &x, const double &y, const double &z, const double &e) : m_edge(e), object(x, y, z) { createObject(); }
    cube(Vector3 pos, double e) : object(pos), m_edge(e) { createObject(); }
    cube(Vector3 pos, Vector3 col, double e) : object(pos, col), m_edge(e)
    {
        createObject();
    }

    bool intersect(Ray ray, Vector3 *intersectionPoint) override
    {
        bool test = false;
        for (int k = 0; k < 6; k++)
        {

            if (m_planes[k].getNormal().dot(ray.direction) < 0)
            {

                Vector3 point;
                if (m_planes[k].intersect(ray, &point))
                {

                    if (isInsideSquare(point, m_planes[k], m_edge))
                    {
                        test = true;
                        *intersectionPoint = point;
                        break;
                    }
                }
            }
        }

        return test;
    }
    Vector3 getSurfaceNormal(Vector3 surfacePoint) override
    {
        for (int k = 0; k < 6; k++)
        {

            if (isInsideSquare(surfacePoint, m_planes[k], m_edge))
            {
                return m_planes[k].getNormal();
            }
        }

        return m_planes[0].getNormal(); // shouldnt happen
    }
};

class triangle : public object
{

private:
    Vector3 m_sommet2;
    Vector3 m_sommet3;
    Vector3 m_edge_12;
    Vector3 m_edge_23;
    Vector3 m_edge_13;
    Vector3 m_normal;

public:
    triangle(Vector3 sommet1, Vector3 sommet2, Vector3 sommet3, Vector3 normal, Vector3 col) : object(sommet1, col), m_sommet2(sommet2), m_sommet3(sommet3), m_edge_12(sommet2 - sommet1), m_edge_23(sommet3 - sommet2), m_edge_13(sommet3 - sommet1), m_normal(normal) {}
    virtual bool intersect(Ray ray, Vector3 *intersectionPoint) override
    {
        const float EPSILON = 0.0000001f;
        Vector3 n1 = ray.direction.cross(m_edge_13);
        double det = m_edge_12.dot(n1);
        if (std::abs(det) < EPSILON)
        {
            return false;
        }
        Vector3 n2 = ray.origin - this->getPosition();
        double u = n2.dot(n1) / det;
        if (u < 0.0f || u > 1.0f)
        {
            return false;
        }
        Vector3 n3 = n2.cross(m_edge_12);
        double v = ray.direction.dot(n3) / det;
        if (v < 0.0f || u + v > 1.0f)
        {
            return false;
        }
        double t = m_edge_13.dot(n3) / det;
        if (t < 0)
        {
            return false;
        }
        *intersectionPoint = ray.origin + t * ray.direction;
        /*bool rayTriangleIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t)
        {   glm::vec3 v0v1 = v1 - v0;
            glm::vec3 v0v2 = v2 - v0;
            glm::vec3 pvec = glm::cross(rayDirection, v0v2);
            float det = glm::dot(v0v1, pvec);    // Ray and triangle are parallel if det is close to 0
            if (fabs(det) < EPSILON) {return false;}
            float invDet = 1.0f / det;
            glm::vec3 tvec = rayOrigin - v0;
            float u = glm::dot(tvec, pvec) * invDet;    // The intersection lies outside of the triangle
            if (u < 0.0f || u > 1.0f) {return false;    }
            glm::vec3 qvec = glm::cross(tvec, v0v1);
            float v = glm::dot(rayDirection, qvec) * invDet;    // The intersection lies outside of the triangle
            if (v < 0.0f || u + v > 1.0f) {return false;}
            t = glm::dot(v0v2, qvec) * invDet;   // The intersection lies behind the ray origin
            if (t < 0.0f) {return false;}
            return true;
        }*/

        return true;
    };
    Vector3 getSurfaceNormal(Vector3 const PointSurface) override
    {
        return m_normal;
    };
    Vector3 getSommet(int n) const
    {
        if (n == 1)
        {
            return this->getPosition();
        }
        else if (n == 2)
        {
            return m_sommet2;
        }
        else if (n == 3)
        {
            return m_sommet3;
        }
        else
        {
            return Vector3(0, 0, 0);
        }
    };
};
#endif