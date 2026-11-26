#include "Velocity.h"
#include "Distance.h"
#include "Source.h"
#include "Microphone.h"

Velocity Velocity_source(Source& source, int i, int Sample_rate) {

    float d_s = Source_Displacement(source, i, Sample_rate);
    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_source = Velocity(d_s / dt, 0.0f, 0.0f);
    source.setVelocity(v_source);

    return v_source;
}

Velocity Velocity_microphone(Source& source, Microphone& mic,  int i, int Sample_rate) {

    float d_m = Microphone_Displacement(mic, i, Sample_rate);
    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_microphone = Velocity(d_m / dt, 0.0f, 0.0f);
    mic.setVelocity(v_microphone);

    return v_microphone;
}


float Closing_velocity_instant(const Position& src_pos, const Position& mic_pos, const Position& prev_src_pos, const Position& prev_mic_pos, int Sample_rate) {

    float distance_instant = Distance_source_microphone(src_pos, mic_pos);
    float distance_previous = Distance_source_microphone(prev_src_pos, prev_mic_pos);

    float closing_v_instant = (distance_previous - distance_instant) * Sample_rate; //relative velocity between source and microphone. positive if approaching, negative if moving away. This is the rate of change of distance between source and microphone in m/s
    return closing_v_instant;
}


float Velocity_Magnitude(const Velocity& v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}


float Velocity_Magnitude(const Velocity& v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
