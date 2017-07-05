#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include "mraa.h"
#include <time.h>
#include <array>
#include <vector>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "I2Cdev_mraa.hpp"
#include "MPU9250_DMP.hpp"
#include "helper_3dmath.hpp"

#define QUATERNION
//#define EULER_ANGLES

#define Measur_wait_time_us 2000
#define Buf_Length 1000
#define UDP_Buf_Len 10000

using namespace std;
MPU9250 mpu;

//MRAA variables  
mraa_gpio_context S0;
mraa_gpio_context S1;
mraa_gpio_context S2;
mraa_gpio_context S3;
mraa_i2c_context MPU9250_i2c;

//MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


struct Motion_param
{
	float w = 0;
	float x = 0;
	float y = 0;
	float z = 0;
	long Timestamp = 0;
};

Motion_param MPU9250_Loop();
void MPU9250_GPIO_Init();


void MPU9250_GPIO_Init()
{
	S0 = mraa_gpio_init(32);
	S1 = mraa_gpio_init(33);
	S2 = mraa_gpio_init(46);
	S3 = mraa_gpio_init(47);

	
	mraa_gpio_dir(S0, MRAA_GPIO_OUT);
	mraa_gpio_dir(S1, MRAA_GPIO_OUT);
	mraa_gpio_dir(S2, MRAA_GPIO_OUT);
	mraa_gpio_dir(S3, MRAA_GPIO_OUT);
}
void MPU9250_MUX_Select()
{
	
		mraa_gpio_write(S0, 1);
	  	mraa_gpio_write(S1, 0);
		mraa_gpio_write(S2, 1);
		mraa_gpio_write(S3, 0);

}
// MPU set to Idle mode
void MPU_Sleep(int wait_time_us)
{
	usleep(wait_time_us);
}

 // Get Time stamp
static unsigned long Get_TimeSinceEpochMillis()
{
    return static_cast<unsigned long>
        (std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count());
}


