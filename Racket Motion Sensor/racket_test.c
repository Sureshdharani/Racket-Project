#include "mraa.h"
#include "math.h"
#include "sys/time.h"
static int array_MaxLength;
#define array_MaxLength 100
typedef struct Array_buffer{
    //enum { is_int, is_float, is_char } type;
    union {
        float measurements;
        long int TimeStamp;
    } val;
} ary_buffer;



int main (int argc, const char* argv[]) {
    // init i2c comms
    mraa_i2c_context i2c = mraa_i2c_init(1); // i2c bus 1
    mraa_i2c_address(i2c, 0x68);
    uint8_t rx_tx_buf[2];

    // reset
    rx_tx_buf[0] = 0x6B; // power management 1
    rx_tx_buf[1] = 0x80; // set reset bit
    mraa_i2c_write(i2c, rx_tx_buf, 2);
    usleep(200000); // wait for reset

    // wakeup
    rx_tx_buf[0] = 0x6B; // power management 1
    rx_tx_buf[1] = 0x00; // unset sleep bit
    mraa_i2c_write(i2c, rx_tx_buf, 2);

    float i=0;
    float conver=0;
    float comp=0;
ary_buffer A_buf[array_MaxLength];

int array_index=0;




struct timeval tp;

tp = (struct timeval){0};

/*gettimeofday(&tp, NULL);
long int ms = tp.tv_sec * 1000;// + tp.tv_usec / 1000;
printf("time: %d\n",ms);
*/


    while(i<100)
    {
    usleep(10000); // wait for new measurements
    printf("Counter: %f\n",i);
    // read raw accelerometer values
    uint8_t ax_rawH = mraa_i2c_read_byte_data(i2c, 0x3B);
    uint8_t ax_rawL = mraa_i2c_read_byte_data(i2c, 0x3C);
    uint8_t ay_rawH = mraa_i2c_read_byte_data(i2c, 0x3D);
    uint8_t ay_rawL = mraa_i2c_read_byte_data(i2c, 0x3E);
    uint8_t az_rawH = mraa_i2c_read_byte_data(i2c, 0x3F);
    uint8_t az_rawL = mraa_i2c_read_byte_data(i2c, 0x40);

 // read raw Gyro values
    uint8_t gx_rawH = mraa_i2c_read_byte_data(i2c, 0x43);
    uint8_t gx_rawL = mraa_i2c_read_byte_data(i2c, 0x44);
    uint8_t gy_rawH = mraa_i2c_read_byte_data(i2c, 0x45);
    uint8_t gy_rawL = mraa_i2c_read_byte_data(i2c, 0x46);
    uint8_t gz_rawH = mraa_i2c_read_byte_data(i2c, 0x47);
    uint8_t gz_rawL = mraa_i2c_read_byte_data(i2c, 0x48);

 // read raw magnetometer values
    uint8_t mx_rawH = mraa_i2c_read_byte_data(i2c, 0x03);
    uint8_t mx_rawL = mraa_i2c_read_byte_data(i2c, 0x04);
    uint8_t my_rawH = mraa_i2c_read_byte_data(i2c, 0x05);
    uint8_t my_rawL = mraa_i2c_read_byte_data(i2c, 0x06);
    uint8_t mz_rawH = mraa_i2c_read_byte_data(i2c, 0x07);
    uint8_t mz_rawL = mraa_i2c_read_byte_data(i2c, 0x08);




    // convert high and low bytes to signed 16-bit integers
    int16_t ax_raw = (int16_t) ((ax_rawH<<8) + ax_rawL);
    int16_t ay_raw = (int16_t) ((ay_rawH<<8) + ay_rawL);
    int16_t az_raw = (int16_t) ((az_rawH<<8) + az_rawL);

    // convert high and low bytes to signed 16-bit integers
    int16_t gx_raw = (int16_t) ((gx_rawH<<8) + gx_rawL);
    int16_t gy_raw = (int16_t) ((gy_rawH<<8) + gy_rawL);
    int16_t gz_raw = (int16_t) ((gz_rawH<<8) + gz_rawL);

    // convert high and low bytes to signed 16-bit integers
    int16_t mx_raw = (int16_t) ((mx_rawH<<8) + mx_rawL);
    int16_t my_raw = (int16_t) ((my_rawH<<8) + my_rawL);
    int16_t mz_raw = (int16_t) ((mz_rawH<<8) + mz_rawL);

    // convert to g
    float ax = ax_raw / 16384.0;
    float ay = ay_raw / 16384.0;
    float az = az_raw / 16384.0;

    float gx = gx_raw ;/// 16384.0;
    float gy = gy_raw ;/// 16384.0;
    float gz = gz_raw ;/// 16384.0;
    
    float mx = mx_raw ;/// 16384.0;
    float my = my_raw ;/// 16384.0;
    float mz = mz_raw ;/// 16384.0;

//    conver = ax*10.0f;
 //   conver = (conver > (floorf(conver)+0.5f)) ? ceilf(conver) : floorf(conver);
   // conver = conver/10.0f;

/*    if(i==0){

    comp=conver;
    printf("ACC_X: %f\n", ax);


    }
    else
    {
        if(comp!=conver)
        {
            comp=conver;
          printf("ACC_X: %f\n", ax);
        }
    
    }
   */ 



	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000+tp.tv_usec/1000;
	
    A_buf[array_index].val.measurements=ax;
    array_index++;
    A_buf[array_index].val.measurements=ay;
    array_index++;
    A_buf[array_index].val.measurements=az;
    array_index++;
    A_buf[array_index].val.measurements=gx;
    array_index++;
    A_buf[array_index].val.measurements=gy;
    array_index++;
    A_buf[array_index].val.measurements=gz;
    array_index++;
    A_buf[array_index].val.measurements=mx;
    array_index++;
    A_buf[array_index].val.measurements=my;
    array_index++;
    A_buf[array_index].val.measurements=mz;
    array_index++;
    A_buf[array_index].val.TimeStamp=ms;
    array_index++;

        if (array_index==array_MaxLength)
        {
            PrintAry(array_MaxLength,A_buf);
        }
    i++;

    }

    

    return 0;
}

void PrintAry(int ay_MaxLength, ary_buffer Array_buffer[] )
{
    int array_index= sizeof(Array_buffer);
    int i;
    for (i=0; i<ay_MaxLength;i++)
    {    printf("ACC_X: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("ACC_Y: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("ACC_Z: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("GYR_X: %f\n",Array_buffer[i].val.measurements);
        i++;
        printf("GYR_Y: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("GYR_Z: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("MAG_X: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("MAG_Y: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("MAG_Z: %f\n", Array_buffer[i].val.measurements);
        i++;
        printf("Time Stamp: %d\n",Array_buffer[i].val.TimeStamp);
        
    }   
};

