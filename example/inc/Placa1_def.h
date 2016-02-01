/*
 * Placa1_def.h
 *
 *  Created on: 2/10/2015
 *      Author: Juani
 */


#ifndef PLACA1_DEF_H_
#define PLACA1_DEF_H_
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "DHT.h"





//puertos
#define		PORT0		0
#define		PORT1		1
#define		PORT2		2
#define		PORT3		3
#define		PORT4		4


//salidas
#define		LEDXpresso	PORT0,22

//uart
#define 	PIN_tx_rs485     PORT0,15
#define 	PIN_rx_rs485     PORT0,16
#define 	PIN_TE_485		 PORT0,22

//SENSOR:
#define		DHT_DATA	PORT0,23

//usb

#define			PIN_CONT_USB PORT2,9


#define			ID_ACTUAL_RPM 	(uint8_t)120
#define			ID_ACTUAL_NIVEL	(uint8_t)140
#define			ID_ACTUAL_TEMP 	(uint8_t)160
#define			ID_ACTUAL_HUM 	(uint8_t)180

// comunic rs-485

#define master_id 		1769
#define esclavo_1 		425
#define rps_id 			13

#define esclavo_2 		333
#define nievel_id 		77


#define trama 5

//Defines generales:
#define		OFF			0
#define		ON			1

#define		MODO0		0
#define		MODO1		1
#define 	MODO2     	2
#define		MODO3		3

#define 	FUNCION0  	0
#define 	FUNCION1  	1
#define 	FUNCION2  	3
#define 	FUNCION3  	3

//buffer circulares para rs-485



#define UART_SRB_SIZE 32	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

// dirección en FIODIR
#define 	ENTRADA			0
#define 	SALIDA			1



#define		__R					volatile const
#define		__W					volatile
#define		__RW				volatile

typedef 	unsigned int 		uint32_t;
typedef 	unsigned short 		uint16_t;
typedef 	unsigned char 		uint8_t;

void kitInic(void);
void InicGPIOs ( void );
void InicTimers(void);
void InicUart1(void);
void usb_init(void);


//colas para las comunicaciones
xQueueHandle tx_uart;
xQueueHandle rx_uart;
xQueueHandle bomba; //cola con los nuevos datos del usb para setar el nivel del tanque
xQueueHandle motor;//cola con los nuevos datosdel usb para setar las rpm
xQueueHandle set_rpm;//cola con el valor los valores a setear
xQueueHandle set_nivel;//colas con los valores a setear
xQueueHandle actual_rpm;//cola con los valores para actualizar las rpm en la pc
xQueueHandle actual_nivel;//cola con los valores para actualizar el nivel del tanque en la pc
xQueueHandle actual_temp;//cola con los valores para actualizar la temp en la pc
xQueueHandle actual_hum;//cola con los valores para actualizar la humedad en la pc

//semaforos para la comun 485
xSemaphoreHandle Sem_env;
xSemaphoreHandle Sem_rec;
xSemaphoreHandle sem_procDatos;

//semaforos para el sensor
xSemaphoreHandle sem_mach1_timer3;
xSemaphoreHandle sem_timeOUT;

//semaforos para encuesta de esclavos
xSemaphoreHandle sem_encuesto_escalvo1;
xSemaphoreHandle sem_encuesto_escalvo2;

xSemaphoreHandle Sem_rec;

uint8_t VENCIOTimeout;



#endif /* PLACA1_DEF_H_ */
