/* USER CODE BEGIN Header */
/**
  **********
  * @file           : main.c
  * @brief          : EE-18 Assignment
  * @Author         : Reuben Low Yu Xiang
  * @Email          : 2200931@sit.singaporetech.edu.sg
  **********
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define FALSE 0
#define TRUE 1
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Header files for checksum validation function
#include <stdbool.h> // required for using boolean data type
#include <stdint.h>  // required for using integer data types
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/*

*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

// Litekit
void moves(uint8_t* move);

//void turn45(uint8_t* move);
int movel(uint8_t* move, int duration);
uint8_t movex(uint8_t* move);

//printing
void myprint(char* str);
void intprint(uint16_t val);
void print_data(void* data, int data_type);
#define DATA_STRING 0
#define DATA_UINT16 1
#define DATA_UINT8 2


// Pixy Camera function declarations
int8_t getVersion();
int8_t getResolution();
int8_t setCameraBrightness(uint8_t brightness);
int8_t getBlocks(uint8_t sigmap, uint8_t maxBlocks);
//checksum
int8_t checksumcal(uint8_t* array);
void rspeed(void);
void bspeed(void);
void lspeed(void);
void fspeed(void);


// Pixy Camera  declarations
#define PIXY_DEFAULT_ARGVAL                  0x80000000
#define PIXY_BUFFERSIZE                      0x104
#define PIXY_CHECKSUM_SYNC                   0xc1af
#define PIXY_NO_CHECKSUM_SYNC                0xc1ae
#define PIXY_SEND_HEADER_SIZE                4
#define PIXY_MAX_PROGNAME                    33
#define PIXY_VERSION_BUFFERSIZE              22
#define PIXY_RESOLUTION_BUFFERSIZE           10

#define PIXY_TYPE_REQUEST_CHANGE_PROG        0x02
#define PIXY_TYPE_REQUEST_RESOLUTION         0x0c
#define PIXY_TYPE_RESPONSE_RESOLUTION        0x0d
#define PIXY_TYPE_REQUEST_VERSION            0x0e
#define PIXY_TYPE_RESPONSE_VERSION           0x0f
#define PIXY_TYPE_RESPONSE_RESULT            0x01
#define PIXY_TYPE_RESPONSE_ERROR             0x03
#define PIXY_TYPE_REQUEST_BRIGHTNESS         0x10
#define PIXY_TYPE_REQUEST_GETBLOCKS			 0x20
#define PIXY_TYPE_REQUEST_SERVO              0x12
#define PIXY_TYPE_REQUEST_LED                0x14
#define PIXY_TYPE_REQUEST_LAMP               0x16
#define PIXY_TYPE_REQUEST_FPS                0x18

#define PIXY_RESULT_OK                       0
#define PIXY_RESULT_ERROR                    -1
#define PIXY_RESULT_BUSY                     -2
#define PIXY_RESULT_CHECKSUM_ERROR           -3
#define PIXY_RESULT_TIMEOUT                  -4
#define PIXY_RESULT_BUTTON_OVERRIDE          -5
#define PIXY_RESULT_PROG_CHANGING            -6

// Pixy Camera getVersion declarations
//uint8_t RequestPacket[4] = {0xc1, 0xae, 0x0e, 0x00};
uint8_t reqV[] = {PIXY_NO_CHECKSUM_SYNC&0xff, PIXY_NO_CHECKSUM_SYNC>>8, PIXY_TYPE_REQUEST_VERSION, 0x00};
uint8_t recV[PIXY_VERSION_BUFFERSIZE];
int8_t checksumstateV = 0;
int8_t checksumstateR = 0;
int8_t checksumstateB = 0;
int8_t checksumstateG = 0;

// Pixy Camera getResolution declarations
uint16_t getWidth = 0, getHeight = 0;

//uint16_t 16BitChksum = 0;
// Order: 1st byte of no_checksum_sync, 2nd byte of no_checksum_sync, 3rd byte-version request type, 4th byte-data length 1, 5th byte unused
uint8_t reqR[] = {PIXY_NO_CHECKSUM_SYNC&0xff, PIXY_NO_CHECKSUM_SYNC>>8, PIXY_TYPE_REQUEST_RESOLUTION, 0x01, 0xFF};
uint8_t VerR[] = {0xAE, 0xc1, 0x0d, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t recR[10];

// Pixy Camera setCameraBrightness declarations
//uint8_t setbright = 0;	// value of brightness to be 0 to 255 //add static if required
// Order: Sync byte, sync byte, type of packet, length of payload, brightness
uint8_t reqB[] = {PIXY_NO_CHECKSUM_SYNC&0xff, PIXY_NO_CHECKSUM_SYNC>>8, PIXY_TYPE_REQUEST_BRIGHTNESS, 0x01, 0xFF};
uint8_t recB[10];

// Pixy Camera getBlocks declarations
uint8_t reqG[] = {PIXY_NO_CHECKSUM_SYNC&0xff, PIXY_NO_CHECKSUM_SYNC>>8, PIXY_TYPE_REQUEST_GETBLOCKS, 0x02, 0x01, 0x01};
uint8_t recG[20];
uint16_t X_BlkPix;  // 16-bit X (center) of block in pixels
uint16_t Y_BlkPix;  // 16-bit Y (center) of block in pixels
uint16_t WidthPix;  // 16-bit Width of block in pixels
uint16_t HeightPix; // 16-bit Height of block in pixels
uint16_t area;
uint16_t zval; //initial value
uint16_t xval; //initial value
int8_t xstate;// -1(left), 0(neutral) , 1 (right)
int8_t zstate;// -1(going further), 0(neutral) , 1 (going nearer)

// PIXYCAM checksum declarations
uint16_t checksum = 0;
uint16_t cal_checksum = 0;
bool validate_PIXYchecksum();

//motor
   const uint8_t dur = 0x64; //duration value
  //3byes for data for all movements: direction, duration, speed, checksum
  uint8_t fwd[] = {0x14, 0x01, 0x1E, 0x01}; //forward
  uint8_t bck[] = {0x14, 0x02, 0x1E, 0x01};//backward
  uint8_t left[] = {0x14, 0x03, 0x1E, 0x01};//turn left commented for now since not used
  uint8_t right[] = {0x14, 0x04, 0x1E, 0x01};//turn right
  uint8_t sleft[] = {0x14, 0x05, 0x1E, 0x01};//side left
  uint8_t sright[] = {0x14, 0x06,0x1E, 0x01};//side right
  uint8_t tleft[] = {0x14, 0x07, 0x1E, 0x01};//top left
  uint8_t tright[] = {0x14, 0x08, 0x1E, 0x01};// top right
  uint8_t bleft[] = {0x14, 0x09, 0x1E, 0x01};//bottom left
  uint8_t bright[] = {0x14, 0x0A, 0x1E, 0x01};//bottom right
  uint8_t stop[] = {0x14, 0x0B, 0x1E, 0x01};//stop
  uint8_t speed = 0; //will further be initalized in speed function
  const uint8_t xdiff = 20;
  uint8_t tmp = 0;				// for movement speed functions
  uint8_t review = 0;			// for movement speed functions
  const uint8_t cycle = 2;
  const uint8_t speedcap = 30;	// maximum speed cap that lite-kit will be moving
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/************************************************************************************
* PIXYCAM CHECKSUM VALIDATION FUNCTION DESCRIPTION:									*
* This function validates the checksum of the following functions:					*
* - getVersion()																	*
* - getResolution()																	*
* - setCameraBrightness(uint8_t brightness)											*
* - getBlocks()																		*
*																					*
* When the checksum of the four functions are validated to be true and correct,		*
* proceed with getBlocks() to retrieve information regarding the coloured object	*
*************************************************************************************/
  bool validate_PIXYchecksum(){
      // x axis in left and right sideways, Z axis is away and near, with reference to PixyCam getBlocks()
      while (1){
          checksumstateV = getVersion();
          HAL_Delay(1);
          checksumstateR = getResolution();
          HAL_Delay(1);
          checksumstateB = setCameraBrightness(100);
          HAL_Delay(1);
          HAL_Delay(100);
          checksumstateG = getBlocks(1, 1);
          HAL_Delay(1);
          xval = X_BlkPix; // store initial state of x axis
          zval = area;     // store initial state of z axis
          if(checksumstateV != 0){
        	  myprint("getVersion function checksum VALIDATION FAILED\n");
          }
          if(checksumstateR != 0){
        	  myprint("getResolution function checksum VALIDATION FAILED\n");
          }
          if(checksumstateB != 0){
        	  myprint("setCameraBrightness function checksum VALIDATION FAILED\n");
          }
          if(checksumstateG != 0){
        	  myprint("getBlocks function checksum VALIDATION FAILED\n");
          }
          if (checksumstateV == 0 && checksumstateR == 0 && checksumstateB == 0 && checksumstateG == 0){
              return true;
          }
      }
  }


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // Validate PixyCam checksum, if true proceed to main loop
  if(validate_PIXYchecksum()){

	  while(1)//infinite main loop
	  {

		getBlocks(1,1);

		// Wait till a valid area of the object is detected
		while(!area)
		{
			// Get current values of position of coloured object
			getBlocks(1,1);

		}
		uint16_t zbdiff =(zval/100*5);
		uint16_t zdiff =(zval/100*20);//5% buffer
		xstate = X_BlkPix>(xval+xdiff)?1:X_BlkPix<(xval-xdiff)?-1:0;//store xval state, buffer of 5 units
		zstate = area>(zval+(zbdiff))?1:(area<(zval-(zdiff))?-1:0);//store zval state

		/*******************************************************************************
		 * 	Variables review and tmp is to ensure smooth transition when algorithm is  *
		 *	switching between position states of the coloured object by ensuring       *
		 *	speed is set to zero before executing the respective movement commands     *
		 *******************************************************************************/
		// When the coloured object is moving away from the front of lite-kit
		if(zstate == -1 && xstate == 0) //x going left and z in bigger
		{
			review = 1;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("forward\n");
			fspeed(); // set speed
			movel(fwd, cycle);//send direction and for 10 times
		}

		// When the coloured object is moving right and away from the front of lite-kit
		else if(zstate == -1 && xstate == 1)
		{
			review = 2;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("top right\n");
			rspeed();// set speed
			movel(tright, cycle);//send direction and for 10 times
		}
		// When the coloured object is moving towards the front of lite-kit
		else if(zstate == 1 && xstate == 0)
		{
			review = 3;
	//		speed=review!=tmp?0:speed;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("backwards\n");
			bspeed();// set speed
			movel(bck, cycle);//send direction and for 10 times
		}
		// When the coloured object is moving left and away from the front of lite-kit
		else if(zstate == -1 && xstate ==-1)
		{
			review = 4;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("top left\n");
			lspeed();// set speed
			movel(tleft, cycle);//send direction and for 10 times
		}
		// When the coloured object is moving left and towards the front of lite-kit
		else if(zstate == 1 && xstate ==-1)
		{
			review = 5;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("bottom left\n");
			lspeed();// set speed
			movel(bleft, cycle);//send direction and for 10 times
		}
		// When the coloured object is moving right and towards the front of lite-kit
		else if(zstate == 1 && xstate == 1)
		{
			review = 6;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("bottom right\n");
			rspeed();// set speed
			movel(bright, cycle);//send direction and for 10 times
		}
		// When the coloured object is moving right sideways
		else if(zstate == 0 && xstate == 1)
		{
			review = 7;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("side right\n");
			rspeed();// set speed
			movel(sright, cycle);//send direction and for 10 times
	//		movex(sright);
		}
		// When the coloured object is moving left sideways
		else if(zstate == 0 && xstate == -1)
		{
			review = 8;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("side left\n");
			lspeed();// set speed
			movel(sleft, cycle);//send direction and for 10 times
		}
		// When the coloured object stops moving, lite-kit stops
		else
		{
			review = 9;
			if(review!=tmp)
			{
				do
				{
					speed--;
				}while(speed>0);
			}
			tmp = review;
			myprint("stop\n");
			movel(stop, cycle);//send direction and for 10 times
			// no speed required
		}
		HAL_Delay(5);
	  }

	}
}
/* USER CODE END WHILE */


