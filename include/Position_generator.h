#ifndef POSITION_GENERATOR_H
#define POSITION_GENERATOR_H

//Position_generator is the superclassfor position generators.
#include "Position.h"
#include "Velocity.h"
#include "Acceleration.h"

//forward declaration of the Velocity and Acceleration structs
struct Velocity;
struct Acceleration;

//use polymorhism to be able to use different position generators for different physical objects.
class Position_generator {
    public:
        Position_generator(Position start_pos): pos_init(start_pos) {};
        virtual Position position_getter(float time) const;
        virtual Velocity velocity_getter(float time) const;
        virtual Acceleration acceleration_getter(float time) const;
        virtual ~Position_generator() = default; // virtual destructor to ensure proper cleanup of derived classes
        
    protected:
        Position pos_init;
        Velocity velocity;
        Acceleration acceleration;
};

class Horizontal_position_generator : public Position_generator {
    public:
        Horizontal_position_generator(Position start_pos, Velocity velocity, Acceleration acceleration):Position_generator(start_pos){
            this->velocity = velocity;
            this->acceleration = acceleration;
        };
        Position position_getter(float time) const override;
        Velocity velocity_getter(float time) const override;
        Acceleration acceleration_getter(float time) const override;
};


class Vertical_position_generator : public Position_generator {
    public:
        Vertical_position_generator(Position start_pos, Velocity velocity, Acceleration acceleration):Position_generator(start_pos){
            this->velocity = velocity;
            this->acceleration = acceleration;
        };
        Position position_getter(float time) const override;
        Velocity velocity_getter(float time) const override;
        Acceleration acceleration_getter(float time) const override;
};

class Circular_position_generator : public Position_generator {
    public: 
        Circular_position_generator(Position start_pos, Position center_pos, float angular_velocity):Position_generator(start_pos){
            this->center_pos = center_pos;
            this->angular_velocity = angular_velocity;
        };
        Position position_getter(float time) const override;
        Velocity velocity_getter(float time) const override;
        Acceleration acceleration_getter(float time) const override;

    private:
        Position center_pos;
        float angular_velocity;  // rad/s, positive = counterclockwise. Rotation around a fixed axis (z) is described a single number (float) instead of a vector.
};


#endif
