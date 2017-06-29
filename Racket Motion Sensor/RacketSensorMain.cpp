
#include <stdio.h>
#include <chrono>
#include <math.h>
#include <mraa.h>
#include <string.h>
#include <time.h>
#include <array>
#include <vector>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define MPU_Addr 0x69
// i2c bus 1
#define I2C_Bus_no 1
#define Power_management 0x6B
#define Reset_bit 0x80
#define Unset_sleep_bit 0x00
#define Reset_wait_time_us 20000
#define Measur_wait_time_us 20000
#define Acc_x_rawH_addr 0x3B
#define Acc_x_rawL_addr 0x3C
#define Acc_y_rawH_addr 0x3D
#define Acc_y_rawL_addr 0x3E
#define Acc_z_rawH_addr 0x3F
#define Acc_z_rawL_addr 0x40
#define Gyro_x_rawH_addr 0x43
#define Gyro_x_rawL_addr 0x44
#define Gyro_y_rawH_addr 0x45
#define Gyro_y_rawL_addr 0x46
#define Gyro_z_rawH_addr 0x47
#define Gyro_z_rawL_addr 0x48
#define Mag_x_rawH_addr 0x4A
#define Mag_x_rawL_addr 0x4B
#define Mag_y_rawH_addr 0x4C
#define Mag_y_rawL_addr 0x4D
#define Mag_z_rawH_addr 0x4E
#define Mag_z_rawL_addr 0x4F
#define Mag_I2C_Address	0x0C

#define Buf_Length 100
#define UDP_Buf_Len 10000

struct Sensor_Meas_Raw_data
{
	int16_t Acc_x_Raw = 0;
	int16_t Acc_y_Raw = 0;
	int16_t Acc_z_Raw = 0;
	int16_t Gyro_x_Raw = 0;
	int16_t Gyro_y_Raw = 0;
	int16_t Gyro_z_Raw = 0;
	int16_t Mag_x_Raw = 0;
	int16_t Mag_y_Raw = 0;
	int16_t Mag_z_Raw = 0;
	long Timestamp = 0;
	
};


void write_setcompass(uint8_t addr, uint8_t data,mraa_i2c_context i2c)
{
	uint8_t rx_tx_buf[2];
	rx_tx_buf[0] = addr; 
	rx_tx_buf[1] = data;
	mraa_i2c_write(i2c, rx_tx_buf,2);	
	
}
void initializeCompass(){
	mraa_i2c_context i2c = mraa_i2c_init(I2C_Bus_no); 
	mraa_i2c_address(i2c, Mag_I2C_Address);

  write_setcompass(0x0A,0x00,i2c); //PowerDownMode
  write_setcompass(0x0A, 0x0F,i2c); //SelfTest
  write_setcompass(0x0A, 0x00,i2c); //PowerDownMode
  	mraa_i2c_context i2c1 = mraa_i2c_init(I2C_Bus_no); 
	
  mraa_i2c_address(i2c1, MPU_Addr);
  
  write_setcompass(0x24, 0x40,i2c1); //Wait for Data at Slave0
  write_setcompass(0x25, 0x8C,i2c1); //Set i2c address at slave0 at 0x0C
  write_setcompass(0x26, 0x02,i2c1); //Set where reading at slave 0 starts
  write_setcompass(0x27, 0x88,i2c1); //set offset at start reading and enable
  write_setcompass(0x28, 0x0C,i2c1); //set i2c address at slv1 at 0x0C
  write_setcompass(0x29, 0x0A,i2c1); //Set where reading at slave 1 starts
  write_setcompass(0x2A, 0x81,i2c1); //Enable at set length to 1
  write_setcompass(0x64, 0x01,i2c1); //overvride register
  write_setcompass(0x67, 0x03,i2c1); //set delay rate
  write_setcompass(0x01, 0x80,i2c1);
  write_setcompass(0x34, 0x04,i2c1); //set i2c slv4 delays
  write_setcompass(0x64, 0x00,i2c1); //override register
  write_setcompass(0x6A, 0x00,i2c1); //clear usr setting
  write_setcompass(0x64, 0x01,i2c1); //override register
  write_setcompass(0x6A, 0x20,i2c1); //enable master i2c mode
  write_setcompass(0x34, 0x13,i2c1); //disable slv4
}

using namespace std;
/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}
// Implementation of itoa()
char* itoa(unsigned long int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}
void initialize(){
   //write(MPU9150_PWR_MGMT_1, 0); //Wake up

   initializeCompass();
}

// MPU set to Idle mode
void MPU_Sleep(int wait_time_us)
{
	usleep(wait_time_us);
}

// initialise MPU 
mraa_i2c_context MPU_Init()
{
	mraa_i2c_context i2c = mraa_i2c_init(I2C_Bus_no); 
	mraa_i2c_address(i2c, MPU_Addr);
	uint8_t rx_tx_buf[2];
	// MPU reset
	rx_tx_buf[0] = Power_management; // power management 1
        rx_tx_buf[1] = Reset_bit; // set reset bit
        mraa_i2c_write(i2c, rx_tx_buf, 2);
        MPU_Sleep(Reset_wait_time_us);// wait for reset
	// MPU wakeup
	rx_tx_buf[0] = Power_management; // power management 1
        rx_tx_buf[1] = Unset_sleep_bit; // unset sleep bit
        mraa_i2c_write(i2c, rx_tx_buf, 2);
	return i2c;
	

}


  // Get Time stamp