/* USER CODE BEGIN 3 */

void moves(uint8_t* move)//moves is move stop, used for stop
{
	for(int i = 0; i<50; i++)
	{
		HAL_Delay(20);
		HAL_UART_Transmit(&huart1, move, sizeof(move), 100);
	}
}
int movel(uint8_t* move, int duration)//movel is move long, used for all other movements
{
	move[2] = speed; //store speed
	move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
	for(int i = 0; i<duration;)//loop for
	{
		HAL_Delay(1);
		uint8_t state = 0x0E; //initial state is notok aka ntok
		HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
		HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
		if(state == 0x0F)//check state so that transmission only occurs when no obstacle
		{
			i++; //increment only if transmitted
		}
	}
	return 1;
}
uint8_t tspeed = 0;
uint8_t movex(uint8_t* move)//movel is move long, used for all other movements
{
	while(1)
	{
		if(xval>X_BlkPix)
		{
			tspeed++;
		}
		else if (xval<X_BlkPix)
		{
			tspeed--;
		}
		tspeed = tspeed>63?63:tspeed;//cap speed at 30
		move[2] = tspeed; //store speed
		move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
		HAL_Delay(20);
		uint8_t state = 0x0E; //initial state is notok aka ntok
		HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
		HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
		if(tspeed == 0)
		{
			break;
		}
	}
	tspeed = 0;
	return 1;
}

