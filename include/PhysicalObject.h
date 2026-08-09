#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

#include <iostream>
#include "Position.h"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

//forward declaration of the Velopcity and Acceleration structs
struct Velocity;
struct Acceleration;

class PhysicalObject {
    protected:
    std::vector<Position> positions;
    std::vector<Velocity> Velocity_history;
    std::vector<Acceleration> Acceleration_history;

    public:
    Position getPositionSample(int n) const;
    void updatePosition(int n, float delta_t);
    void updateVelocity(int n, float delta_t);
    void setVelocity(Velocity v);
    Velocity getVelocitySample(int n) const;
    void setAcceleration(Acceleration a);
    Acceleration getAccelerationSample(int n) const;
};

#endif // PHYSICAL_OBJECT_H