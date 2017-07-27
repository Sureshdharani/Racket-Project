/* 
    Copyright 2017,
    Suresh ...
*/

#include <signal.h>
#include <mraa.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <exception>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>  // NOLINT()
#include <ctime>  // NOLINT()
#include <ratio>  // NOLINT()
#include <string>
#include <atomic>
#include <thread>  // NOLINT()
#include <cmath>

#include "I2Cdev_mraa.hpp"
#include "MPU9250_DMP.hpp"
#include "helper_3dmath.hpp"

// #define DEBUG

// #define QUATERNION
#define EULER_ANGLES

#define MPU_Addr 0x69
#define I2C_Bus_no 1

#define DIVIDER static_cast<float>(16384.0)

// https://www.ptb.de/cartoweb3/SISproject.php
#define g static_cast<float>(9.808227)

using std::vector;
using std::string;
using std::cout;
using std::endl;

// Typedefs:

// Command line parser output
struct CmdParserOut {
    bool isValid = false;
    std::string error = "Wrong parameters: ";
};

// Settings of the client
struct ClientSettings {
    std::string ip = "127.0.0.1";
    unsigned int port = 5554;
    unsigned int bufLen = 100;
    unsigned int udpBufLen = 300;
    unsigned int measWaitTime = 2000;
    bool isCmdPrint = false;
    bool isLog = false;
    bool isSend = true;
};

MPU9250 mpu;

// MRAA variables
mraa_gpio_context S0;
mraa_gpio_context S1;
mraa_gpio_context S2;
mraa_gpio_context S3;
mraa_i2c_context MPU9250_i2c;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;  // holds actual interrupt status byte from MPU

// return status after each device operation (0 = success, !0 = error)
uint8_t devStatus;
uint16_t packetSize;  // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;  // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
Quaternion q;  // [w, x, y, z] - quaternion container
VectorInt16 aa;  // [x, y, z] - accel sensor measurements
VectorInt16 aaReal;  // [x, y, z] - gravity-free accel sensor measurements
VectorInt16 aaWorld;  // [x, y, z] - world-frame accel sensor measurements
VectorFloat gravity;  // [x, y, z] - gravity vector
float euler[3];  // [psi, theta, phi] - Euler angle container
float ypr[3];  // [yaw, pitch, roll] - yaw/pitch/roll

// ----------------------------------------------------------------------------
struct Motion_param {
    // Acceleration
    float Acc_x_Raw = 0;
    float Acc_y_Raw = 0;
    float Acc_z_Raw = 0;

    // Gyroscope:
    float Gyro_x_Raw = 0;
    float Gyro_y_Raw = 0;
    float Gyro_z_Raw = 0;

    // Quaternions
    float w = 0;
    float x = 0;
    float y = 0;
    float z = 0;

    // Orientations
    float oX = 0;
    float oY = 0;
    float oZ = 0;

    // Time stamp:
    int64_t Timestamp = 0;
};


// Functions declaration
Motion_param MPU9250_Loop();
void MPU9250_GPIO_Init();
void quat2euler(const float q_w,
                const float q_x, const float q_y, const float q_z,
                float *t_x, float *t_y, float *t_z);

// ----------------------------------------------------------------------------
void MPU9250_GPIO_Init() {
    S0 = mraa_gpio_init(32);
    S1 = mraa_gpio_init(33);
    S2 = mraa_gpio_init(46);
    S3 = mraa_gpio_init(47);

    mraa_gpio_dir(S0, MRAA_GPIO_OUT);
    mraa_gpio_dir(S1, MRAA_GPIO_OUT);
    mraa_gpio_dir(S2, MRAA_GPIO_OUT);
    mraa_gpio_dir(S3, MRAA_GPIO_OUT);
}

// ----------------------------------------------------------------------------
void MPU9250_MUX_Select() {
    mraa_gpio_write(S0, 1);
    mraa_gpio_write(S1, 0);
    mraa_gpio_write(S2, 1);
    mraa_gpio_write(S3, 0);
}

// MPU set to Idle mode
void MPU_Sleep(int wait_time_us) {
    usleep(wait_time_us);
}

// Get Time stamp
// ----------------------------------------------------------------------------
static int64_t Get_TimeSinceEpochMillis() {
    return static_cast<int64_t>
        (std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count());
}

