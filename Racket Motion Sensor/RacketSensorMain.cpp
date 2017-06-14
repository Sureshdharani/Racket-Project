#include <stdio.h>
#include <chrono>
#include <math.h>
#include <mraa.h>
#include <string.h>
#include <time.h>
#include <array>


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
#define Mag_x_rawH_addr 0x03
#define Mag_x_rawL_addr 0x04
#define Mag_y_rawH_addr 0x05
#define Mag_y_rawL_addr 0x06
#define Mag_z_rawH_addr 0x07
#define Mag_z_rawL_addr 0x08

#define Buf_Length 100


struct Sensor_Meas_Raw_data
{
	int16_t Acc_x_Raw;
	int16_t Acc_y_Raw;
	int16_t Acc_z_Raw;
	int16_t Gyro_x_Raw;
	int16_t Gyro_y_Raw;
	int16_t Gyro_z_Raw;
	int16_t Mag_x_Raw;
	int16_t Mag_y_Raw;
	int16_t Mag_z_Raw;
	long Timestamp;
	
};

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
	struct Sensor_Meas_Raw_data Sen_meaurement;
	// mraa_i2c_context i2c = mraa_i2c_init(I2C_Bus_no); 
	// mraa_i2c_address(i2c, MPU_Addr);
	
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

void Print(mraa_i2c_context i2c)
{
	
	struct Sensor_Meas_Raw_data Buf_Sensor_data[Buf_Length];
	int Record_count = 0;
	int Buf_count=0;
	while(Record_count<10)
	{
		usleep(10000);
		if(Buf_count > 9)
		{	
			printf("Buff Count: -------------------------------------------------------%d\n",Buf_count);
			for(int i=0; i<=Buf_count; i++)
			{
				printf("ACC_X: %6d\n", Buf_Sensor_data[i].Acc_x_Raw);        
				printf("ACC_Y: %6d\n", Buf_Sensor_data[i].Acc_y_Raw);        
				printf("ACC_Z: %6d\n", Buf_Sensor_data[i].Acc_z_Raw);        
				printf("GYR_X: %6d\n", Buf_Sensor_data[i].Gyro_x_Raw);	
				printf("GYR_Y: %6d\n", Buf_Sensor_data[i].Gyro_y_Raw);        
				printf("GYR_Z: %6d\n", Buf_Sensor_data[i].Gyro_z_Raw);        
				printf("MAG_X: %6d\n", Buf_Sensor_data[i].Mag_x_Raw);        
				printf("MAG_Y: %6d\n", Buf_Sensor_data[i].Mag_y_Raw);        
				printf("MAG_Z: %6d\n", Buf_Sensor_data[i].Mag_z_Raw);       
				printf("Time Stamp: %lu\n",Buf_Sensor_data[i].Timestamp);
			}
			Buf_count=0;
			Buf_Sensor_data[Buf_count]=Get_RawData(i2c);
			Buf_count++;
			Record_count++;
		}
		else
		{
			printf("Buff Count: -------------------------------------------------------%d\n",Buf_count);
			Buf_Sensor_data[Buf_count]=Get_RawData(i2c);
			Buf_count++;
			Record_count++;
		}
	}
	// int Record_count=0;
		// while(Record_count<1000)
	 // {
			// struct Sensor_Meas_Raw_data Buf_Sensor_data;
			// Buf_Sensor_data=Get_RawData();
			// printf("ACC_X: %6d\n", Buf_Sensor_data.Acc_x_Raw);        
				// printf("ACC_Y: %6d\n", Buf_Sensor_data.Acc_y_Raw);        
				// printf("ACC_Z: %6d\n", Buf_Sensor_data.Acc_z_Raw);        
				// printf("GYR_X: %6d\n", Buf_Sensor_data.Gyro_x_Raw);
				// printf("GYR_Y: %6d\n", Buf_Sensor_data.Gyro_y_Raw);        
				// printf("GYR_Z: %6d\n", Buf_Sensor_data.Gyro_z_Raw);        
				// printf("MAG_X: %6d\n", Buf_Sensor_data.Mag_x_Raw);        
				// printf("MAG_Y: %6d\n", Buf_Sensor_data.Mag_y_Raw);        
				// printf("MAG_Z: %6d\n", Buf_Sensor_data.Mag_z_Raw);       
				// printf("Time Stamp: %ld\n",Buf_Sensor_data.Timestamp);
				// Record_count++;
	// }
        
	
}





int main( int argc , const char* argv[]){
	
	mraa_i2c_context i2c = MPU_Init();
	MPU_Sleep(Measur_wait_time_us);
	//struct Sensor_Meas_Raw_data Buf_Sensor_data[Buf_Length];
	Print(i2c);
	
	return 0;

	
	
}











