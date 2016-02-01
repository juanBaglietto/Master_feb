/*
 * Mde_DHT.c
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */
#include "DHT.h"


 //extern uint8_t VENCIOTimeout;

//Funcion con maquina de estados
int DHT_Mde(void){



	static uint8_t estado=1;
	static uint8_t byte[5];
	static uint8_t i=0,j=7;//i recorre los bytes y j los bits de cada byte
	static 	uint8_t Humestring[7];
	static	uint8_t Tempstring[8];


	uint8_t neg=0;
	uint16_t checkmio=0;
	uint16_t hume=0,temp=0,check=0;
	uint16_t ent_temp,dec_temp,ent_hume,dec_hume; //partes enteras y partes decimales

	uint32_t timerFreq;
	timerFreq = Chip_Clock_GetSystemClockRate();


	switch(estado){
	case 1:
		DHT_SALIDA; //LO HAGO SALIDA
		DHT_SETLOW;
		Chip_TIMER_SetMatch(LPC_TIMER3, 1, timerFreq / (40)); //seteo el mach1 en 25ms
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
		Chip_TIMER_Reset(LPC_TIMER3);
		Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);//ARRANCO TIMER QUE ESPERA 25ms
		estado=2;

		break;
	case 2:
		xSemaphoreTake(sem_mach1_timer3, portMAX_DELAY);
		Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
		DHT_SETHIGH;


		Chip_TIMER_SetMatch(LPC_TIMER3, 1, timerFreq / (33333)); //seteo el mach 1 con 30us
		Chip_TIMER_Reset(LPC_TIMER3);
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
		Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
		estado = 3;
		break;
	case 3:
		xSemaphoreTake(sem_mach1_timer3, portMAX_DELAY);
		Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
		DHT_ENTRADA;//LO HAGO ENTRADA
		Chip_TIMER_SetMatch(LPC_TIMER3, 1, timerFreq / (50000)); //seteo el mach 1 con 30us
		Chip_TIMER_Reset(LPC_TIMER3);
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
		Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
		estado=4;

		break;
	case 4:
		xSemaphoreTake(sem_mach1_timer3, portMAX_DELAY);
		Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);

		Chip_TIMER_Reset(LPC_TIMER2);
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
		Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
		estado=5;

		break;
	case 5:
		//Primero espero un uno...
		if(DHT_is==true) //is hig
		{
			estado=6;
			Chip_TIMER_Reset(LPC_TIMER2);
			Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
			Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
		}//termina el 0 de 80us
		if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
		{
			estado=1;
			VENCIOTimeout=0;
			Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);
			//termino lectura

			return 11; //DEVUELVO ERROR
		}
		break;
	case 6:
		if(DHT_is==false) //espero un 0
		{
			estado=7; //tengo q ir esperar el 1

			Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);//paro el timeout

		}//termina el 1 de 80us
		if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
		{
			estado=1;
			//termino lectura
			VENCIOTimeout=0;
			Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);
			return 11; //DEVUELVO ERROR
		}
		break;
	case 7:	//espera un 0
		if(DHT_is==false)
		{
			estado=8;
		}
		if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
		{
			estado=1;
			//termino lectura
			VENCIOTimeout=0;
			Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);
			return 11; //DEVUELVO ERROR
		}
		break;
	case 8: //espero un 1
		if(DHT_is==true)
		{
			estado=9;
			Chip_TIMER_SetMatch(LPC_TIMER3, 1, timerFreq / (25000)); //seteo el mach 1 con 40us
			Chip_TIMER_Reset(LPC_TIMER3);
			Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
			Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);

		} //llego el 1 de transmision
		if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
		{
			estado=1;
		//termino lectura
			VENCIOTimeout=0;
			Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);
			return 11; //DEVUELVO ERROR
		}
		break;
	case 9:
		xSemaphoreTake(sem_mach1_timer3, portMAX_DELAY);
		Chip_TIMER_MatchDisableInt(LPC_TIMER3, 1);
		if(DHT_is==true)
		{
			byte[i]|=(uint8_t)0x01<<j;//pongo un uno en la posicion
			estado=7; //espero q se haga cero
			Chip_TIMER_Reset(LPC_TIMER2);
			Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
			Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
		}
		else{ //la lectura fue un 0, espero al proximo 1
			estado=8;
			Chip_TIMER_Reset(LPC_TIMER2);
			Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
			Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
		}
		j--;
		if(j>7) //era 0 y ahora hizo overflow
		{
			i++;
			j=7;
			if(i==5){
				estado=10; //termino la lectura de los 5 bytes
			}
		}
		//
		break;
	case 10:
		Chip_TIMER_MatchDisableInt(LPC_TIMER2, 1);
		checkmio=byte[0]+byte[1]+byte[2]+byte[3];
		checkmio&=0x00ff;
		//anido los datos
		hume=byte[0];
		hume=hume<<8;
		hume|=byte[1];
		temp=byte[2];
		temp=temp<<8;
		temp|=byte[3];
		check=byte[4];
		//checkeo si lo que me llego es correcto
//					if(checkmio!=check)
//					{
//						estado=0;
////						i=0;
////						j=7;
//						VENCIOTimeout=0;
////						check=0;
////						checkmio=0;
////						byte[0]=byte[1]=byte[2]=byte[3]=byte[4]=0;//termino lectura
//						return 11; //suma incorrecta
//
//					}
		//datos correctamente recibidos
		//estan en hume y temp
		// checkeo si la temperatura es negativa
		if(temp & 0x8000)//si esto es verdad es negativa
		{
			neg=1;//activo un flag
			temp=temp & 0x7FFF;//le saco el uno del bit mas significativo que indica negatividad
		}
		//decodifico la información
		dec_temp=temp%10;
		ent_temp=temp/10;
		dec_hume=hume%10;
		ent_hume=hume/10;

		//La coloco en el buffer que manipularán otras funciones:
		Humestring[6] = '\0';//para que puea ser manipulado como string
		Humestring[5] = 'H';
		Humestring[4] = ' ';
		Humestring[3] = '0'+dec_hume;
		Humestring[2] = ',';
		for (i=1;i>=0 && i<=1;i--)
		{
			Humestring[i]='0'+ent_hume%10;
			ent_hume=ent_hume/10;
		}

		Tempstring[7] = '\0';//para que puea ser manipulado como string
		Tempstring[6] = 'C';
		Tempstring[5] = ' ';
		Tempstring[4] = '0'+dec_temp;
		Tempstring[3] = ',';
		for (i=2;i>=1 && i<=2;i--)
		{
			Tempstring[i]='0'+ent_temp%10;
			ent_temp=ent_temp/10;
		}
		if(neg==1)//si es negativo le agrego un menos
		{
			Tempstring[0]='-';
		}
		else{
			Tempstring[0]=' ';//si es positivo dejo un espacio
		}

		xQueueSendToBack(actual_temp,&Tempstring,0);
		xQueueSendToBack(actual_hum,&Humestring,0);
		estado=0; //para la proxima vez que entre

		i=0;
		j=7;
		byte[0]=byte[1]=byte[2]=byte[3]=byte[4]=0;
		return 10;
		 //termino bien la lectura

		break;
	}
	return 0;//return estado; //lectura correcta por el momento
}