// ----------------------------------------------------------------------------
Motion_param MPU9250_Loop() {
    // If programming failed, don't try to do anything
    // if(!dmpReady) return;

    // Get current FIFO count
    fifoCount = mpu.getFIFOCount();
    // Wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // Read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    int64_t Timestamp = Get_TimeSinceEpochMillis();
    Motion_param mp;

    // Display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    int16_t Acc_x_Raw = 0;
    int16_t Acc_y_Raw = 0;
    int16_t Acc_z_Raw = 0;
    int16_t Gyro_x_Raw = 0;
    int16_t Gyro_y_Raw = 0;
    int16_t Gyro_z_Raw = 0;

    mpu.getAcceleration(&Acc_x_Raw, &Acc_y_Raw, &Acc_z_Raw);
    mp.Acc_x_Raw  = g * (static_cast<float>(Acc_x_Raw) / DIVIDER);
    mp.Acc_y_Raw  = g * (static_cast<float>(Acc_y_Raw) / DIVIDER);
    mp.Acc_z_Raw  = g * (static_cast<float>(Acc_z_Raw) / DIVIDER);

    mpu.getRotation(&Gyro_x_Raw, &Gyro_y_Raw, &Gyro_z_Raw);
    mp.Gyro_x_Raw = static_cast<float>(Gyro_x_Raw);
    mp.Gyro_y_Raw = static_cast<float>(Gyro_y_Raw);
    mp.Gyro_z_Raw = static_cast<float>(Gyro_z_Raw);

    mp.w = q.w;
    mp.x = q.x;
    mp.y = q.y;
    mp.z = q.z;

    // Calculate orientation from quaternions:
    quat2euler(q.w, q.x, q.y, q.z, &(mp.oX), &(mp.oY), &(mp.oZ));
    mp.Timestamp = Timestamp;

    return mp;
}

// ----------------------------------------------------------------------------
void MPU9250_Setup() {
    MPU9250 mpu;
    cout << "------------------------------------------------------" << endl;

    // Select the Multiplexer Channel
    MPU9250_MUX_Select();
    cout << "Initializing I2C... " << endl;

    // Initialize the MPU
    mpu.initialize();

    // Report Connection
    if (mpu.testConnection()) {
        cout << "MPU9250 connection successful" << endl;
    } else {
        cout << "MPU9250 connection Failed" << endl;
    }

    // Initialize the DMP
    cout << "Initializing DMP... " << endl;
    devStatus = mpu.dmpInitialize();
    cout << "DMP Status: ";

    // Make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // Turn on the DMP, now that it's ready
        printf("Enabling DMP... \n");
        mpu.setDMPEnabled(true);

        // Set our DMP Ready flag so the main loop()
        // function knows it's okay to use it
        cout << "DMP Ready! " << endl;
        dmpReady = true;

        // Get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

    } else {
        cout << "DMP Initialization failed: ";
        cout << devStatus;
        cout  << ")" << endl;
    }

    cout << " Ready " << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "\n\tInitialization Complete: All Systems are GO!!!\n" << endl;
}

// ----------------------------------------------------------------------------
string Send_MsgStr(Motion_param mp) {
    string Send_Message;
    #ifdef QUATERNION
        Send_Message = "s@" + to_string(mp.Timestamp) + "@" +
                       to_string(mp.Acc_x_Raw) + "@" +
                       to_string(mp.Acc_y_Raw) + "@" +
                       to_string(mp.Acc_z_Raw) + "@" +
                       to_string(mp.Gyro_x_Raw) + "@" +
                       to_string(mp.Gyro_y_Raw) + "@" +
                       to_string(mp.Gyro_z_Raw) + "@" +
                       to_string(mp.w) + "@" +
                       to_string(mp.x) + "@" +
                       to_string(mp.y) + "@" +
                       to_string(mp.z) + "@;";
    #endif

    #ifdef EULER_ANGLES
        Send_Message = "s@" + to_string(mp.Timestamp) + "@" +
                       to_string(mp.Acc_x_Raw) + "@" +
                       to_string(mp.Acc_y_Raw) + "@" +
                       to_string(mp.Acc_z_Raw) + "@" +
                       to_string(mp.Gyro_x_Raw) + "@" +
                       to_string(mp.Gyro_y_Raw) + "@" +
                       to_string(mp.Gyro_z_Raw) + "@" +
                       to_string(mp.oX) + "@" +
                       to_string(mp.oY) + "@" +
                       to_string(mp.oZ) + "@;";
    #endif

    return Send_Message;
}

