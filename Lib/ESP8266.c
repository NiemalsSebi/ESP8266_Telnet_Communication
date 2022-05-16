/*  ************************************************************************
  * @copyright HTL - HOLLABRUNN  2022 All rights reserved AUSTRIA
  * @file    ESP8266.h 
  * @author  Tobias Meszlenyi & Simon Wild
  * @version V1.0
  * @date    07-March-2022
  * @brief   Source File for ESP8266 Library

  *************************************************************************
  * @history 06/03/2021: WILD: Creation
	* @history 07/03/2022: MESZ: Final
  ************************************************************************/

	
/*************************************************************************
* @brief Portuebersicht
**************************************************************************
* P
* P
*
**************************************************************************
*/

/* --------------------------------Includes -----------------------------*/
#include <stm32f10x.h>
#include "stdio.h"
#include "string.h"


/* D E F I N E S */

// Calc Bit Band Adress from peripheral address: 
// a = peripheral address b = Bit number
#define BITBAND_PERI(a,b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))

#define GPIOA_ODR GPIOA_BASE+3*sizeof(uint32_t) //address GPIOA ODR


/* P R O T O T Y P E N */
void wait_ms(int ms);
void init_USART2(void);
void init_USART1(void);
void uart2_put_char(char zeichen);
void uart2_put_string(char *string);
char uart2_get_char(void);
void uart1_put_char(char zeichen);
void uart1_put_string(char *string);
char uart1_get_char(void);
void nib2asc(char* nib);
void uart_put_hex(char c);
char ESP8266_configure(void);

/* U N T E R P R O G R A M M E */

/*************************************************************************
* @brief Initialize USART2 for 115200,n,8,1
*
* @retval none
* @param none
* @note SysClk must be 72MHz
*************************************************************************/
void init_USART2(void) {
	
	RCC->APB2ENR |= 0x4;      // Enable clock for GPIOA
	
	GPIOA->CRL &= 0xFFFFF0FF; // Reset PA2 configuration-bits
	GPIOA->CRL |= 0xB00; 			// Tx (PA2) - alt. out push-pull
	
	GPIOA->CRL &= 0xFFFF0FFF; // Reset PA3 configuration-bits
	GPIOA->CRL |= 0x4000; 			// Rx (PA3) - floating
	
	RCC->APB1ENR |= 0x20000;	  // USART2 mit einem Takt versrogen
	
	USART2->CR1 &= ~0x1000; 	// M: --> Start bit, 8 Data bits, n Stop bit
	USART2->CR1 &= ~0x0400;	  // PCE: 0: Parity control disabled
	USART2->CR2 &= ~0x3000; 	// STOP: 00: 1 Stop bit
	USART2->BRR = 0x0138; 		// set Baudrate to 115200 Baud (SysClk 36Mhz)
	USART2->CR1 |= 0x0C; 			// enable Receiver and Transmitter
	USART2->CR1 |= 0x2000; 		// Set USART Enable Bit
}

/*************************************************************************
* @brief Initialize USART1 for 19200,n,8,1
*
* @retval none
* @param none
* @note SysClk must be 72MHz
*************************************************************************/
void init_USART1(void) {
	
	RCC->APB2ENR |= 0x4;      // Enable clock for GPIOA
	
	GPIOA->CRH &= 0xFFFFFF0F; // Reset PA9 configuration-bits
	GPIOA->CRH |= 0xB0; 			// Tx (PA9) - alt. out push-pull
	
	GPIOA->CRH &= 0xFFFFF0FF; // Reset PA10 configuration-bits
	GPIOA->CRH |= 0x400; 			// Rx (PA10) - floating
	
	RCC->APB2ENR |= 0x4000;	  // USART1 mit einem Takt versrogen
	
	USART1->CR1 &= ~0x1000; 	// M: --> Start bit, 8 Data bits, n Stop bit
	USART1->CR1 &= ~0x0400;	  // PCE: 0: Parity control disabled
	USART1->CR2 &= ~0x3000; 	// STOP: 00: 1 Stop bit
	USART1->BRR = 0x0EA6; 		// set Baudrate to 19200 Baud (SysClk 72Mhz)
	USART1->CR1 |= 0x0C; 			// enable Receiver and Transmitter
	USART1->CR1 |= 0x2000; 		// Set USART Enable Bit
}

/************************************************************************/
/* W A R T E S C H L E I F E */
/************************************************************************
* @brief Wait about 1milisecond * (wanted time) (Works only with SysClk 72MHz)
* 
* @param ms - Time to wait in miliseconds
* @retval none
*************************************************************************/
void wait_ms(int ms){
int j;
	for(j = 0; j < 7987*ms; j++) {
	   }
} 

/************************************************************************
* @brief Send one charcter via USART2
*
* @param zeichen - character to send
* @retval none
*************************************************************************/
void uart2_put_char(char zeichen)
{
	while (!(USART2->SR & 0x80)); //last char sent ?
	USART2->DR = zeichen;	// send char
}

/************************************************************************
* @brief Send one charcter via USART1
*
* @param zeichen - character to send
* @retval none
*************************************************************************/
void uart1_put_char(char zeichen)
{
	while (!(USART1->SR & 0x80)); //last char sent ?
	USART1->DR = zeichen;	// send char
}

/*************************************************************************
* @brief Sends one '\0' terminated String via USART2
*
* @param string - pointer to '0' terminated String (C convention)
* @retval none
*************************************************************************/
void uart2_put_string(char *string)
{
	while (*string) {
		uart2_put_char (*string++);
	}
}

/*************************************************************************
* @brief Sends one '\0' terminated String via USART1
*
* @param string - pointer to '0' terminated String (C convention)
* @retval none
*************************************************************************/
void uart1_put_string(char *string)
{
	while (*string) {
		uart1_put_char (*string++);
	}
}

