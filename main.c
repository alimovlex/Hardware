#include "MDR32Fx.h"                    // Device header
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "MDR32F9Qx_uart.h"             // Keil::Drivers:UART
#include "diskio.h"
#include "ff.h"
#include "ffconf.h"
#include "integer.h"
#include "rtc.h"
#include "mlt_font.h"
#include "mlt_lcd.h"
#include "lcd.h"
#include "rst.h"
#include "string.h"
#include "types.h"
#include "joystick.h"
#define delay(T) for(i = 0; i < T; i++) 
int i;

PORT_InitTypeDef PortInit;   
UART_InitTypeDef UART_InitStructure; 
uint32_t uart2_IT_TX_flag = RESET;  
uint32_t uart2_IT_RX_flag = RESET;  
FATFS fs; //structure
FIL file; //store file information here
UINT lin; //store real amount of bits
FRESULT res; //for analyze of returned results
DIR dj;
char buffer[20];
unsigned char temp[1];
//char str[] = "Hello, world!";
void UART2_IRQHandler(void)
{
if (UART_GetITStatusMasked(MDR_UART2, UART_IT_RX) == SET)

  {
    UART_ClearITPendingBit(MDR_UART2, UART_IT_RX);
    uart2_IT_RX_flag = SET; 
  }
if (UART_GetITStatusMasked(MDR_UART2, UART_IT_TX) == SET)

  {
    UART_ClearITPendingBit(MDR_UART2, UART_IT_TX); 
    uart2_IT_TX_flag = SET; 
  }
}

void UART2init(void) 
{

    RST_CLK_HSEconfig(RST_CLK_HSE_ON);

    if (RST_CLK_HSEstatus() == SUCCESS){    
        
        RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 7);
       
        RST_CLK_CPU_PLLcmd(ENABLE);
        if (RST_CLK_HSEstatus() == SUCCESS){ 
            RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV2); 
            RST_CLK_CPU_PLLuse(ENABLE); 
            
            RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
        }
        else while(1);
    }
    else while(1); 
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE); 
       PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
    PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
    PortInit.PORT_PD = PORT_PD_DRIVER;
    PortInit.PORT_GFEN = PORT_GFEN_OFF;
    PortInit.PORT_FUNC =     PORT_FUNC_OVERRID; 
    PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;
   
    PortInit.PORT_OE = PORT_OE_OUT;
    PortInit.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTF, &PortInit);
   
    PortInit.PORT_OE = PORT_OE_IN;
    PortInit.PORT_Pin = PORT_Pin_0;
    PORT_Init(MDR_PORTF, &PortInit);
   
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
    
    UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
    
    NVIC_EnableIRQ(UART2_IRQn);
    
    UART_InitStructure.UART_BaudRate   = 115200;    
    UART_InitStructure.UART_WordLength = UART_WordLength8b; 
    UART_InitStructure.UART_StopBits   = UART_StopBits1;    
    UART_InitStructure.UART_Parity     = UART_Parity_No;    
    UART_InitStructure.UART_FIFOMode   = UART_FIFO_OFF;    
    
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | 
UART_HardwareFlowControl_TXE;       
    UART_Init (MDR_UART2, &UART_InitStructure);   
    UART_ITConfig (MDR_UART2, UART_IT_RX, ENABLE);
    UART_ITConfig (MDR_UART2, UART_IT_TX, ENABLE);
    UART_Cmd(MDR_UART2, ENABLE);
}
void UART2SD (void)
{
    static uint8_t ReciveByte=0x00; 		char empty[] = " ";
	disk_initialize(0);
			f_mount(0, &fs);
	f_mkdir("0:UARTdata");
	f_open(&file, "0:UARTdata/data.txt", FA_CREATE_NEW | FA_READ | FA_WRITE);
	f_write(&file, empty, sizeof(empty), &lin);
	f_close(&file);
    while (1) {
        while (uart2_IT_RX_flag != SET); 
        uart2_IT_RX_flag = RESET;          
        ReciveByte = UART_ReceiveData (MDR_UART2); 
f_open(&file, "0:UARTdata/data.txt", FA_OPEN_EXISTING | FA_WRITE);
f_lseek(&file, f_size(&file));
	f_write(&file, &ReciveByte, sizeof(ReciveByte), &lin);
	f_close(&file);			
        UART_SendData (MDR_UART2, ReciveByte); 
        while (uart2_IT_TX_flag != SET); 
        uart2_IT_TX_flag = RESET; 
    }
}		
 void U_LCD_Task_Function ()
{	
	uint32_t k = 0;
	const char s[] =  "\xD1\xCF\xC1\xC3\xD3\xD2 \xC1\xCE\xCD\xD7\x20\x32\x30\x31\x37";
	f_open(&file, "0:UARTdata/data.txt", FA_OPEN_EXISTING | FA_READ);   
	f_read(&file, buffer, sizeof(buffer), &lin);	
  U_MLT_Put_String (buffer, 3);	
	U_MLT_Scroll_String (s, 7, k++);
	while(1)
	{
		delay(0xEEEEE); 
	U_MLT_Scroll_String(buffer, 3, k++);	
	}
}
void Button_Init ()
{
RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE); 
    PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;
    PortInit.PORT_OE = PORT_OE_IN;
    PortInit.PORT_Pin = PORT_Pin_All;
    PORT_Init(MDR_PORTC, &PortInit);		
}
void LED_Init()
{
PORT_InitTypeDef PORTDInit; 
PORT_InitTypeDef PORTEInit; 

RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE); 


	PORTDInit.PORT_Pin = PORT_Pin_10 | 
	PORT_Pin_11 |
	PORT_Pin_12 |
	PORT_Pin_13 | 
	PORT_Pin_14;
	PORTDInit.PORT_OE = PORT_OE_OUT; 
	PORTDInit.PORT_FUNC = PORT_FUNC_PORT; 
	PORTDInit.PORT_MODE = PORT_MODE_DIGITAL; 
	PORTDInit.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MDR_PORTD, &PORTDInit); 
	

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE); 
	

	PORTEInit.PORT_Pin = PORT_Pin_10;
	PORTEInit.PORT_OE = PORT_OE_IN; 
	PORTEInit.PORT_FUNC = PORT_FUNC_PORT; 
	PORTEInit.PORT_MODE = PORT_MODE_DIGITAL;
	PORTEInit.PORT_SPEED = PORT_SPEED_SLOW; 
	PORT_Init(MDR_PORTC, &PORTEInit); 

