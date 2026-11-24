#ifndef DISTANCE_H
#define DISTANCE_H

#include "Position.h"
#include "PhysicalObject.h"

//forward declaration of the Source and Microphone classes
class Source;
class Microphone;

float Source_Displacement(const Source& source, int i, int Sample_rate);
float Microphone_Displacement(const Microphone& mic, int i, int Sample_rate);
float Distance_source_microphone(const Source& source, const Position& mic_pos, int i, int Sample_rate);

#endif // DISTANCE_H
