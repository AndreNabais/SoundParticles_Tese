#include "Velocity.h"
#include "Distance.h"
#include "Source.h"
#include "Microphone.h"

Velocity Velocity_source(Source& source, int i) {

    float d_s = Distance_source(source, i);
    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_source = Velocity(d_s / dt, 0.0f, 0.0f);
    source.setVelocity(v_source);

    return v_source;
}

Velocity Velocity_microphone(Source& source, Microphone& mic,  int i) {

    float d_m = Distance_microphone(mic, i);
    float dt = 1.0f / source.getSampleRate(); //time step between samples
    Velocity v_microphone = Velocity(d_m / dt, 0.0f, 0.0f);
    mic.setVelocity(v_microphone);

    return v_microphone;
}

Velocity Closing_velocity(const Source& source, const Microphone& mic, int i, int current_sample) {

    float distance_check = Distance_source_microphone(source, mic, i, current_sample);
    float prev_distance_check = Distance_source_microphone(source, mic, i-1, current_sample);
    Velocity closing_v = Velocity((prev_distance_check - distance_check) * source.getSampleRate(), 0.0f, 0.0f); //relative velocity between source and microphone. positive if approaching, negative if moving away. This is the rate of change of distance between source and microphone in m/s
    return closing_v;
}

Velocity Closing_velocity_instant(const Source& source, const Microphone& mic, int i, int current_sample) {

    float distance_check = Distance_source_microphone(source, mic, i, current_sample);
    float prev_distance_check = Distance_source_microphone(source, mic, i-1, current_sample-1);
    Velocity closing_v_instant = Velocity((prev_distance_check - distance_check) * source.getSampleRate(), 0.0f, 0.0f); //relative velocity between source and microphone. positive if approaching, negative if moving away. This is the rate of change of distance between source and microphone in m/s
    return closing_v_instant;
}