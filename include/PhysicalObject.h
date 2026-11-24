#ifndef PHYSICAL_OBJECT_H
#define PHYSICAL_OBJECT_H

#include <iostream>
#include "Position.h"
#include "Distance.h"
#include "Velocity.h"
#include "Acceleration.h"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Position_generator.h"
#include <memory>

//PhysicalObject contains a Position_generator, while Source and Microphone inherit from PhysicalObject. The different trajectory generators inherit from Position_generator.

//forward declaration of the Velocity and Acceleration structs
struct Velocity;
struct Acceleration;

class PhysicalObject {
    protected:
        std::unique_ptr<Position_generator> pos_gen;

    public:
        void setVelocity(Velocity new_velocity);
        void setAcceleration (Acceleration new_acceleration);
        Position getPosition(float time) const;
        Velocity getVelocity(float time) const;
        Acceleration getAcceleration(float time) const;
        PhysicalObject(std::unique_ptr<Position_generator> start_pos);

    private:
        Velocity velocity;
        Acceleration acceleration;

};

#endif // PHYSICAL_OBJECT_H