void rspeed(void)//right speed calculation
{
	//can be multipled with other values to get different value for testing
	//speed = 10;//(((X_BlkPix - xval)/10)*10 - xdiff);//calculate change for acceleration
	if(X_BlkPix>xval)
	{
		speed+=2;
	}
	else// if(X_BlkPix<xval)
	{
		speed-=2;
	}
	speed = speed>speedcap?speedcap:speed;//cap speed at 30
	//should implement if case for speed to = 0;
}
void lspeed(void)//left speed calculation
{
//	speed = 10;//(((xval - X_BlkPix)/10)*10-xdiff);//calculate change for acceleration
	if(X_BlkPix<xval)
	{
		speed+=2;
	}
	else// if(X_BlkPix>xval)
	{
		speed-=2;
	}
	speed = speed>speedcap?speedcap:speed;//cap speed at 30
}
void fspeed(void)//front speed calculation
{
//	speed = 10;//((zval - area)/area) * 20;//calculate change for acceleration
	if(area<zval)
	{
		speed+=2;
	}
	else// if(area>zval)
	{
		speed-=2;
	}
	speed = speed>speedcap?speedcap:speed;//cap speed at 30
}
void bspeed(void)//Back speed calculation
{
	if(area>zval)
	{
		speed+=2;
	}
	else// if(area<zval)
	{
		speed-=2;
	}
//	speed = 10;//((area - zval)/zval) * 20;//calculate change for acceleration
	speed = speed>speedcap?speedcap:speed;//cap speed at 30
}

