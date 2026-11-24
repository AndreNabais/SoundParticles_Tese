#include "Distance.h"
#include "Source.h"
#include "Microphone.h"
#include "Position_generator.h"

float Source_Displacement(const Source& source, int i, int Sample_rate) {

    float dx = source.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).x - source.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).x;
    float dy = source.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).y - source.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).y;
    float dz = source.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).z - source.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).z;
    float d_source = sqrt(dx * dx + dy * dy + dz * dz);

    return d_source;

}

float Microphone_Displacement(const Microphone& mic, int i, int Sample_rate) {


    float dx = mic.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).x - mic.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).x;
    float dy = mic.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).y - mic.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).y;
    float dz = mic.getPosition(static_cast<float>(i)/static_cast<float>(Sample_rate)).z - mic.getPosition(static_cast<float>(i-1)/static_cast<float>(Sample_rate)).z;
    float d_microphone = sqrt(dx * dx + dy * dy + dz * dz);

    return d_microphone;

}


float Distance_source_microphone(const Source& source, const Position& mic_pos, int i, int Sample_rate) {
    // distance from the source at sample i to the mic position

    Position src_pos = source.getPosition(static_cast<float>(i) / static_cast<float>(Sample_rate)); 

    float dx = mic_pos.x - src_pos.x;
    float dy = mic_pos.y - src_pos.y;
    float dz = mic_pos.z - src_pos.z;

    return sqrt(dx * dx + dy * dy + dz * dz);
}
