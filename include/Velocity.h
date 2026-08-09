#ifndef VELOCITY_H
#define VELOCITY_H

#include "Position.h"
#include "PhysicalObject.h"

//forward declaration of the Source and Microphone classes
class Source;
class Microphone;

struct Velocity {
    float x;
    float y;
    float z;

    Velocity() : x(0.0f), y(0.0f), z(0.0f) {}
    Velocity(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

Velocity Velocity_source(Source& source, int i);
Velocity Velocity_microphone(Source& source, Microphone& mic, int i);
Velocity Closing_velocity(const Source& source, const Microphone& mic, int i, int current_sample);
Velocity Closing_velocity_instant(const Source& source, const Microphone& mic, int i, int current_sample);

#endif // VELOCITY_H