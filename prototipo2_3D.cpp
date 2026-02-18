#include <iostream>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
#include <filesystem>
using namespace std;



void save_to_csv(const vector<float>& time, const vector<float>& data, const string& filename) {
    ofstream file(filename);
    for (size_t i = 0; i < time.size(); ++i)
        file << time[i] << "," << data[i] << "\n";
}
class Source {
    private:
    double Amplitude;
    double frequency;
    int SampleRate;
    int bufferSize;

    public:
    //constructor
    Source(double amp, double freq, int sr, int bs)
    : Amplitude(amp), frequency(freq), SampleRate(sr), bufferSize(bs) {}

    vector<float> buffer_source() const {

        vector<float> source(bufferSize);
        for (int n=0; n<bufferSize; n++){
            source[n] = Amplitude * sin(2.0 * M_PI * frequency * n / SampleRate);
        }
        return source;     
        }

    vector<float> time_source() const {
        vector<float> time_s(bufferSize);
        for (int n=0; n<bufferSize; n++){
            time_s[n] = static_cast<float>(n) / static_cast<float>(SampleRate);  
        } 
        return time_s;  
    } 



    double getSampleRate() const{
        return SampleRate;
    }

    double getBufferSize() const{
        return bufferSize;
    }
};

class Environment {
    private:
    double x_source;
    double x_left;
    double x_right;
    double y_source;
    double y_left;
    double y_right;
    double z_source;
    double z_left;
    double z_right;
    double v;

    public:
    //constructor
    Environment(double xs, double xl, double xr, double ys, double yl, double yr, double zs, double zl, double zr, double speed)
    : x_source(xs), x_left(xl), x_right(xr), y_source(ys), y_left(yl),
    y_right(yr), z_source(zs), z_left(zl), z_right(zr), v(speed) {}

    pair<double,double> DelayTime() const{
        double delayLeft = sqrt((x_left - x_source)*(x_left - x_source) + (y_left - y_source)*(y_left - y_source) +
        (z_left - z_source)*(z_left - z_source))/v;
        double delayRight = sqrt((x_right - x_source)*(x_right - x_source) + (y_right - y_source)*(y_right - y_source) +
        (z_right - z_source)*(z_right - z_source))/v;  
        return make_pair(delayLeft, delayRight);
    }

    pair<int,int> DelaySample(const Source& src) const{
        auto [delayLeft, delayRight] = DelayTime();
        int delaySampleLeft = delayLeft*src.getSampleRate();
        int delaySampleRight = delayRight*src.getSampleRate();      
        return make_pair(delaySampleLeft, delaySampleRight);
    }

    double get_x_left() const{
        return x_left;
    }

    double get_x_right() const{
        return x_right;
    }
};

class Microphones {
    private:
       // vector<float> micLeftTime; 
       // vector<float> micRightTime; 
        vector<float> micLeft;
        vector<float> micRight; 

    public:
    pair<vector<float>, vector<float>> capture(const Source& src, const Environment& env) {
        auto [delaySampleLeft, delaySampleRight] = env.DelaySample(src);
        auto buffer = src.buffer_source();
        micLeft.resize(src.getBufferSize() + delaySampleLeft);
        micRight.resize(src.getBufferSize() + delaySampleRight); 
        //int totalLength = src.getBufferSize() + max(delaySampleLeft, delaySampleRight);
        for (int n=0; n<micLeft.size(); n++){
            if ((n-delaySampleLeft) < 0) {
                micLeft[n] = 0;
            }
            else {
                micLeft[n] = buffer[n-delaySampleLeft];
               // micLeftTime[n] = static_cast<float>(n-delaySampleLeft)/static_cast<float>(src.getSampleRate());
            }
        }
        for (int n=0; n<micRight.size(); n++){
            if ((n-delaySampleRight) < 0) {
                micRight[n] = 0;
               // micRightTime[n] = 0;
            }
            else {
                micRight[n] = buffer[n-delaySampleRight];
               // micRightTime[n] = static_cast<float>(n-delaySampleRight)/static_cast<float>(src.getSampleRate());
            }
        }
        return make_pair(micLeft, micRight);
        }
    };
    /*

    vector<float> getLeftTime() const {
        return micLeftTime;
    }
    vector<float> getRightTime() const {
        return micRightTime;
    }
};

*/