Motion_param MPU9250_Loop()
{
	//If programming failed, don't try to do anything
    //if(!dmpReady) return;

    //Get current FIFO count
    fifoCount = mpu.getFIFOCount();
    //Wait for correct available data length, should be a VERY short wait
    while(fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    //Read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
	
	long Timestamp = Get_TimeSinceEpochMillis();
	Motion_param mp;
    #ifdef QUATERNION
    	//Display quaternion values in easy matrix form: w x y z
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        // std::cout << Ansi_Colour << "[ SEN" << i << " ]" << ANSI_COLOUR_RESET << ".."<<fifoCount;
        // std::cout << "\t";
        mp.w=q.w;	//std::cout << q.w;//std::cout << "\t";
		mp.x=q.x;	//std::cout << q.x;//std::cout << "\t";
        mp.y=q.y;	//std::cout << q.y ;//std::cout << "\t";
        mp.z=q.z;	//std::cout << q.z << "\n"<< endl;
		mp.Timestamp= Timestamp;
		return mp;

    #endif
    
    #ifdef EULER_ANGLES
        //Display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        //std::cout << "Euler Angles\t";
        // std::cout << Ansi_Colour << "[ SEN" << i << " ]" << ANSI_COLOUR_RESET;
        // std::cout << "\t";
        mp.x=(euler[0] * 180/M_PI);	//std::cout << (euler[0] * 180/M_PI) ;std::cout << "\t";
        mp.y=(euler[1] * 180/M_PI);//std::cout << (euler[1] * 180/M_PI);std::cout << "\t";
        mp.z=(euler[1] * 180/M_PI);//std::cout << (euler[2] * 180/M_PI) << "\n"<< endl;
		mp.Timestamp= Timestamp;
		return mp;
    #endif

}
void MPU9250_Setup()
{
	
		MPU9250 mpu;
    	printf("------------------------------------------------------------\n");
 		   
		//Select the Multiplexer Channel
		MPU9250_MUX_Select();
    
    	

		printf("Initializing I2C... \n");
		//Initialize the MPU
		mpu.initialize();
		//Report Connection
		mpu.testConnection() ? printf("MPU9250 connection successful \n") : printf("MPU9250 connection Failed\n");

		printf("Initializing DMP... \n");
		//Initialize the DMP
		devStatus = mpu.dmpInitialize();
    	std::cout << "DMP Status";
    	//Make sure it worked (returns 0 if so)
    	if (devStatus == 0) 
    	{
        	//Turn on the DMP, now that it's ready
        	printf("Enabling DMP... \n");
        	mpu.setDMPEnabled(true);

        	//Set our DMP Ready flag so the main loop() function knows it's okay to use it
        	printf("DMP Ready! \n");
        	dmpReady = true;

        	//Get expected DMP packet size for later comparison
        	packetSize = mpu.dmpGetFIFOPacketSize();
        
    	}

    	else 
    	{
        	printf("DMP Initialization failed");
        	std::cout << devStatus;
        	printf(")\n");
    	}
    	
    	printf(" Ready \n");
    
    	printf("------------------------------------------------------------\n");
	
	printf("\n\tInitialization Complete: All Systems are GO!!!\n\n");
}
string Send_MsgStr(Motion_param mp)
{
	string Send_Message;
	 #ifdef QUATERNION
		Send_Message="s@"+to_string(mp.Timestamp)+"@"+to_string(mp.w)+"@"+to_string(mp.x)+"@"+to_string(mp.y)+"@"+to_string(mp.z)+"@;";		
	#endif	
	#ifdef EULER_ANGLES
		Send_Message="s@"+to_string(mp.Timestamp)+"@"+to_string(mp.x)+"@"+to_string(mp.y)+"@"+to_string(mp.z)+"@;";		
	#endif		
	return Send_Message;	
}
void SendviaUDP ( vector<Motion_param> someRec)
{
	int sock,length, n;
			struct sockaddr_in server, from;
			struct hostent *hp;
			char buffer1[UDP_Buf_Len];
			
			sock = socket(AF_INET,SOCK_DGRAM,0);
			if(sock<0)
			{
				printf("socket");
			}
			server.sin_family=AF_INET;
			hp=gethostbyname("192.168.178.95");
			if (sock<0)
				printf("Unknown host");

			bcopy((char *)hp->h_addr, (char *)&server.sin_addr,hp->h_length);
			server.sin_port=htons(8888);
			length=sizeof(struct sockaddr_in);
			for(unsigned int i =0; i < someRec.size(); i++)
				{
						bzero(buffer1,UDP_Buf_Len);
						string msg=Send_MsgStr(someRec.at(i));
						memcpy(buffer1, msg.c_str(), msg.size() + 1);
						//n=0;
						n=sendto(sock,buffer1,UDP_Buf_Len,0,(struct sockaddr *)&server,length);
						std::cout<<"Message"<<msg<<"\n"<<std::endl;
						if (n<0)
							{
								printf("sendto");
							}
				}
	
}
int main( int argc , const char* argv[]){
	
	
	mraa_init();
	MPU9250_GPIO_Init();
	MPU_Sleep(Measur_wait_time_us);
	MPU9250_Setup();
	
	int Record_count = 0;
	std::vector<Motion_param> someRec;
	int Buffer_size=Buf_Length;
    int Buffer_count=1;
	while(Record_count<100000)
	{
		 
		Motion_param Mp= MPU9250_Loop();
		someRec.push_back(Mp);
		
		if (Buffer_count>=Buffer_size )
			{
				
				SendviaUDP(someRec);
				MPU9250_Setup();
				// MPU9250 mpu;
				// MPU9250_MUX_Select();
				// mpu.initialize();
				// if(mpu.testConnection()) { printf("MPU9250 connection successful \n");}else{ printf("MPU9250 connection Failed\n"); return 0;}
				devStatus = mpu.dmpInitialize();
				mpu.setDMPEnabled(true);
				dmpReady = true;
				packetSize = mpu.dmpGetFIFOPacketSize();
				Buffer_count=0;
                someRec.clear();	
				
			}
			Record_count++;
            Buffer_count++;
	}
		
        return 0;
}
