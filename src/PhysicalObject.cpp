#include "PhysicalObject.h"
#include <utility>

//constructor, gives ownership of the Position_generator object to the PhysicalObject.

    PhysicalObject::PhysicalObject(std::unique_ptr<Position_generator> start_pos) : pos_gen(std::move(start_pos)) {}

    //setter that updates the velocity value
    void PhysicalObject::setVelocity(Velocity new_velocity){
        this->velocity = new_velocity;
    }

    void PhysicalObject::setAcceleration(Acceleration new_acceleration){
        this->acceleration = new_acceleration;
    }

    //getter for the position of the physical object at a given time. We define a unique pointer to the Position_generator object, which is a base class for different types of position generators. 
    Position PhysicalObject::getPosition(float time) const {
        return pos_gen->position_getter(time);
    }

    Velocity PhysicalObject::getVelocity(float time) const {
        return pos_gen->velocity_getter(time);
    }

    Acceleration PhysicalObject::getAcceleration(float time) const {
        return pos_gen->acceleration_getter(time);
    }
