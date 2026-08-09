//THIS CODE IMPLEMENTATION SIMULATES THE CONSTRUCTIVE AND DESTRUCTIVE INTERFERENCE OBTAINED IN A STATIONARY MICROPHONE FROM 2 STATIONARY SOURCES GENERATING SIGNALS.

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
    float wavelength_distance = 343.0f / 440.0f; //distance in meters for a 440Hz sound wave to travel one full cycle at the speed of sound (343 m/s)

    vector <Source> sources_vec;
    int bufferSize = 2048;
    //start with 2 sources
        sources_vec.push_back(Source(0.5, 440.0, 44100, bufferSize, 0, 0, 0));
        sources_vec.push_back(Source(-0.5, 440.0, 44100, bufferSize, 0, 0, 0));
        //use 0.5 amplitude for both sources to obtain constructive interference and -0.5 amplitude for the second source to obtain destructive interference


    vector<Microphone> mic_vec; //start with 2 microphones
        mic_vec.push_back(Microphone(bufferSize, 0, 0.0f, 0.0f)); //initialize the microphone at position (0,0,0)


    for (size_t i = 0; i < bufferSize; ++i) {
        for (size_t j = 0; j < mic_vec.size(); ++j) {
            mic_vec[j].update_recv_sound(sources_vec, i);
        }        

    }

    

    //save csv files

    ofstream src_file("./source1.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_source = static_cast<float>(i)/static_cast<float>(sources_vec[0].getSampleRate());
            src_file << time_source << "," << sources_vec[0].getSignalSample(i) << "\n";
            };
    src_file.close();

    ofstream src_file2("./source2.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_source = static_cast<float>(i)/static_cast<float>(sources_vec[1].getSampleRate());
            src_file2 << time_source << "," << sources_vec[1].getSignalSample(i) << "\n";
            };
    src_file2.close();


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