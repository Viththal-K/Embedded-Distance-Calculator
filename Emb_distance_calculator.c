#include "mbed.h"
#include "math.h"
#include "string.h"

void axes_values();

// setup the SPI
// PC1 - CS (chip select for gyro)
// PF7 - SCLK (Serial clock)
// PF8 - MISO (Master in Slave out)
// PF9 - MOSI (Master out Slave in)

SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk

DigitalOut gyro_cs(PC_1); // chip select

//global variables

int i = 1;
int vel_x[40];
int dx = 0;
int vx = 0;
int vel_y[40];
int dy = 0;
int vy = 0;
int vel_z[40];
int dz = 0;
int vz = 0;
int dis = 0;
int dis_final = 0;

int main() {

  // Chip must be deselected
  gyro_cs = 1;
 
  // Setup the SPI for 8 bit data and SPI mode is 3
  spi.format(8,3);
  // Set 8MHz clock rate
  spi.frequency(8000000);

  gyro_cs = 0;
  wait_us(5);

  // Send 0x8f, the command to read the WHOAMI register
  spi.write(0x8F);
 
  int whoami = spi.write(0x00);

  if(whoami == 0xD3) {
    printf("Welcome to Embedded Challenge \n");
  } else{
    printf("Error");
  }
  wait_us(5);
  gyro_cs = 1;

  // Initializing the gyroscope
  // Control register 1 (Addr: 0x20 / 32) set to 00001111 -> 0x0F / 15
  // Enabling all axes
    
  gyro_cs = 0;
  wait_us(5);

  spi.write(32);
  spi.write(15);
    
  wait_us(5);
  gyro_cs = 1;

  // Control register 4 (Addr: 0x23 / 35) set to 00010000 -> 0x10 / 16
  // Setting the sensitivity range to +/- 500 degrees per second
  
  gyro_cs = 0;
  wait_us(5);

  spi.write(35);
  spi.write(16);
    
  wait_us(5);
  gyro_cs = 1;


  while(i<=40){ // loop runs 40 times
    
    axes_values();

    i++;
    
    // Questions asks to sample at 0.5 second intervals
    // 0.5 sec = 500000 us
    wait_us(500000);
  }

  for (int i=1; i<=40; i++){
    
    // since we have taken both linear velocity in both negative and positive direction 
    // we take the average linear velocity and use abs function to obtain postive values 
    vx+=(abs(vel_x[i]))/2;
    vy+=(abs(vel_y[i]))/2;
    vz+=(abs(vel_z[i]))/2;

    // We know that, distance = velocity * time
    // time is 0.5 sec which is the time of every interval
    
    // total distance traveled in all three directions
    dx+=vx*0.5;
    dy+=vy*0.5;
    dz+=vz*0.5;
  }

  // taking average of the distances traveled in every direction
  dis = (dx + dy + dz) / 3;

  if (dis == 0){
    printf("Distance = 0 \n"); 
  } 
  
  else {

    // y = 0.0027x - 4.22 -> equation obtained using previous values
    dis_final = abs((0.0027 * dis) - 4.22);

    printf("Distance = %d \n", dis_final); 

  }
  
}
    


void axes_values(){
  
  gyro_cs = 0;
  wait_us(5);

  // Read 2 bytes of angular velocity data around the X axis (address code: 0xE8)

  spi.write(232);

  short x_axis_low = spi.write(0);
  short x_axis_high = spi.write(0);

  // Shifting the high bits by 8 positions and then "OR" them with the low bits
  short x_axis = x_axis_high << 8 | x_axis_low;
      
  wait_us(5);
  gyro_cs = 1;
  
  // Read 2 bytes of angular velocity data around the Y axis (address code: 0xEA)
  
  gyro_cs = 0;
  wait_us(5);
  
  spi.write(234);

  short y_axis_low = spi.write(0);
  short y_axis_high = spi.write(0);

  // Shifting the high bits by 8 positions and then "OR" them with the low bits
  short y_axis = y_axis_high << 8 | y_axis_low;
      
  wait_us(5);
  gyro_cs = 1;

  // Read 2 bytes of angular velocity data around the Z axis (address code: 0xEC)
  
  gyro_cs = 0;
  wait_us(5);

  spi.write(236);

  short z_axis_low = spi.write(0);
  short z_axis_high = spi.write(0);

  // Shifting the high bits by 8 positions and then "OR" them with the low bits
  short z_axis = z_axis_high << 8 | z_axis_low;
      
  wait_us(5);
  gyro_cs = 1;
      
  // 1 dps = 0.01745 rps

  float omega_x = x_axis * 0.01745;
  float omega_y = y_axis * 0.01745;
  float omega_z = z_axis * 0.01745;

  // linear velocity = omega * radius

  // radius -> 23 inch = 0.6 metre 
  // radius -> distance from waist to the point the microcontroller is placed on the leg

  float lin_vel_x = omega_x * 0.6;
  float lin_vel_y = omega_y * 0.6;
  float lin_vel_z = omega_z * 0.6; 
  
  // storing the linear velocity values into an array
  vel_x[i] = lin_vel_x;
  vel_y[i] = lin_vel_y;
  vel_z[i] = lin_vel_z;   
  
  // printing the linear velocity obtained in all 3 axes
  printf("Linear Velocity: x: %f, y: %f, z: %f \n", lin_vel_x, lin_vel_y, lin_vel_z);

}