PORT_SetBits(MDR_PORTD, PORT_Pin_10); 
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_10); 
		PORT_SetBits(MDR_PORTD, PORT_Pin_11);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_11);
		PORT_SetBits(MDR_PORTD, PORT_Pin_12);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_12);
		PORT_SetBits(MDR_PORTD, PORT_Pin_13);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_13);
		PORT_SetBits(MDR_PORTD, PORT_Pin_14);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_14);

		PORT_SetBits(MDR_PORTD, PORT_Pin_14); 
		delay(0xFFFF); 
		PORT_ResetBits(MDR_PORTD, PORT_Pin_14); 
		PORT_SetBits(MDR_PORTD, PORT_Pin_13);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_13);
		PORT_SetBits(MDR_PORTD, PORT_Pin_12);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_12);
		PORT_SetBits(MDR_PORTD, PORT_Pin_11);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_11);
		PORT_SetBits(MDR_PORTD, PORT_Pin_10);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_10);
	}
int main(void) 
{
disk_initialize(0);
U_RST_Init ();	
f_mount(0, &fs);
f_mkdir("0:UARTdata");
f_open(&file, "0:UARTdata/data.txt", FA_CREATE_NEW | FA_READ | FA_WRITE);
UART2_IRQHandler();	
UART2init();	
//UART2SD();			
Button_Init ();	
U_MLT_Init ();

while(1)
{
if(GetKey() == DOWN)
		{	
LED_Init();	
	}	
if(GetKey() == SEL)
		{
		U_LCD_Task_Function();	
	  }		
		}	 
}