/*************************************************************************
* @brief Receives sent character from UART1
*
* @param none
* @retval received character
*************************************************************************/
char uart1_get_char(void) {	// how to get string=???
	// Check if Data is ready to be recieved
	while((USART1->SR&0x20)) {
		return USART1->DR;
	}
	return 0;
}

/*************************************************************************
* @brief Receives sent character from UART2
*
* @param none
* @retval received character
*************************************************************************/
char uart2_get_char(void) {	// how to get string=???
	// Check if Data is ready to be recieved
	while((USART2->SR&0x20)) {
		return USART2->DR;
	}
	return 0;
}


/*************************************************************************
* @brief Configures ESP8266 as stated per Angabe
*				 + Init USART1 and USART2
*
* @note All AT commands must end with \n\r
* @param none
* @retval none
*************************************************************************/
char ESP8266_configure(void) {
	init_USART1();
	init_USART2();
	
	// https://www.electronicshub.org/esp8266-at-commands/
	
	// Test Connection (expect "OK") -- If not exit with Error 1
	uart2_put_string("AT\n\r");
	if(!(uart2_get_char()=='O') && !(uart2_get_char()=='K')) {
		uart1_put_string("Error ESP");
		return 1;
	}
	
	// Create WIFI network
	uart2_put_string("AT+CWMODE=2\n\r");
	uart2_put_string("AT+CWSAP=\"espTobi_Wild\", \"1234\",5,3 "); // Create Wifi network with name "espTobi", Password "1234", use channel 5 with WPA2_PSK Encryption
	// Give esp 172.168.1.1 Address
	// Give Gateway adress 172.168.1.100
	// Create Subnet 255.255.255.0
	uart2_put_string("AT+CIPSTA=\"172.168.1.100\", \"172.168.1.1\", \"255.255.255.0\"\n\r");  
	// Start Server + Allow muiltiple Connections
	uart2_put_string("AT+CIPMUX=1\n\r");
	// Start Telnet Server @ Port 23
	uart2_put_string("AT+CIPSERVER=1,23\n\r");
	
	return 0;
}


/*************************************************************************
* @brief Function converts a Nibble(0-F) to an ASCII ('0'-'F')
*
* @param nib - Nibble to convert
* @retval nib - Converted Nibble
*************************************************************************/
void nib2asc(char *nib)
{
	*nib &= 0x0F;	// extract Nibble
	if(*nib >= 0x0A) {	// Hex value A-F ?
			*nib -= 0x0A;
			*nib += 'A';
		}
	else {	// Hex value 0 - 9
			*nib += '0';
		}
}


/*************************************************************************
* @brief Send a hex number via USARTx (2 characters)
*
* @param char - hexnumber (0-FF)
* @retval none
************************************************************************/

void uart_put_hex(char c)
{
	char help;
	help = (c>>4)&0x0f;		// Extract High Nibble
	nib2asc(&help);				// Convert to ASCII Code
	uart1_put_char(help);	// Send High Nibble auf USART1
	help = c&0x0f;				// Extract Low Nibble
	nib2asc(&help);				// Convert to ASCII Code
	uart1_put_char(help);	// Send Low Nibble auf USART1
	uart1_put_string("\n\r\n\r");
}


 /* H A U P T P R O G R A M M */
int main() {
	
	init_USART1();
	init_USART2();
	
	uart1_put_string("ESP8266_Library\n\r\n\r");
	
	if(ESP8266_configure()==1) {
		// ERROR
		uart1_put_string("ERROR\n\r");
		wait_ms(10000);
		uart1_put_string("Shutdown programme");
		return 0;
	}

	wait_ms(100);
	
	char buf;
	char buf1;
	char buf2;

	
	while(1) {
		
		// Did something get send to ESP??
		if(USART2->SR&0x20) {
			// +IPD,0,44:(TEXT) is expected.
			// - 0 in this case is the client that sent it; this is important
			// So we filter this out
			for(char i=0; i<=4; ++i) {uart2_get_char(); }
			char client=uart2_get_char();
			
			// test to terminal 
			uart1_put_char(client);
			
			// Now we need the message part
			for(char i=0; i<=3; ++i) {uart2_get_char(); }
			// Message starts now (100 charcarcters max)
			char message[100];
			char i=0;
			char buf='x';
			while(buf!='\n' && i<100) {
				buf=uart2_get_char();
				message[i]=buf;
				++i;
			}
			uart2_get_char();
			
			/* All USART1 here are for debug purposes */
			
			// send to client with length of message
			uart2_put_string("AT+CIPSEND=");
			uart1_put_string("AT+CIPSEND=");
			// Since we only got 2 users we look who it sent and send it to the other
			if(client=='0') {
				uart2_put_char('0');
				uart1_put_char('0');
			}
			else {
				uart2_put_char('1');
				uart1_put_char('1');
			}
			uart2_put_char(',');
			uart1_put_char(',');
			char buflen[10];
			sprintf(buflen,"%d",(strlen(message)));
			uart2_put_string(buflen);
			uart1_put_string(buflen);
			uart2_put_string("\n\r");
			uart1_put_string("\n\r");
			uart2_put_string(message);		
			uart1_put_string(message);
			uart2_put_string("\r");	
			uart1_put_string("\r");	
			// Message should now appear on different terminal
		}
		
	}
		/* https://petestechprojects.wordpress.com/2014/11/30/simple-communication-through-esp8266-wifi-module/comment-page-1/ */
		/* https://docs.espressif.com/projects/esp-at/en/latest/esp32/AT_Command_Set/Wi-Fi_AT_Commands.html#cmd-mode */
}
