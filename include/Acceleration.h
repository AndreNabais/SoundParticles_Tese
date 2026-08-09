#ifndef ACCELERATION_H
#define ACCELERATION_H

#include "Position.h"
#include "PhysicalObject.h"

//forward declaration of the Source and Microphone classes
class Source;
class Microphone;

struct Acceleration {
    float x;
    float y;
    float z;

    Acceleration() : x(0.0f), y(0.0f), z(0.0f) {}
    Acceleration(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

Acceleration Acceleration_source(Source& source, int i);
Acceleration Acceleration_microphone(Source& source, Microphone& mic, int i);

#endif // ACCELERATION_H