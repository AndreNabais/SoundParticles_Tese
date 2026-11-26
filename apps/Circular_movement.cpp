//THIS CODE IMPLEMENTATION SIMULATES THE DOPPLER EFFECT WHEN A SOURCE APPROACHES A STATIONARY MICROPHONE WITH A CONSTANT VELOCITY.

#include "Acceleration.h"
#include "Distance.h"
#include "io_utils.h"
#include "Microphone.h"
#include "PhysicalObject.h"
#include "Source.h"
#include "Velocity.h"
#include "Position.h"
#include "Position_generator.h"
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
    int SampleRate = 44100;
    float source_frequency = 440.0f; // Frequency of the sound wave in Hz
    float velocity_sound = 343.0f; // Speed of sound in m/s
    float wavelength_distance = velocity_sound / source_frequency; //wavelength distance in meters for a 440Hz sound wave

    //start with 1 source
    //make source describe a circular movement around the mic
    auto pos_gen_source = std::make_unique<Circular_position_generator>(Position{2.0, 0, 0}, Position{0, 0.0, 0.0f}, 100.0f);
    sources_vec.push_back(Source(std::move(pos_gen_source), 0.5, source_frequency, SampleRate, bufferSize)); //initialize the source at position (0,0,0) with 0 velocity

    vector<Microphone> mic_vec; //start with 1 microphone
    //make microphone stationary at position (0,0,0)
    auto pos_gen_microphone = std::make_unique<Position_generator>(Position{0, 0.0f, 0.0f});
    mic_vec.push_back(Microphone(std::move(pos_gen_microphone), bufferSize, SampleRate)); //initialize the microphone at position (0,0,0)

    //Doppler equation for observer approaching: f' = f*(v + vr)/(v - vs)
    //Doppler equation for observer moving further away: f' = f*(v - vr)/(v + vs)


    for (size_t i = 0; i < bufferSize; ++i) { 
        float time = static_cast<float>(i) / static_cast<float>(SampleRate);

        for (size_t j = 0; j < sources_vec.size(); ++j) {
            sources_vec[j].SignalGenerator(sources_vec[j], i);
        }

        for (size_t k = 0; k < mic_vec.size(); ++k) {
            mic_vec[k].update_recv_sound(sources_vec, i);
        
            
            for (size_t j = 0; j < sources_vec.size(); ++j) {
                if (i % 500 == 0) {
                    cout << "Microphone " << (k + 1) << " position at sample " << (i) << ": ("
                    << mic_vec[k].getPosition(time).x << ", "
                    << mic_vec[k].getPosition(time).y << ", "
                    << mic_vec[k].getPosition(time).z << ")" << endl;

                    cout << "Source " << (j+1) << " position at sample " << (i) << ": ("
                    << sources_vec[j].getPosition(time).x << ", "
                    << sources_vec[j].getPosition(time).y << ", "
                    << sources_vec[j].getPosition(time).z << ")" << endl;

                    cout << "Velocity at Microphone " << (k+1) << ": " << mic_vec[k].getVelocity(time).x << "m/s"<< endl;
                    cout << "Velocity at Source " << (j+1) << ": " << sources_vec[j].getVelocity(time).x << "m/s"<< endl;
                }

                //use a cardioid mic
                //mic_vec[0].setPolarPattern(PolarPattern::Cardioid, Position{1.0, 0.0f, 0.0f});

                float observed_frequency;
                // closing_v_instant > 0  → distance shrinking → source or microphone approaching
                // closing_v_instant < 0  → distance growing  → source or microphone receding
                float closing_v_instant = 0.0f;
                if (i > 1) {
                    closing_v_instant = Closing_velocity_instant(sources_vec[j].getSourcePositions()[i], mic_vec[k].getPosition(static_cast<float>(i)/static_cast<float>(SampleRate)), sources_vec[j].getSourcePositions()[i-1], mic_vec[k].getPosition(static_cast<float>(i-1)/static_cast<float>(SampleRate)), SampleRate); 
                }

                float closing_v_previous = 0.0f;
                if (i > 2) {
                    closing_v_previous = Closing_velocity_instant(sources_vec[j].getSourcePositions()[i-1], mic_vec[k].getPosition(static_cast<float>(i-1)/static_cast<float>(SampleRate)), sources_vec[j].getSourcePositions()[i-2], mic_vec[k].getPosition(static_cast<float>(i-2)/static_cast<float>(SampleRate)), SampleRate);
                }

                observed_frequency = source_frequency * velocity_sound / (velocity_sound - closing_v_instant);


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
            float time_source = static_cast<float>(i)/static_cast<float>(SampleRate);
            src_file << time_source << "," << sources_vec[0].getSignalSample(i) << "\n";
            };
    src_file.close();


    ofstream mic_file("./microphone.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_mic = static_cast<float>(i)/static_cast<float>(SampleRate);
            mic_file << time_mic << "," << mic_vec[0].getCapturedSample(i) << "\n";
            };
    mic_file.close();

    ofstream traj_file("./Circular.csv"); 
    traj_file << "time,source_x,source_y,source_z,mic_x,mic_y,mic_z\n";
        for (size_t i = 0; i < bufferSize; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(SampleRate);
            Position ps = sources_vec[0].getPosition(t);
            Position pm = mic_vec[0].getPosition(t);
            traj_file << t << ","
                      << ps.x << "," << ps.y << "," << ps.z << ","
                      << pm.x << "," << pm.y << "," << pm.z << "\n";
            };
    traj_file.close();

    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    
    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();

    // Calculate duration
    // Use microseconds (us) if the code runs very fast, or milliseconds (ms) for longer simulations
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    return 0;
    }