// ----------------------------------------------------------------------------
void SendUDP(const int sock, vector<Motion_param> someRec,
             const ClientSettings settings) {
    string Timestamp = (to_string)(Get_TimeSinceEpochMillis());
    int length;
    struct sockaddr_in server;
    struct hostent *hp;
    char buffer1[settings.udpBufLen];  // NOLINT()

    if (sock < 0)
        throw std::string("Wrong socket");

    server.sin_family = AF_INET;
    hp = gethostbyname(settings.ip.c_str());

    if (sock < 0)
        throw std::string("Unknown host");

    bcopy(reinterpret_cast<char *>(hp->h_addr),
          reinterpret_cast<char *>(&server.sin_addr), hp->h_length);
    server.sin_port = htons(settings.port);
    length = sizeof(struct sockaddr_in);

    string msg = "";
    for (unsigned int i = 0; i < settings.bufLen; i++) {
        // bzero(buffer1, settings.udpBufLen);
        msg = Send_MsgStr(someRec.at(i));
        memcpy(buffer1, msg.c_str(), msg.size() + 1);

    // std::cout<<"Message"<<msg<<"\n"<<std::endl;
    // usleep(200000);
    sendto(sock, buffer1, settings.udpBufLen, 0,
               (struct sockaddr *)&server, length);
        // printf(" Message %s\n",msg);
        // std::cout<<"Message"<<msg<<"\n"<<std::endl;
    }
}

// ----------------------------------------------------------------------------
void printUsage() {
    cout << "*** Racket Sensor Client ***" << endl;
    cout << "\tUsage:" << endl;
    cout << "\t\t-ip - host ip adress" << endl;
    cout << "\t\t-port - host port" << endl;
    cout << "\t\t-bufl - buffer length (typical 100)" << endl;
    cout << "\t\t-udpbufl - UDP buffer length (typical 300)" << endl;
    cout << "\t\t-mwaitus - measurement wait time in us (typical 2000)"
    << endl;
    cout << "\t\t-log - measurement data log (typical off)" << endl;
    cout << "\t\t-cmdprint - printig to cmd (typical off)" << endl;
    cout << "\t\t-send - sending over UDP (typical on)" << endl;
    cout << endl;
}

// ----------------------------------------------------------------------------
CmdParserOut parseCommandLineArgs(const char* argv[], int argc,
                                  ClientSettings *settings) {
    CmdParserOut parserOut;
    vector<string> args(argv, argv + argc);

    parserOut.isValid = true;
    string s = "";
    for (unsigned int i = 1; i < args.size(); i++) {
        s = args.at(i);
        i++;
        if (s == "-ip") {
            settings->ip = args.at(i);
        } else if (s == "-port") {
            settings->port = std::stoul(args.at(i));
        } else if (s == "-bufl") {
            settings->bufLen = std::stoul(args.at(i));
        } else if (s == "-udpbufl") {
            settings->udpBufLen = std::stoul(args.at(i));
        } else if (s == "-mwaitus") {
            settings->measWaitTime = std::stoul(args.at(i));
        } else if (s == "-log") {
            if (args.at(i) == "on") {
                settings->isLog = true;
            } else if (args.at(i) == "off") {
                settings->isLog = false;
            }
        } else if (s == "-cmdprint") {
            if (args.at(i) == "on") {
                settings->isCmdPrint = true;
            } else if (args.at(i) == "off") {
                settings->isCmdPrint = false;
            }
        } else if (s == "-send") {
            if (args.at(i) == "on") {
                settings->isSend = true;
            } else if (args.at(i) == "off") {
                settings->isSend = false;
            }
        } else {  // wrong parameter
            parserOut.isValid = false;
            parserOut.error = "Error: Wrong command line argument: " + s;
        }
    }
    return parserOut;
}

// ----------------------------------------------------------------------------
void logData(vector<Motion_param> motionRecord) {
    // Generate log file name:
    std::string logFileName = "DataLog_";

    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::ctime(&end_time);

    std::ostringstream oss;
    oss << std::ctime(&end_time);
    auto strTime = oss.str();
    strTime = strTime.substr(0, strTime.size()-1);  // delete last character

    logFileName.append(strTime);
    logFileName.append(".txt");

    std::ofstream logFile;
    logFile.open(logFileName);

    for (unsigned int i = 0; i < motionRecord.size(); i++)
        logFile << Send_MsgStr(motionRecord.at(i)) << "\n";

    logFile.close();
}

// ----------------------------------------------------------------------------
void readCin(std::atomic<bool> *run) {
    std::string buffer;
    while (run->load()) {
        std::cin >> buffer;
        if (buffer == "q")
            run->store(false);
    }
}

