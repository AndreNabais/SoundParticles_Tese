#include "Distance.h"
#include "Source.h"
#include "Microphone.h"

float Distance_source(const Source& source, int i) {

    float dx = source.getPositionSample(i).x - source.getPositionSample(i-1).x;
    float dy = source.getPositionSample(i).y - source.getPositionSample(i-1).y;
    float dz = source.getPositionSample(i).z - source.getPositionSample(i-1).z;
    float d_source = sqrt(dx * dx + dy * dy + dz * dz);

    return d_source;

}

float Distance_microphone(const Microphone& mic, int i) {


    float dx = mic.getPositionSample(i).x - mic.getPositionSample(i-1).x;
    float dy = mic.getPositionSample(i).y - mic.getPositionSample(i-1).y;
    float dz = mic.getPositionSample(i).z - mic.getPositionSample(i-1).z;
    float d_microphone = sqrt(dx * dx + dy * dy + dz * dz);

    return d_microphone;
}

float Distance_source_microphone(const Source& source, const Microphone& mic, int i, int current_sample) {
    //this is the distance to check if the sound from the source at sample i has reached the microphone at sample current_sample

    float dx = mic.getPositionSample(current_sample).x - source.getPositionSample(i).x;
    float dy = mic.getPositionSample(current_sample).y - source.getPositionSample(i).y;
    float dz = mic.getPositionSample(current_sample).z - source.getPositionSample(i).z;
    float d_check = sqrt(dx * dx + dy * dy + dz * dz);

    return d_check;
}


