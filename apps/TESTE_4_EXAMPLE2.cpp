//THIS CODE IMPLEMENTATION SIMULATES THE DOPPLER EFFECT WHEN A SOURCE APPROACHES A STATIONARY MICROPHONE WITH A CONSTANT VELOCITY.

#include "Acceleration.h"
#include "Distance.h"
#include "io_utils.h"
#include "Microphone.h"
#include "PhysicalObject.h"
#include "Source.h"
#include "Velocity.h"
#include "Position.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std;


int main(){

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    vector <Source> sources_vec;
    int bufferSize = 2048;
    float source_frequency = 440.0f; // Frequency of the sound wave in Hz
    float velocity_sound = 343.0f; // Speed of sound in m/s
    //start with 1 source
        sources_vec.push_back(Source(0.5, source_frequency, 44100, bufferSize, 0, 0, 0));

    
    float wavelength_distance = velocity_sound / source_frequency; //wavelength distance in meters for a 440Hz sound wave

    vector<Microphone> mic_vec; //start with 1 microphone
        mic_vec.push_back(Microphone(bufferSize, 4*wavelength_distance, 0.0f, 0.0f)); //initialize each microphone at position (1,0,0)

    for (size_t i = 0; i < sources_vec.size(); ++i) {
        sources_vec[i].setVelocity(Velocity(50.0f, 0.0f, 0.0f)); // Set a constant velocity of 50 m/s for each source
    }
    
    for (size_t j = 0; j < mic_vec.size(); ++j) {
        mic_vec[j].setVelocity(Velocity(0.0f, 0.0f, 0.0f)); // Set a constant velocity of 60 m/s for each microphone
    }

    //Doppler equation for observer approaching: f' = f*(v + vr)/(v - vs)
    //Doppler equation for observer moving further away: f' = f*(v - vr)/(v + vs)

    for (size_t i = 0; i < bufferSize; ++i) {  


        for (size_t j = 0; j < sources_vec.size(); ++j) {
            sources_vec[j].updatePosition(i, 1.0f / sources_vec[0].getSampleRate());
            sources_vec[j].updateVelocity(i, 1.0f / sources_vec[0].getSampleRate());
    
            for (size_t k = 0; k < mic_vec.size(); ++k) {
                mic_vec[k].updatePosition(i, 1.0f / sources_vec[0].getSampleRate());
                mic_vec[k].updateVelocity(i, 1.0f / sources_vec[0].getSampleRate());
                mic_vec[k].update_recv_sound(sources_vec, i);
            
                float observed_frequency;
                Velocity closing_v_instant = Closing_velocity_instant(sources_vec[j], mic_vec[k], i, i); 
                // closing_v_instant > 0  → distance shrinking → source or microphone approaching
                // closing_v_instant < 0  → distance growing  → source or microphone receding

                if (closing_v_instant.x > 0) {
                    // Observer is moving slower than the source, moving towards
                    observed_frequency = fabs((source_frequency) * (velocity_sound + mic_vec[k].getVelocitySample(i).x) / (velocity_sound - sources_vec[j].getVelocitySample(i).x));
                    } 
                else if (closing_v_instant.x < 0) {
                    // Observer is moving faster than the source, moving away
                    observed_frequency = fabs((source_frequency) * (velocity_sound - mic_vec[k].getVelocitySample(i).x) / (velocity_sound + sources_vec[j].getVelocitySample(i).x));
                    }

                else {
                    // Observer and source are moving at the same speed, no relative motion
                    observed_frequency = source_frequency;
                    //cout << "Observed frequency at microphone " << j+1 << ": " << observed_frequency << " Hz" << endl;
                    } 


                if (i % 500 == 0) {
                    cout << "Observed frequency at microphone " << (k + 1) 
                    << " (Sample " << i << "): " << observed_frequency << " Hz" << endl;
                }
            }
        }
    }


    

    //save csv files

    ofstream src_file("./source.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_source = static_cast<float>(i)/static_cast<float>(sources_vec[0].getSampleRate());
            src_file << time_source << "," << sources_vec[0].getSignalSample(i) << "\n";
            };
    src_file.close();


    ofstream mic_file("./microphone.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_mic = static_cast<float>(i)/static_cast<float>(sources_vec[0].getSampleRate());
            mic_file << time_mic << "," << mic_vec[0].getCapturedSample(i) << "\n";
            };
    mic_file.close();

    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    
    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();

    // Calculate duration
    // Use microseconds (us) if the code runs very fast, or milliseconds (ms) for longer simulations
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    return 0;
    }