/*******************************************************************************
 * 	Alternative: Tracking colored object									   *
 * 	Code snippet below														   *
 *******************************************************************************/

/*******************************************************************************
 * 	PRINT CHAR FUNCTION DESCRIPTION:									   	   *
 * 	Prints a string of maximum 50 characters								   *
 *******************************************************************************/
void myprint(char *str)//print a sting of max 50 char
{
	char print[50] = {'\0'}; // buffer
	sprintf(print, "%s", str);
	HAL_UART_Transmit(&huart2, (uint8_t *)print, strlen(print), 100);

}
/*******************************************************************************
 * 	PRINT unsigned int FUNCTION DESCRIPTION:								   *
 * 	Prints a 2 byte unsigned integer								   		   *
 *******************************************************************************/
//print a 2byte int
void intprint(uint16_t val)//print an unsigned int
{
	char print[50] = {'\0'}; // buffer
	sprintf(print, "%u", val);
	HAL_UART_Transmit(&huart2, (uint8_t *)print, strlen(print), 100);
}

/*******************************************************************************
 * 	PRINT unsigned int and char FUNCTION DESCRIPTION:						   *
 * 	Print char and uint16_t												       *
 * 	DIS BE COOL LIKE KANYE ALL OF THE LIGHTS ALL OF THE LIGHTS WHOOP WHOOP	   *
 *******************************************************************************/
void print_KANYEdata(void* data, int data_type){
    char print[50] = {'\0'};
    if (data_type == DATA_STRING){
        sprintf(print, "%s", (char*)data);
    }else if (data_type == DATA_UINT16){
        sprintf(print, "%u", *((uint16_t*)data));
    }else if (data_type == DATA_UINT8){
    	sprintf(print, "%u", *((uint8_t*)data));
    }
    HAL_UART_Transmit(&huart2, (uint8_t *)print, strlen(print), 100);
}


// PIXY CAMERA

// Order: 1st byte of no_checksum_sync, 2nd byte of no_checksum_sync, 3rd byte-version request type, 4th byte-data length 0
// To request hardware and firmware version data

/*******************************************************************************
 * 	checksumcal(uint8_t* array) DESCRIPTION:						           *
 * 	Calculates the checksum for getVersion() and getResolution()               *
 * 	Function parameter array is the array of response bytes sent from PixyCam  *
 *******************************************************************************/
