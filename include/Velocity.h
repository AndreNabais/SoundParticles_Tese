#ifndef VELOCITY_H
#define VELOCITY_H

#include "Position.h"

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

Velocity Velocity_source(Source& source, int i, int Sample_rate);
Velocity Velocity_microphone(Source& source, Microphone& mic, int i, int Sample_rate);
float Closing_velocity_instant(const Position& src_pos, const Position& mic_pos, const Position& prev_src_pos, const Position& prev_mic_pos, int Sample_rate);
float Velocity_Magnitude(const Velocity& v);

#endif // VELOCITY_H
