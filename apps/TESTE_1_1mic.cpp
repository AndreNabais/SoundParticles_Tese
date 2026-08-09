//THIS CODE IMPLEMENTATION SIMULATES THE DELAY PROPAGATION BETWEEN A STATIONARY MIC AND A STATIONARY SOURCE, BOTH 1 METER APART

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
    //start with 1 source
        sources_vec.push_back(Source(0.5, 440.0, 44100, bufferSize, 0, 0, 0));

    
    float wavelength_distance = 343.0f / 440.0f; //wavelength distance in meters for a 440Hz sound wave

    vector<Microphone> mic_vec; //start with 1 microphone
        mic_vec.push_back(Microphone(bufferSize, 1.0, 0.0f, 0.0f)); //initialize each microphone at position (1,0,0)
    

    for (size_t i = 0; i < bufferSize; ++i) {
        for (size_t j = 0; j < mic_vec.size(); ++j) {
            mic_vec[j].update_recv_sound(sources_vec, i);
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