static unsigned long Get_TimeSinceEpochMillis()
{
    return static_cast<unsigned long>
        (std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count());
}

  // Get Raw sensor data
Sensor_Meas_Raw_data Get_RawData(mraa_i2c_context i2c)
{

	Sensor_Meas_Raw_data Sen_meaurement;

	
	uint8_t ax_rawH = mraa_i2c_read_byte_data(i2c, Acc_x_rawH_addr);
    uint8_t ax_rawL = mraa_i2c_read_byte_data(i2c, Acc_x_rawL_addr);
	
	uint8_t ay_rawH = mraa_i2c_read_byte_data(i2c, Acc_y_rawH_addr);
    uint8_t ay_rawL = mraa_i2c_read_byte_data(i2c, Acc_y_rawL_addr);	
	
	uint8_t az_rawH = mraa_i2c_read_byte_data(i2c, Acc_z_rawH_addr);
    uint8_t az_rawL = mraa_i2c_read_byte_data(i2c, Acc_z_rawL_addr);
	
	uint8_t gx_rawH = mraa_i2c_read_byte_data(i2c, Gyro_x_rawH_addr);
    uint8_t gx_rawL = mraa_i2c_read_byte_data(i2c, Gyro_x_rawL_addr);
	
    uint8_t gy_rawH = mraa_i2c_read_byte_data(i2c, Gyro_y_rawH_addr);
    uint8_t gy_rawL = mraa_i2c_read_byte_data(i2c, Gyro_y_rawL_addr);
	
    uint8_t gz_rawH = mraa_i2c_read_byte_data(i2c, Gyro_z_rawH_addr);
    uint8_t gz_rawL = mraa_i2c_read_byte_data(i2c, Gyro_z_rawL_addr);
	
	uint8_t mx_rawH = mraa_i2c_read_byte_data(i2c, Mag_x_rawH_addr);
    uint8_t mx_rawL = mraa_i2c_read_byte_data(i2c, Mag_x_rawL_addr);
	
    uint8_t my_rawH = mraa_i2c_read_byte_data(i2c, Mag_y_rawH_addr);
    uint8_t my_rawL = mraa_i2c_read_byte_data(i2c, Mag_y_rawL_addr);
	
    uint8_t mz_rawH = mraa_i2c_read_byte_data(i2c, Mag_z_rawH_addr);
    uint8_t mz_rawL = mraa_i2c_read_byte_data(i2c, Mag_z_rawL_addr);
	
	
	int16_t Acc_x_Raw = (int16_t) ((ax_rawH<<8) + ax_rawL);
	int16_t Acc_y_Raw = (int16_t) ((ay_rawH<<8) + ay_rawL);
	int16_t Acc_z_Raw = (int16_t) ((az_rawH<<8) + az_rawL);
	 
	int16_t Gyro_x_Raw = (int16_t)((gx_rawH<<8) + gx_rawL);
	int16_t Gyro_y_Raw = (int16_t)((gy_rawH<<8) + gy_rawL);
	int16_t Gyro_z_Raw = (int16_t)((gz_rawH<<8) + gz_rawL);
	
	int16_t Mag_x_Raw = (int16_t) ((mx_rawH<<8) + mx_rawL);
	int16_t Mag_y_Raw = (int16_t) ((my_rawH<<8) + my_rawL);
	int16_t Mag_z_Raw = (int16_t) ((mz_rawH<<8) + mz_rawL);
	
	
	
	Sen_meaurement.Acc_x_Raw = Acc_x_Raw;// / 16384.0;
    Sen_meaurement.Acc_y_Raw= Acc_y_Raw;// / 16384.0;
    Sen_meaurement.Acc_z_Raw = Acc_z_Raw;// / 16384.0;

    Sen_meaurement.Gyro_x_Raw = Gyro_x_Raw ;
    Sen_meaurement.Gyro_y_Raw = Gyro_y_Raw ;
    Sen_meaurement.Gyro_z_Raw = Gyro_z_Raw ;
    
    Sen_meaurement.Mag_x_Raw = Mag_x_Raw ;
    Sen_meaurement.Mag_y_Raw = Mag_y_Raw ;
    Sen_meaurement.Mag_z_Raw = Mag_z_Raw ;
	Sen_meaurement.Timestamp = Get_TimeSinceEpochMillis();
	
	return Sen_meaurement;
}