int8_t checksumcal(uint8_t* array)//calculate checksum of pixycam
{
	checksum = array[5] << 8 | array[4];
	cal_checksum = 0;
	for(int i = 6; i<22;i++)
	{
		cal_checksum+=array[i];
	}
	print_KANYEdata("PixyCam Checksum: ", DATA_STRING);
	print_KANYEdata(&checksum, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Checksum calculation of Payload Bytes: ", DATA_STRING);
	print_KANYEdata(&cal_checksum, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Can we get much higher?\n", DATA_STRING);
	if(array == recV){
		if(checksum == cal_checksum){
			print_KANYEdata("SO HIGH.... OH OH OH\n", DATA_STRING);
			print_KANYEdata("getVersion PixyCam Checksum Validation SUCCESS\n", DATA_STRING);
		}else{
			print_KANYEdata("getVersion PixyCam Checksum Validation FAILED\nTRY AGAIN\n", DATA_STRING);
		}
	}else if(array == recR){
		if(checksum == cal_checksum){
			print_KANYEdata("SO HIGH.... OH OH OH\n", DATA_STRING);
			print_KANYEdata("getResolution PixyCam Checksum Validation SUCCESS\n", DATA_STRING);
		}else{
			print_KANYEdata("getResolution PixyCam Checksum Validation FAILED\nTRY AGAIN\n", DATA_STRING);
		}
	}

	return checksum==cal_checksum?PIXY_RESULT_OK:PIXY_RESULT_ERROR;
}

/*******************************************************************************
 * 	getVersion() DESCRIPTION:						           				   *
 * 	getVersion() queries and receives the firmware and hardware version of 	   *
 * 	PixyCam																	   *
 * 	- Hardware Version                                     					   *
 * 	- Firmware version (major)												   *
 * 	- Firmware version (minor)												   *
 * 	- Firmware Build             											   *
 * 	- Firmware Type 														   *
 * 	- PixyCam Checksum														   *
 *******************************************************************************/
int8_t getVersion()//get the current version of  Pixy cam
{
	uint16_t hardwareVer = 0;
	uint16_t firmwareVerMajor = 0;
	uint16_t firmwareVerMinor = 0;
	uint16_t firmwareBuild = 0;
	uint16_t firmwareType = 0;

	HAL_UART_Transmit(&huart3, reqV, 4, 100);
	HAL_UART_Receive(&huart3, recV, PIXY_VERSION_BUFFERSIZE, 100);
	hardwareVer = recV[7] << 8 | recV[6];
	firmwareVerMajor = recV[8];
	firmwareVerMinor = recV[9];
	firmwareBuild = recV[11] << 8 | recV[10];

	print_KANYEdata("Hardware Version: ", DATA_STRING);
	print_KANYEdata(&hardwareVer, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Firmware version major: ", DATA_STRING);
	print_KANYEdata(&firmwareVerMajor, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Firmware version minor: ", DATA_STRING);
	print_KANYEdata(&firmwareVerMinor, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Firmware Build: ", DATA_STRING);
	print_KANYEdata(&firmwareBuild, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Firmware Type: ", DATA_STRING);
	print_KANYEdata(&firmwareType, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	// Checksum sent from PixyCam and Checksum validation result will be printed from
	// checksumcal and validate_PIXYchecksum() function

	HAL_Delay(1);
	return checksumcal(recV);
	//if loop to check error;
}

/*******************************************************************************
 * 	getResolution() DESCRIPTION:						           			   *
 * 	getResolution() gets the width and height of the frames used by the current*
 * 	program. After calling this function, the width and height can be found in *
 * 	the frameWidth and frameHeight member variables							   *								   *
 * 	- Frame Width                                     					       *
 * 	- Frame Height															   *
 * 	- PixyCam Checksum														   *
 *******************************************************************************/
// getResolution gets the width and height of the frames used by the current program
int8_t getResolution()//get the resolution of  Pixy cam
{
	uint16_t ResWidth = 0;
	uint16_t ResHeight = 0;
	HAL_UART_Transmit(&huart3, reqR, 5, 100);
	HAL_UART_Receive(&huart3, recR, 10, 100);

	ResWidth = recR[7] << 8 | recR[6];
	ResHeight = recR[9] << 8 | recR[8];

	print_KANYEdata("PixyCam Resolution Width: ", DATA_STRING);
	print_KANYEdata(&ResWidth, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("PixyCam Resolution Height: ", DATA_STRING);
	print_KANYEdata(&ResHeight, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	// Checksum sent from PixyCam and Checksum validation result will be printed from
	// checksumcal and validate_PIXYchecksum() function

	HAL_Delay(1);
	return checksumcal(recR);
}

/*******************************************************************************
 * 	setCameraBrightness(uint8_t setbright) DESCRIPTION:						   *
 * 	setCameraBrightness() sets the relative exposure level of Pixy2's image    *
 * 	sensor. Higher values result in a brighter (more exposed) image.		   *
 * 	Parameter setbright is the brightness value that will be set			   *
 *******************************************************************************/
// setCameraBrightness
// Taken from Pixy camera docu: sets the relative exposure level of Pixy2's image sensor. Higher values result in a brighter (more exposed) image.
// Verify brightness response packet
// Order: 8 bit sync byte, 8 bit sync byte, types of packet, length of payload, 4-5 16 bit checksum, 6-9 32 bit brightness
uint8_t VerBright[] = {0xC1, 0xAF, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
int8_t setCameraBrightness(uint8_t setbright)//set the current brightness of  Pixy cam, set to 100 for testing
{
	print_KANYEdata("Brightness set to: ", DATA_STRING);
	print_KANYEdata(&setbright, DATA_UINT8);
	print_KANYEdata("\n", DATA_STRING);

	reqB[4] = setbright;//store brightness value
	HAL_UART_Transmit(&huart3, reqB, 5, 100);
	HAL_UART_Receive(&huart3, recB, 10, 100);
	HAL_Delay(1);
	checksum = recB[5] << 8 | recB[4];
	cal_checksum = 0;
	for(int i = 6; i<sizeof(recB);i++)
	{
		cal_checksum+=recB[i];
	}
	// Checksum sent from PixyCam and Checksum validation result will be printed from
	// checksumcal and validate_PIXYchecksum() function
	return checksum==cal_checksum?PIXY_RESULT_OK:PIXY_RESULT_ERROR;
}

// if receive center of object that litekit is tracking, how to use coordinates to classify movements slight left?
// diagonally left? coordinates and size must take this into account, use get blocks, set threshold when setting motion
// based on height and width of object
// if moves further away, center of object moves to the right andf getting smaller, motion cannot be just slightly right
// must be diagonally right
// devise this into flowchart

// getBlocks, gets all the detected blocks in the most recent frame
// Description taken from pixy cam documentation
// the returned blocks are sorted by area, with the largest blocks appearing first in the blocks array.
// returns an error value (<0) if it fails and the number of detected blocks (>=0) if it succeeds.
// sigmap is a bitmap of all 7 signatures from which you wish to receive block data.
// if you are only interested in block data from signature 1, you would pass in a value of 1.
// If you are interested in block data from both signatures 1 and 2, you would pass in a value of 3.
// If you are interested in block data from signatures 1, 2, and 3, you would pass a value of 7, and so on.
// The most-significant-bit (128 or 0x80) is used for color-codes.
// A value of 255 (default) indicates that you are interested in all block data.

// maxblocks is the maximum number of blocks you want to receive.
// passing 1 will receive one block, passing 255 will receive all blocks.

//uint8_t sigmap = 0xFF;//for sigmap value for 1
//uint8_t maxBlocks = 0x01;
//uint8_t VerBlock[] = {0xC1, 0xAF, 0x21, 0x0E, 0x00, 0x00, // last two bytes are sum of payload bytes
//					  0x00, 0x00, 0x00, 0x00, // 16-bit signature, 16-bit X (center) of block in pixels
//					  0x00, 0x00, 0x00, 0x00, // 16-bit Y (center) of block in pixels, 16-bit Width of block in pixels
//					  0x00, 0x00, 0x00, 0x00, // 16-bit Height of block in pixels, 16-bit Angle of color-code in degrees
//					  0x00, 0x00			  // Tracking index of block (see API for more info), Age - number of frames this block has been tracked
//					  };
//

//
//uint16_t signature = 0; // colour code number
//uint16_t X_BlkPix = 0;  // 16-bit X (center) of block in pixels
//uint16_t Y_BlkPix = 0;  // 16-bit Y (center) of block in pixels
//uint16_t WidthPix = 0;  // 16-bit Width of block in pixels
//uint16_t HeightPix = 0; // 16-bit Height of block in pixels
//int16_t AngleCode = 0; // 16-bit Angle of color-code in degrees -180 - 180 (0 if not a color code)
//uint8_t TrackIdx = 0;   // Tracking index of block (see API for more info)
//uint8_t Age = 0;	    // Age - number of frames this block has been tracked
//int16_t getPayloadSum = 0;

/*******************************************************************************
 * 	getBlocks() retrieves detected blocks in the most recent frame. The new	   *
 * 	data is then available in the blocks member variable.				       *
 *																			   *
 * 	Parameter sigmap is the selected bitmap of 7 signatures or object profile  *
 * 	that are defined and set in the PixyMon application.					   *
 *																			   *
 * 	Parameter maxBlocks is the maximum number of blocks the PixyCam will detect*
 * 	As the lite-kit will be tracking and following only one coloured object,   *
 * 	maxBlocks is assigned to a value of 1.									   *
 *																			   *
 * 	The getBlocks function will retrieve the following data from PixyCam:	   *
 * 	- 16-bit PixyCam checksum												   *
 * 	- 16-bit signature / Color code number									   *
 * 	- 16-bit X (center) of block in pixels								       *
 * 	- 16-bit Y (center) of block in pixels									   *
 * 	- 16-bit Width of block in pixels										   *
 * 	- 16-bit Height of block in pixels										   *
 * 	- 16-bit Angle of color-code in degrees									   *
 * 	- Tracking index of block												   *
 * 	- Age - number of frames this block has been tracked					   *
 *******************************************************************************/
int8_t getBlocks(uint8_t sigmap, uint8_t maxBlocks)//getblocks function to get X,Y coordinates and Calculate Area
{
	print_KANYEdata("Signature Map selected: ", DATA_STRING);
	print_KANYEdata(&sigmap, DATA_UINT8);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Maximum number of blocks to be detected: ", DATA_STRING);
	print_KANYEdata(&maxBlocks, DATA_UINT8);
	print_KANYEdata("\n", DATA_STRING);

	HAL_UART_Transmit(&huart3, reqG, 6, 100);
	HAL_UART_Receive(&huart3, recG, 20, 100);
	uint16_t sig_colour = ((uint16_t)recG[7] << 8)| recG[6];
	X_BlkPix = ((uint16_t)recG[9] << 8)| recG[8];//Bitshift to find x Val
	Y_BlkPix = ((uint16_t)recG[11] << 8)| recG[10];//Bitshift to find Y Val
	WidthPix = ((uint16_t)recG[13] << 8)| recG[12];//Bitshift to find Width
	HeightPix = ((uint16_t)recG[15] << 8)| recG[14];//Bitshift to find Height

	uint16_t color_deg = ((uint16_t)recG[17] << 8)| recG[16];
	uint8_t track_idx = recG[18];
	uint8_t blk_age = recG[19];

	area = WidthPix*HeightPix;// Calculate and store area
	checksum = recG[5] << 8 | recG[4];
	cal_checksum = 0;
	for(int i = 6; i<sizeof(recG);i++)
	{
		cal_checksum+=recG[i];
	}

	print_KANYEdata("PixyCam Checksum: ", DATA_STRING);
	print_KANYEdata(&checksum, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Signature Color code number: ", DATA_STRING);
	print_KANYEdata(&sig_colour, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("X center of block in pixels: ", DATA_STRING);
	print_KANYEdata(&X_BlkPix, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Y center of block in pixels: ", DATA_STRING);
	print_KANYEdata(&Y_BlkPix, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Width of block in pixels: ", DATA_STRING);
	print_KANYEdata(&WidthPix, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Height of block in pixels: ", DATA_STRING);
	print_KANYEdata(&HeightPix, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Area of detected object: ", DATA_STRING);
	print_KANYEdata(&area, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Angle of color code in degrees: ", DATA_STRING);
	print_KANYEdata(&color_deg, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Tracking index of block: ", DATA_STRING);
	print_KANYEdata(&track_idx, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	print_KANYEdata("Age - number of frames this block has been tracked: ", DATA_STRING);
	print_KANYEdata(&blk_age, DATA_UINT16);
	print_KANYEdata("\n", DATA_STRING);

	if(checksum == cal_checksum){
		print_KANYEdata("getBlocks function checksum VALIDATION SUCCESS\n", DATA_STRING);
	}else{
		print_KANYEdata("getBlocks function checksum VALIDATION FAILED\n", DATA_STRING);
	}

	return checksum==cal_checksum?PIXY_RESULT_OK:PIXY_RESULT_ERROR;

}

  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
	Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
	Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
	Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
	Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/////////
/*******************************************************************************
 * 	Alternative Version: Tracking colored object							   *
 * 	In progress																   *
 *******************************************************************************/
// buffer for x and y coordinates in pixels
//	#define x_buffer 30
//	// buffer for area in percentage
//	#define area_buffer 60
//	  while(1)//infinite main loop
//	  {
//
//		getBlocks(1,1);
//		while(!area) //till area value is valid
//		{
//			getBlocks(1,1);// current value of positon
//
//		}
//		uint16_t diff = ((zval/100)*area_buffer);
//		xstate = X_BlkPix>(xval+x_buffer)?1:X_BlkPix<(xval-x_buffer)?-1:0;//store xval state, buffer of 5 units
//		zstate = area>(zval+diff)?1:(area<(zval-diff)?-1:0);//store zval state
//
//
//		if(zstate == -1 && xstate == 0) //x going left and z in bigger
//		{
//			review = 1;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("forward\n");
//			fspeed(fwd); // set speed
////			movel(fwd, 10);//send direction and for 10 times
//		}
//		else if(zstate == -1 && xstate == 1)
//		{
//			review = 2;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("top right\n");
//			rspeed(tright);// set speed
////			trspeed();
////			movel(tright, 10);//send direction and for 10 times
//		}
//		else if(zstate == 1 && xstate == 0)
//		{
//			review = 3;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("backwards\n");
//			bspeed(bck);// set speed
////			movel(bck, 10);//send direction and for 10 times
//		}
//		else if(zstate == -1 && xstate ==-1)
//		{
//			review = 4;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("top left\n");
//			lspeed(sleft);// set speed
////			tlspeed();
////			movel(tleft, 10);//send direction and for 10 times
//		}
//		else if(zstate == 1 && xstate ==-1)
//		{
//			review = 5;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("bottom left\n");
//			lspeed(sleft);// set speed
////			blspeed();
////			movel(bleft, 10);//send direction and for 10 times
//		}
//		else if(zstate == 1 && xstate == 1)
//		{
//			review = 6;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("bottom right\n");
//			rspeed(bright);// set speed
////			brspeed();
////			movel(bright, 10);//send direction and for 10 times
//		}
//		else if(zstate == 0 && xstate == 1)
//		{
//			review = 7;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("side right\n");
//			rspeed(sright);// set speed
////			movel(sright, 10);//send direction and for 10 times
//		}
//		else if(zstate == 0 && xstate == -1)
//		{
//			review = 8;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("side left\n");
//			lspeed(sleft);// set speed
////			movel(sleft, 10);//send direction and for 10 times
//		}
//		else if(zstate == 0 && xstate == 0)
//		{
//			review = 9;
//			if(review!=tmp)
//			{
//				speed = 0;
//			}
//			tmp = review;
//			myprint("stop\n");
//			movel(stop, 10);//send direction and for 10 times
//			// no speed required
//		}
//		HAL_Delay(1);
//	  }
//
//	}
//	/* USER CODE END WHILE */

//	// value for speed increments and decrements
//	#define base_spd 1
//
//	// side_offset is the difference between the x coordinate reading from getBlocks() and initialised x coordinate
//	uint16_t side_offset;
//
//	// area_offset is the difference between the area reading and the initialised area
//	uint16_t area_offset;
//
//	// offset buffer if needed
//	//#define offset_buffer 0
//	uint16_t offset_buffer = 0;
//
//	// area percentage
//	#define area_divider 20
//
//	// area buffer
//	uint16_t area_buff = 60;
//
//	// max speed
//	#define spd_cap 40
//
//	// max speed for fwd and bck
//	#define line_cap 35
//
//	// for loop increments
//	uint16_t scoop;
//
//	// refer to x_buffer for buffer for x coordinates
//
//	void rspeed(uint8_t* move)//right speed calculation
//	{
//		//can be multipled with other values to get different value for testing
//		//speed = ((X_BlkPix - xval)/10)*10;//calculate change for acceleration
//		side_offset = X_BlkPix - xval + offset_buffer;
//		scoop = side_offset ? 10 : side_offset+5 ? 14 : side_offset+10 ? 18 : side_offset+15 ? 22 : side_offset+20 ? 26 : side_offset+25 ? 30 : scoop;
//		for(uint16_t i = 0; i < scoop; i++){
//			//speed += base_spd;
//			if(X_BlkPix > (xval+x_buffer)){
//				speed += base_spd;
//			}else if(X_BlkPix < (xval+x_buffer)){
//				speed -= base_spd;
//			}
////			else if(X_BlkPix == (xval+x_buffer)){
////				speed = 0;
////			}
//			move[2] = speed; //store speed
//			move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
//			uint8_t state = 0x0E; //initial state is notok aka ntok
//			HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
//			HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
//			HAL_Delay(300);
//		}
//		speed = speed>spd_cap?spd_cap:speed;//cap speed at 30
//	}
//	void lspeed(uint8_t* move)//left speed calculation
//	{
////		speed = ((xval - X_BlkPix)/10)*10;//calculate change for acceleration
////		side_offset = xval - X_BlkPix + offset_buffer;
////		for(uint16_t i = 0; i < side_offset; i++){
////			//speed += base_spd;
////			if(X_BlkPix < (xval+x_buffer)){
////				speed += base_spd;
////			}else if(X_BlkPix > (xval+x_buffer)){
////				speed -= base_spd;
////			}
////			else if(X_BlkPix == (xval+x_buffer)){
////				speed = 0;
////			}
////		}
////		speed = speed>spd_cap?spd_cap:speed;//cap speed at 30
//
//		side_offset = xval - X_BlkPix + offset_buffer;
//		scoop = side_offset ? 10 : side_offset+20 ? 15 : side_offset+40 ? 20 : side_offset+80 ? 30 : scoop;
//		for(uint16_t i = 0; i < scoop; i++){
//			//speed += base_spd;
//			if(X_BlkPix < (xval+x_buffer)){
//				speed += base_spd;
//			}else if(X_BlkPix > (xval+x_buffer)){
//				speed -= base_spd;
//			}
////			else if(X_BlkPix == (xval+x_buffer)){
////				speed = 0;
////			}
//			move[2] = speed; //store speed
//			move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
//			uint8_t state = 0x0E; //initial state is notok aka ntok
//			HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
//			HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
//			HAL_Delay(300);
//		}
//		speed = speed>spd_cap?spd_cap:speed;//cap speed at 30
//	}
//	void fspeed(uint8_t* move)//front speed calculation
//	{
////		// offset difference in area
//////		speed = ((zval - area)/area) * area_divider;//calculate change for acceleration
////		area_offset = ((zval - area)/area) * area_divider;
//		scoop = (area>zval+20)?10:(area>zval+40)?15:(area>zval+60)?20:(area>zval+80)?25:(area>zval+80)?30:scoop;
//		for(uint16_t i = 0; i < scoop; i++){
//			//speed += base_spd;
//			if(area < (zval+area_buff)){
//				speed += base_spd;
//			}else if(area > (zval+area_buff)){
//				speed -= base_spd;
//			}
////			else if(X_BlkPix == (xval+x_buffer)){
////				speed = 0;
////			}
//			move[2] = speed; //store speed
//			move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
//			uint8_t state = 0x0E; //initial state is notok aka ntok
//			HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
//			HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
//			HAL_Delay(300);
//		}
//
//		speed = speed>line_cap?line_cap:speed;//cap speed at 30
//	}
//	void bspeed(uint8_t* move)//Back speed calculation
//	{
//		// calculate offset difference in area
////		speed = ((area - zval)/zval) * area_divider;//calculate change for acceleration
//		scoop = (area>zval+20)?10:(area>zval+40)?15:(area>zval+60)?20:(area>zval+80)?25:(area>zval+80)?30:scoop;
//		for(uint16_t i = 0; i < scoop; i++){
//			//speed += base_spd;
//			if(area > (zval+area_buff)){
//				speed += base_spd;
//			}else if(area < (zval+area_buff)){
//				speed -= base_spd;
//			}
////			else if(X_BlkPix == (xval+x_buffer)){
////				speed = 0;
////			}
//			move[2] = speed; //store speed
//			move[3] = move[0] + move[1] + move[2];//store check sum by addition as total val is below 255
//			uint8_t state = 0x0E; //initial state is notok aka ntok
//			HAL_UART_Receive(&huart1, &state, sizeof(state), 10); //receive and store
//			HAL_UART_Transmit(&huart1, move, sizeof(move), 10);//transmit movement to comm2control
//			HAL_Delay(300);
//		}
//		speed = speed>line_cap?line_cap:speed;//cap speed at 30
//	}
