#ifndef geometryHPP
#define geometryHPP

#include <cmath>

class Vector3 {
public:
    double x, y, z;
    
    Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}
    
    // Calculate the norm (length) of the vector
    double norm() const {
        return sqrt(x*x + y*y + z*z);
    }
    
    void normalize()  {
        double norm = this->norm();
        if (norm != 0){
            x = x/norm;
            y = y/norm;
            z = z/norm;
        }
    }

    // Calculate the dot product of this vector with another vector
    double dot(const Vector3& other) const {
        return x*other.x + y*other.y + z*other.z;
    }
    
    // Calculate the cross product of this vector with another vector
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y*other.z - z*other.y,
            z*other.x - x*other.z,
            x*other.y - y*other.x
        );
    }
    
    // Overload the addition operator to add two vectors together
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x+other.x, y+other.y, z+other.z);
    }
    
    // Overload the subtraction operator to subtract one vector from another
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x-other.x, y-other.y, z-other.z);
    }
    
    // Overload the multiplication operator to multiply the vector by a scalar
    Vector3 operator*(double scalar) const {
        return Vector3(x*scalar, y*scalar, z*scalar);
    }

    Vector3 operator*(Vector3 vector) const {
        return Vector3(x*vector.x, y*vector.y, z*vector.z);
    }
};

inline Vector3 operator*(double scalar, const Vector3& vec) {
    return vec * scalar;
}

struct Ray {
    Vector3 origin;
    Vector3 direction;
};

#endif