string Make_Packet(Sensor_Meas_Raw_data Sens_data)
{
	
	char Timestamp[100];
	char Acc_x_Raw[100];
	char Acc_y_Raw[100];
	char Acc_z_Raw[100];
	char Gyro_x_Raw[100];
	char Gyro_y_Raw[100];
	char Gyro_z_Raw[100];
	char Mag_x_Raw[100];
	char Mag_y_Raw[100];
	char Mag_z_Raw[100];
	itoa (Sens_data.Timestamp,Timestamp,2);	
	itoa(Sens_data.Acc_x_Raw,Acc_x_Raw,2);
	itoa(Sens_data.Acc_y_Raw,Acc_y_Raw,2);
	itoa(Sens_data.Acc_z_Raw,Acc_z_Raw,2);
	itoa(Sens_data.Gyro_x_Raw,Gyro_x_Raw,2);
	itoa(Sens_data.Gyro_y_Raw,Gyro_y_Raw,2);
	itoa(Sens_data.Gyro_z_Raw,Gyro_z_Raw,2);
	itoa(Sens_data.Mag_x_Raw,Mag_x_Raw,2);
	itoa(Sens_data.Mag_y_Raw,Mag_y_Raw,2);
	itoa(Sens_data.Mag_z_Raw,Mag_z_Raw,2);
	
	string Send_Message;
	
	//Send_Message=Timestamp;
	 
Send_Message="s@"+to_string(Sens_data.Timestamp)+"@"+to_string(Sens_data.Acc_x_Raw)+"@"+to_string(Sens_data.Acc_y_Raw)+"@"+to_string(Sens_data.Acc_z_Raw)
					+"@"+to_string(Sens_data.Gyro_x_Raw)+"@"+to_string(Sens_data.Gyro_y_Raw)+"@"+to_string(Sens_data.Gyro_z_Raw)
					+"@"+to_string(Sens_data.Mag_x_Raw)+"@"+to_string(Sens_data.Mag_y_Raw)+"@"+to_string(Sens_data.Mag_z_Raw)+"@;";							   
	return Send_Message;
}
string Send_MsgStr(Sensor_Meas_Raw_data Sens_data)
{
string Send_Message;
	 
Send_Message="s@"+to_string(Sens_data.Timestamp)+"@"+to_string(Sens_data.Acc_x_Raw)+"@"+to_string(Sens_data.Acc_y_Raw)+"@"+to_string(Sens_data.Acc_z_Raw)
					+"@"+to_string(Sens_data.Gyro_x_Raw)+"@"+to_string(Sens_data.Gyro_y_Raw)+"@"+to_string(Sens_data.Gyro_z_Raw)
					+"@"+to_string(Sens_data.Mag_x_Raw)+"@"+to_string(Sens_data.Mag_y_Raw)+"@"+to_string(Sens_data.Mag_z_Raw)+"@;";							   
	return Send_Message;	
}

int Init_UDP()
{
	int sock,length, n;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[256];
	
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
	printf("please enter the message :");
	bzero(buffer,256);
	fgets(buffer,256,stdin);
	n=sendto(sock,buffer,256,0,(struct sockaddr *)&server,length);
	if (n<0)
	{
		printf("sendto");
	}
	// n=recvfrom(sock,buffer,256,0,&from,&length);
	// if(n<0)
	// {
		// error("revfr");
	// }
	write(1,"Got an ack: ",12);
	write (1,buffer,n);
	
	return 0;
}
void Print(mraa_i2c_context i2c)
{
	

	int Record_count = 0;
	std::vector<Sensor_Meas_Raw_data> someRec;
        int Buffer_size=Buf_Length;
        int Buffer_count=1;

        while(Record_count<1000)
	{
                Sensor_Meas_Raw_data Sensor_Meas_Raw_data1 = Get_RawData(i2c);

		someRec.push_back(Sensor_Meas_Raw_data1);


                if (Buffer_count>=Buffer_size )
			{
			int sock,length, n;
			struct sockaddr_in server, from;
			struct hostent *hp;
			char buffer[UDP_Buf_Len];
			
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
			//printf("please enter the message :");
			
                        std::cout << "-------------------------------- " <<Record_count+1<< std::endl;
                        for(unsigned int i =0; i < someRec.size(); i++)
							{
									bzero(buffer,UDP_Buf_Len);
									string msg=Send_MsgStr(someRec.at(i));
									//string msg="slkdjfskd";//Send_MsgStr(someRec.at(i));
									//buffer=(char *)alloca(s0.size() + 1);
									memcpy(buffer, msg.c_str(), msg.size() + 1);
									// printf("enter msg:");
									// fgets(buffer,256,stdin);
									n=sendto(sock,buffer,UDP_Buf_Len,0,(struct sockaddr *)&server,length);
									std::cout<<"Message"<<msg<<"\n"<<std::endl;
									if (n<0)
										{
											printf("sendto");
										}
							}
                        Buffer_count=0;
                        someRec.clear();
                }
                //std::cout << "---------------Recording----------------- " <<Buffer_count<< std::endl;
		Record_count++;
                Buffer_count++;
        }
	
}


int main( int argc , const char* argv[]){
	
	mraa_i2c_context i2c = MPU_Init();
        initialize();
	MPU_Sleep(Measur_wait_time_us);
	Print(i2c);	
        return 0;
}