// ----------------------------------------------------------------------------
void quat2euler(const float q_w,
                const float q_x, const float q_y, const float q_z,
                float *t_x, float *t_y, float *t_z) {
    double ysqr = q_y * q_y;

    // roll (x-axis rotation)
    double t0 = +2.0 * (q_w * q_x + q_y * q_z);
    double t1 = +1.0 - 2.0 * (q_x * q_x + ysqr);
    *t_x = static_cast<float>(std::atan2(t0, t1));

    // pitch (y-axis rotation)
    double t2 = +2.0 * (q_w * q_y - q_z * q_x);
    t2 = ((t2 > 1.0) ? 1.0 : t2);
    t2 = ((t2 < -1.0) ? -1.0 : t2);
    *t_y = static_cast<float>(std::asin(t2));

    // yaw (z-axis rotation)
    double t3 = +2.0 * (q_w * q_z + q_x * q_y);
    double t4 = +1.0 - 2.0 * (ysqr + q_z * q_z);
    *t_z = static_cast<float>(std::atan2(t3, t4));
}

// ----------------------------------------------------------------------------
int main(int argc, const char* argv[]) {
    // Parse command line arguments:
    if (argc < 11) {
        // Print usage and exit:
        printUsage();
        return 0;
    }

    ClientSettings set;
    CmdParserOut parserOut = parseCommandLineArgs(argv, argc, &set);
    if (!parserOut.isValid) {  // not valid parameters
        // print wrong argument
        cout << parserOut.error << endl;
        // wrong command line arguments - print udage and exit:
        printUsage();
        return 0;
    }

    mraa_init();
    MPU9250_GPIO_Init();
    MPU_Sleep(set.measWaitTime);
    MPU9250_Setup();

    int Record_count = 0;
    vector<Motion_param> someRec;
    vector<Motion_param> someLogRec;  // log vector
    int Buffer_size = set.bufLen;
    int Buffer_count = 1;
    std::chrono::duration<double> time_span;
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    // Print information about starting settings:
    cout << "-----------------------------------------------" << endl;
    cout << std::dec;
    cout << "\tSending to: " << set.ip << ":" << set.port << endl;
    cout << "\tBuffer size: " << set.bufLen << endl;
    cout << "\tUDP buffer size: " << set.udpBufLen << endl;
    cout << "\tMeasurement waiting time: " << set.measWaitTime << endl;
    cout << "\tLogging: " << set.isLog << endl;
    cout << "\tCmd Print: " << set.isCmdPrint << endl;
    cout << "\tUDP Sending: " << set.isSend << endl;
    cout << "------------------------------------------------" << endl;

    try {
        Motion_param Mp;
        int sock = socket(AF_INET, SOCK_DGRAM, 0);

        // Asynchronouse console input thread:
        std::atomic<bool> run(true);
        std::thread cinThread(readCin, &run);  // std::ref(run)

        cout << "***** Type 'q' to quit the application *****" << endl;
        cout << "Server is running..." << endl;
        while (run.load()) {
            Mp = MPU9250_Loop();  // read motion data
            someRec.push_back(Mp);

            // Onply for debugn purposes:
            #ifdef DEBUG
                cout << Send_MsgStr(Mp) << endl;
            #endif

            // Save the data if log is enabled
            if (set.isLog)
                someLogRec.push_back(Mp);

            if (Buffer_count >= Buffer_size && set.isSend) {
                t1 = std::chrono::high_resolution_clock::now();
                SendUDP(sock, someRec, set);

                // Clear internal data vector:
                Buffer_count = 0;
                someRec.clear();

                // Time for sending over udp:
                t2 = std::chrono::high_resolution_clock::now();
                time_span = std::chrono::duration_cast
                            <std::chrono::duration<double>>(t2 - t1);

                // Console printing on/off
                if (set.isCmdPrint) {
                    cout << "Transfer in "
                    << time_span.count() * 1000 << " ms."
                    << endl;
                }
            }
            Record_count++;
            Buffer_count++;
        }
        run.store(false);
        cinThread.join();

        // Write notification to user:
        cout << "***** Sensor Server were stopped by the user. *****" << endl;

        // Logging the data to internal storage:
        if (set.isLog) {
            cout << "***** Write log data. *****" << endl;
            logData(someLogRec);
        }
    } catch (std::string &err) {
        cout << "Error: " << err << endl;
        return -1;
    } catch (std::exception &err) {
        cout << "Error: " << err.what() << endl;
        return -1;
    }

    cout << "***** Exit Programm. *****" << endl;
    return 0;
}
