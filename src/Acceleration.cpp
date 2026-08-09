#include "Acceleration.h"
#include "Source.h"
#include "Microphone.h"
#include "Velocity.h"


Acceleration Acceleration_source(Source& source, int i) {

    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_s = Velocity_source(source, i);
    Velocity previous_v_s = Velocity_source(source, i-1);
    Acceleration a_source;
    a_source.x = (v_s.x - previous_v_s.x) / dt;
    a_source.y = (v_s.y - previous_v_s.y) / dt;
    a_source.z = (v_s.z - previous_v_s.z) / dt;
    source.setAcceleration(a_source);

    return a_source;
}

Acceleration Acceleration_microphone(Source& source, Microphone& mic, int i) {

    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_m = Velocity_microphone(source, mic, i);
    Velocity previous_v_m = Velocity_microphone(source, mic, i-1);
    Acceleration a_microphone;
    a_microphone.x = (v_m.x - previous_v_m.x) / dt;
    a_microphone.y = (v_m.y - previous_v_m.y) / dt;
    a_microphone.z = (v_m.z - previous_v_m.z) / dt;
    mic.setAcceleration(a_microphone);

    return a_microphone;
}