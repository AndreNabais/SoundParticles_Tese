#ifndef DISTANCE_H
#define DISTANCE_H

#include "Position.h"
#include "PhysicalObject.h"

//forward declaration of the Source and Microphone classes
class Source;
class Microphone;

float Distance_source(const Source& source, int i);
float Distance_microphone(const Microphone& mic, int i);
float Distance_source_microphone(const Source& source, const Microphone& mic, int i, int current_sample);

#endif // DISTANCE_H