int main(){
    //let's simulate the discretized movement of the microphones throughout 3 different x positions
    Source src(0.5, 440.0, 44100, 512);
    Environment env1(0, -1.0, 1.0, 0, 0, 0, 0, 0, 0, 343); //for now change x over constant y and z
    Environment env2(0, 1.0, 5.0, 0, 0, 0, 0, 0, 0, 343);
    Environment env3(0, 4.0, 12.0, 0, 0, 0, 0, 0, 0, 343);
    Microphones mic;
    auto buffer = src.buffer_source();
    auto buffer_time = src.time_source();
    env1.DelayTime();
    env2.DelayTime();
    env3.DelayTime();
    env1.DelaySample(src);
    env2.DelaySample(src);
    env3.DelaySample(src);
    auto [micLeft1, micRight1] = mic.capture(src, env1);
    auto [micLeft2, micRight2] = mic.capture(src, env2);
    auto [micLeft3, micRight3] = mic.capture(src, env3);
    cout << "Samples at microphone on the left in the 1st interval: " << micLeft1.size() << endl;
    cout << "Samples at microphone on the right in the 1st interval: " << micRight1.size() << endl;
    cout << "Samples at microphone on the left in the 2nd interval: " << micLeft2.size() << endl;
    cout << "Samples at microphone on the right in the 2nd interval: " << micRight2.size() << endl;   
    cout << "Samples at microphone on the left in the 3rd interval: " << micLeft3.size() << endl;
    cout << "Samples at microphone on the right in the 3rd interval: " << micRight3.size() << endl;

    //compute velocity_left and velocity_right throughout total_time
    float total_time = static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate());
    float velocity_left = ((env3.get_x_left()-env1.get_x_left())/total_time);  //x_start = env1.get_x_left() and x_end = env3.get_x_left()
    float velocity_right = ((env3.get_x_right()-env1.get_x_right())/total_time);  //x_start = env1.get_x_right() and x_end = env3.get_x_right()


    //save csv files

    ofstream src_file("./source.csv");
    for (size_t i = 0; i < buffer.size(); ++i) {
        float time_source = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
         src_file << time_source << "," << buffer[i] << "\n";       
        };

    src_file.close();


    // Combine the left and right mic into one long signal (simulating motion)
    vector<float> micLeft_all;
    micLeft_all.insert(micLeft_all.end(), micLeft1.begin(), micLeft1.end());
    micLeft_all.insert(micLeft_all.end(), micLeft2.begin(), micLeft2.end());
    micLeft_all.insert(micLeft_all.end(), micLeft3.begin(), micLeft3.end());

    vector<float> micRight_all;
    micRight_all.insert(micRight_all.end(), micRight1.begin(), micRight1.end());
    micRight_all.insert(micRight_all.end(), micRight2.begin(), micRight2.end());
    micRight_all.insert(micRight_all.end(), micRight3.begin(), micRight3.end());


    // Make a time axis for plotting
    vector<float> time_left(micLeft_all.size());
    for (size_t i = 0; i < time_left.size(); ++i)
        time_left[i] = static_cast<float>(i) / static_cast<float>(src.getSampleRate());

    vector<float> time_right(micRight_all.size());
    for (size_t i = 0; i < time_right.size(); ++i)
        time_right[i] = static_cast<float>(i) / static_cast<float>(src.getSampleRate());

    string filenames[2] = {"./mic_left.csv", "./mic_right.csv"};
    ofstream ofs;

    //left_file
    //ver problema na escrita, micrightall tá a dar valores mas n ta a salvar devidamente no ficheiro csv
    ofs.open(filenames[0]);
        for (size_t i = 0; i < time_left.size(); ++i) {
            float x_mic_left = env1.get_x_left() + velocity_left*time_left[i];
            Environment env_moving(0, x_mic_left, 0, 0, 0, 0, 0, 0, 0, 343);
            env_moving.DelayTime();
            auto [delaySampleLeft, delaySampleRight] = env_moving.DelaySample(src);
            if ((i-delaySampleLeft) < 0) {
                micLeft_all[i] = 0;
                }
            else {
                micLeft_all[i] = buffer[i-delaySampleLeft];
                }
            ofs << time_left[i] << "," << micLeft_all[i] << "\n"; 
        };
        ofs.close();
        cout << "Saved mic_left.csv with simulated movement!" << endl;

        //right_file
    ofs.open(filenames[1]);
        for (size_t i = 0; i < time_right.size(); ++i) {
            float x_mic_right = env1.get_x_right() + velocity_right*time_right[i];
            Environment env_moving(0, 0, x_mic_right, 0, 0, 0, 0, 0, 0, 343);
            env_moving.DelayTime();
            auto [delaySampleLeft, delaySampleRight] = env_moving.DelaySample(src);
            if ((i-delaySampleRight) < 0) {
                micRight_all[i] = 0;
                }
            else {
                micRight_all[i] = buffer[i-delaySampleRight];
                }
            ofs << time_right[i] << "," << micRight_all[i] << "\n"; 
        };
        ofs.close();
        cout << "Saved mic_right.csv with simulated movement!" << endl;
    


    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    return 0;
    }
