//THIS CODE IMPLEMENTATION SIMULATES THE DELAY PROPAGATION BETWEEN A STATIONARY SOURCE AND 2 STATIONARY MIC, WHERE MICROPHONE 1 IS 1 METER AWAY FROM THE SOURCE AND MICROPHONE 2 IS 2 METERS AWAY FROM THE SOURCE.

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
    


    vector<Microphone> mic_vec; //start with 2 microphone
        mic_vec.push_back(Microphone(bufferSize, -1.0f, 0.0f, 0.0f)); 
        mic_vec.push_back(Microphone(bufferSize, 2.0f, 0.0f, 0.0f)); 

        
    

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


    ofstream mic_file("./microphone1.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_mic = static_cast<float>(i)/static_cast<float>(sources_vec[0].getSampleRate());
            mic_file << time_mic << "," << mic_vec[0].getCapturedSample(i) << "\n";
            };
    mic_file.close();

    ofstream mic_file2("./microphone2.csv");
        for (size_t i = 0; i < bufferSize; ++i) {
            float time_mic = static_cast<float>(i)/static_cast<float>(sources_vec[0].getSampleRate());
            mic_file2 << time_mic << "," << mic_vec[1].getCapturedSample(i) << "\n";
            };
    mic_file2.close();

    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    
    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();

    // Calculate duration
    // Use microseconds (us) if the code runs very fast, or milliseconds (ms) for longer simulations
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    return 0;
    }

