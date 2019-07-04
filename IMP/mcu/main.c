/*
	Jmeno a prijmeni:		Klara Necasova <xnecas24@stud.fit.vutbr.cz>
	Login:					xnecas24
    Popis změn:             Pridana nova vlakna pro obsluhu robota, lisu, pasu a LCD displeje.
	Projekt:				IMP - Rizeni robota na vyrobni lince
	Procento zmen:	        90%
	Poslední zmena:			11.12.2016
*/

/*******************************************************************************
   FreeRTOS (see http://www.freertos.org/)
   Copyright (C) 2010 Brno University of Technology,
                      Faculty of Information Technology
   Author(s): Josef Strnadel <strnadel AT stud.fit.vutbr.cz>
              Zdenek Vasicek <vasicek AT fit.vutbr.cz>

   LICENSE TERMS

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. All advertising materials mentioning features or use of this software
      or firmware must display the following acknowledgement:

        This product includes software developed by the University of
        Technology, Faculty of Information Technology, Brno and its
        contributors.
   4. Neither the name of the Company nor the names of its contributors
      may be used to endorse or promote products derived from this
      software without specific prior written permission.

   This software or firmware is provided ``as is'', and any express or implied
   warranties, including, but not limited to, the implied warranties of
   merchantability and fitness for a particular purpose are disclaimed.
   In no event shall the company or contributors be liable for any
   direct, indirect, incidental, special, exemplary, or consequential
   damages (including, but not limited to, procurement of substitute
   goods or services; loss of use, data, or profits; or business
   interruption) however caused and on any theory of liability, whether
   in contract, strict liability, or tort (including negligence or
   otherwise) arising in any way out of the use of this software, even
   if advised of the possibility of such damage.

   $Id$
*******************************************************************************/

#include <fitkitlib.h>
#include "FreeRTOS.h" /* see http://www.freertos.org/ */
#include "task.h"

#include <keyboard/keyboard.h>
#include <lcd/display.h>

/*
 * Prototypy funkci - vlakna.
 */
// vlakno pro obsluhu robota
static void robotTask(void *param);
// vlakno pro obsluhu lisu 
static void pressTask(void *param); 
// vlakno pro obsluhu pasu
static void shiftTask(void *param);

// vlakno pro obsluhu terminalu
static void terminalTask(void *param); 
// vlakno pro obsluhu klavesnice
static void keyboardTask(void *param);
// vlakno pro obsluhu LCD displeje
static void LCDTask(void *param);  

/*
 * Protypy dalsich funkci.
 */ 
int FBEmpty();
void FBInsert();
void DBInsert();
int FBDBCheckPosition(int x, int y, int beltType);
void FBDBShift();
void LCD_info();
void switchArms();

/*
 * Potrebne promenne a signaly.
 */
volatile char last_ch;
unsigned char pressed = 0;
unsigned long cnt = 0;

/*
 * Konstanty.
 */
// pocet znaku pasu
const unsigned int LCDParts = 10;

/*
 * Vstupy radice.
 */
// signalizace noveho materialu na FB
unsigned int IN	= 0;
// aktualni poloha (0-3)
unsigned int POS = 0;

/*
 * Vystupy radice.
 */
unsigned int LEFT	= 0;
unsigned int RIGHT	= 0;
unsigned int STEPS	= 0;
unsigned int GETA	= 0;
unsigned int PUTA	= 0;
unsigned int GETB	= 0;
unsigned int PUTB	= 0;

/*
 * Interni signaly.
 */
unsigned int noActionCnt = 0;
unsigned int notGetA = 0;
unsigned int armReadyB = 0;

/*
 * Pole reprezentujici pasy.
 */
unsigned int FB[5]	= {0, 0, 0, 0, 0};
unsigned int DB[5]	= {0, 0, 0, 0, 0};

/*
 * Definice grafickych prvku displeje.
 */
// material
unsigned char charmap1[8] = {0x1F, 0x15, 0x15, 0x1F, 0x1F, 0x15, 0x15, 0x1F};
// staticka cast ramene
unsigned char charmap2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F};
// pohybliva cast ramene
unsigned char charmap3[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
// pohybliva a staticka cast ramene
unsigned char charmap4[8] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
// pas
unsigned char charmap5[8] = {0x00, 0x00, 0x00, 0x1F, 0x1F, 0x00, 0x00, 0x00};
// prazdny lis
unsigned char charmap6[8] = {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};

/**
 * Stavy robota.
 */
enum ROBOT_STATES
{
	NO_ACTION		= 0,
	WAITING_FB		= 1,
	GET_AB			= 2,
	PUT_AB			= 3
};

/**
 * Stavy lisu.
 */
enum PRESS_STATES
{
	EMPTY		= 0,
	WORKING		= 1,
	FULL		= 2
};

/**
 * Typy pasu.
 */
enum BELT_TYPES
{
	FB_BELT		= 0,
	DB_BELT		= 1
};

// vychozi stav robota
int robotState = NO_ACTION;
// vychozi stav lisu
int pressState = EMPTY;

void FBInsert()
{
    // DEBUG
    //term_send_str_crlf("FBInsert");
	int i;
	// predpokladany maximalni pocet kusu materialu: 5
	for(i = 0; i < 5; i++)
	{
		if(FB[i] == 0)
		{// prazdny pas 
			// vlozeni materialu (posun materialu - inkrementace hodnoty pole od 1 do 50)
			FB[i] = 1;
			term_send_str_crlf("FB: Vlozena nova polozka.");
			break;
		}
		if(i == 4)
		{// plny pas
			term_send_str_crlf("FB: Pas je zaplnen.");
		}
	}
}

void DBInsert()
{
	int i;
	// predpokladany maximalni pocet kusu materialu: 5
	for(i = 0; i < 5; i++)
	{
		if(DB[i] == 0)
		{// prazdny pas
			// vlozeni materialu (posun materialu - dekrementace hodnoty pole od 49 do 0)
			DB[i] = 50;
			term_send_str_crlf("DB: Vlozena nova polozka.");
			break;
		}
	}
}

void switchArms()
{
    // uhel rotace: 90 stupnu
    STEPS = 1;
    if(POS == 0)
    {// rotace doprava
        RIGHT = 1;
        LEFT = 0;
    }
    else
    {// rotace doleva
        LEFT = 1;
        RIGHT = 0;
    }
    term_send_str_crlf("ROBOT: Rameno presunuto.");
    // zpozdeni 900 ms - 1 stupen za 10 ms -> 90 stupnu za 900 ms (90 * 10 = 900 ms) 
    vTaskDelay(900/portTICK_RATE_MS);
    // aktualizace polohy ramena
    if(POS == 0)
    {
        POS = 1;
    }
    else
    {
        POS = 0;
    }
    // konec rotace
    STEPS = 0;
}

static void robotTask(void *param) 
{
    // DEBUG
    //term_send_str_crlf("robotTask.");
	while(1)
	{
		switch(robotState)
		{
			case NO_ACTION:
				if(IN == 1)
				{
					robotState = WAITING_FB;
				}
				break;
			case WAITING_FB:
				if(IN == 1)
                {
                   IN = 0; 
                }
                // nastaveni vstupu robota
				GETA = 0;
                PUTA = 0;
                GETB = 0;
                PUTB = 0;
				LEFT = 0;
				RIGHT = 0;
				STEPS = 0;

				if(FBEmpty() && pressState == FULL && robotState == WAITING_FB)
				{// na pasu FB zadny novy material, lis material zpracoval + je treba se vyhnout triggering (otoceni k pasu FB)
					notGetA = 1;
					robotState = GET_AB;
				}
				break;
			case GET_AB:
				if(IN == 1)
                {
                   IN = 0; 
                }
				term_send_str_crlf("ROBOT: Uchopeni materialu ramenem A/B.");
				PUTA = 0;
                PUTB = 0;
				if(!notGetA)
					GETA = 1;
				if(pressState == FULL)
				{
					GETB = 1;
					pressState = EMPTY;      
					armReadyB = 1;
				}
				// zpozdeni 1 s na uchopeni materialu
				vTaskDelay(1000 / portTICK_RATE_MS);
				vTaskSuspendAll();
				switchArms();
				xTaskResumeAll();
				robotState = PUT_AB;
				break;
			case PUT_AB:
				if(IN == 1)
                {
                   IN = 0; 
                }
				term_send_str_crlf("ROBOT: Polozeni materialu ramenem A/B.");
				GETA = 0;
                GETB = 0;
				if(!notGetA)
				{
					PUTA = 1;
					pressState = WORKING;
				}
				else
					notGetA = 0;
				if(armReadyB)
				{
					PUTB = 1;
					armReadyB = 0;
					DBInsert();        
				}
				// zpozdeni 1s na polozeni materialu
				vTaskDelay(1000 / portTICK_RATE_MS);
				vTaskSuspendAll();
				switchArms();
				xTaskResumeAll();
                // MY
                //if(pressState == FULL)
                    //pressState = EMPTY;
				robotState = WAITING_FB;
				break;
		}
	}
}

static void pressTask(void *param)
{
    while(1)
    {
        if(pressState == WORKING)
        {
            // zpracovani materialu
            //taskENTER_CRITICAL();
            vTaskDelay(100/portTICK_RATE_MS);
            //taskEXIT_CRITICAL();
            pressState = FULL;
        }
        if(IN == 0 && FBEmpty() && robotState != NO_ACTION)
        {// zadny novy material, prazdny pas FB, ale robot neni v klidovem rezimu
            // 5 m = 500 cm, 10 cm/s => 500:10 = 50 s (doba od vlozeni materilu po dosazeni konce pasu FB); 5:50 (50x posun o 10 cm => 500 cm) = 0,1 s = 100 ms 
            vTaskDelay(100/portTICK_RATE_MS);
            noActionCnt++;
            if(noActionCnt >= 50)
            {
                if(robotState == WAITING_FB && pressState == EMPTY && armReadyB == 0)
                {// robot ceka na novy material, lis nic nezpracovava, rameno nema co zpracovat
                    robotState = NO_ACTION;
                    noActionCnt = 0;
                }
                else
                {
                    continue;
                }
            }
        }        
    }    
}

void FBDBShift()
{
	int i;
    // predpokladany maximalni pocet kusu materialu: 5 
	for(i = 0; i < 5; i++)
	{
		if(FB[i] != 0)
		{// neprazdny pas
			// posun materialu po pasu
			FB[i]++;

			if(FB[i] >= 45 && FB[i] < 50)
			{// priprava ramen robota (predchozi material jiz odevzdan lisu)
				if(robotState == WAITING_FB)
				{
					if(POS == 1)
                    {// otoceni ramene k pasu FB
			vTaskSuspendAll();
                        switchArms();
			xTaskResumeAll();
                    }	
				}
			}
			else if(FB[i] == 50)
			{// material na konci pasu FB
				if(robotState == WAITING_FB)
				{// odstraneni materialu z pasu FB
					FB[i] = 0;
					robotState = GET_AB;
				}
			}
			else if(FB[i] > 50)
			{// material spadl z pasu FB
				FB[i] = 0;
				term_send_str_crlf("FB: Material spadl na podlahu!");
			}
		}

		if(DB[i] >= 1)
		{// posun materialu po pasu DB
			DB[i]--;
		}
	}
}

int FBEmpty()
{
    int i;
    for(i = 0; i < 5; i++)
    {
        if(FB[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}

/*******************************************************************************
 * Bodies of the RT tasks running over FreeRTOS
 *******************************************************************************/
   
/* ---------------------
 keyboard service task
 --------------------- */
static void keyboardTask(void *param)
{
    char ch;
       
    last_ch = 0;
    keyboard_init();
    // DEBUG
    //term_send_str_crlf("keyboardTask.");
    for (;;)
    {
        set_led_d6(0);

        ch = key_decode(read_word_keyboard_4x4());
        if (ch != last_ch) {
            last_ch = ch;
            // DEBUG
            //term_send_str_crlf("keyboardTask - stisk tlacitka.");
            if (ch != 0) {
                switch (ch) {
                    case 'A':
                        // DEBUG
                        //term_send_str_crlf("keyboardTask - stisk A.");
                        pressed = 1;
                        set_led_d6(1);
                        //term_send_str_crlf("\t\tkeyboardTask: 'A' press detected");
                        // signalizace vlozeni materialu na pas
                        IN = 1;
                        // samotne vlozeni materialu na pasu
                        FBInsert();
                        // zpozdeni 5 s - rychlost materialu 10 cm/s, predpokladana minimalni vzdalenost materialu 50 cm (5*10 = 50)
                        vTaskDelay(5000/portTICK_RATE_MS);
                        break;
                    default:
                        break;
                }
            }
        }
        vTaskDelay( 10 / portTICK_RATE_MS); /* delay for 10 ms (= btn-press sampling period) */
    }
}
  
/* ---------------------
 terminal service task
 --------------------- */
static void terminalTask(void *param)
{
    for (;;) {
        terminal_idle();
        vTaskDelay( 1000 / portTICK_RATE_MS); /* delay for 1000 ms */
     }
}

int FBDBCheckPosition(int x, int y, int beltType)
{
	int i;
	for(i = 0; i < 5; i++)
	{
        if(beltType == FB_BELT)
        {
            if(FB[i] > x && FB[i] <= y)
            {
                return 1;
            }
        }
        else
        {
            if(DB[i] > x && DB[i] <= y)
            {
                return 1;
            }
        }
	}
	return 0;
}

static void LCDTask(void *param)
{
	while(1)
	{
		if(robotState == NO_ACTION)
		{
			delay_ms(20);
			LCD_clear();
			delay_ms(20);
			LCD_info("Stiskem A vlozte material.");
			vTaskDelay(1000 / portTICK_RATE_MS);
			continue;
		}

		LCD_clear();

		int i;
		for(i = 0; i < LCDParts; i++)
		{
			// vykresleni pasu DB
			if(FBDBCheckPosition(i * 5, (i + 1) * 5, DB_BELT))
			{
				LCD_append_char('\x1'); 
			}
			else
			{
				LCD_append_char('\x5');
			}
			delay_ms(2);
		}
        
        LCD_append_char(' ');
		delay_ms(2);
        
		// vykresleni staticke, pripadne pohyblive casti ramene
		if(POS == 1)
		{
			LCD_append_char('\x4');
		}
		else
		{
			LCD_append_char('\x2');
		}
		delay_ms(2);

		// vykresleni staticke casti ramene
		//LCD_append_char('\x2');
		//delay_ms(2);

		// vykresleni horni casti lisu
		if(pressState == FULL)
		{
			LCD_append_char('\x1');
            delay_ms(2);
            LCD_append_char('\x1');
		}
		else
		{
			LCD_append_char('\x6');
            delay_ms(2);
            LCD_append_char('\x6');
		}
		delay_ms(2);

		LCD_append_char(' ');
		delay_ms(2);

		LCD_append_char(' ');
		delay_ms(2);

		for(i = 0; i < LCDParts; i++)
		{
			// vykresleni pasu FB
			if(FBDBCheckPosition(i * 5, (i + 1) * 5, FB_BELT))
			{
				LCD_append_char('\x1');
			}
			else
			{
				LCD_append_char('\x5');
			}
			delay_ms(2);
		}
        LCD_append_char(' ');
		delay_ms(2);
        
		// vykresleni pohyblive casti ramene
		if(POS == 0)
		{
			LCD_append_char('\x3');
		}
		else
		{
			LCD_append_char(' ');
		}
		delay_ms(2);

		// vykresleni spodni casti lisu
		if(pressState == FULL)
		{
			LCD_append_char('\x1');
            delay_ms(2);
            LCD_append_char('\x1');
		}
		else
		{
			LCD_append_char('\x6');
            delay_ms(2);
            LCD_append_char('\x6');
		}
		delay_ms(2);

		LCD_append_char(' ');
		delay_ms(2);

        LCD_append_char(' ');
		delay_ms(2);        
		
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

static void shiftTask(void *param)
{
    while(1)
    {
        // zpozdeni 1 s - simulace pohybu kusu materialu o 10 cm
        vTaskDelay(1000/portTICK_RATE_MS);
        // posun pasu FB a DB
        FBDBShift();
    }
}

/*******************************************************************************
 * misc functions
*******************************************************************************/
void print_user_help(void) 
{
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
}


void LCD_info(char str[LCD_CHAR_COUNT])
{
	/* send futher data to LCD */
	LCD_send_cmd(0x0c, 0);							 
	LCD_append_string(str);
}

/*******************************************************************************
 * inits preceding main() call
*******************************************************************************/
void fpga_initialized() 
{
	LCD_init();
	
	/* load usr characters (FreeRTOS) to LCD */
	LCD_load_char(1, charmap1);
	LCD_load_char(2, charmap2);
	LCD_load_char(3, charmap3);
	LCD_load_char(4, charmap4);
	LCD_load_char(5, charmap5);
    LCD_load_char(6, charmap6);

	/* welcome text */
	LCD_info("Press A to insert a material.");
}

/*******************************************************************************
 * top level code
 *******************************************************************************/
int main( void ) 
{
	/*--- HW init ---*/
	initialize_hardware();
	WDG_stop();

	term_send_crlf();
    
    //lcdTask();
    // TODO: nastavit parametry tasku
    /*--- install FreeRTOS tasks ---*/
	term_send_str_crlf("FreeRTOS: Inicializace vlaken FreeRTOS.");
	xTaskCreate(terminalTask /* code */, "Terminal task" /* name */, 500 /* stack size */, NULL /* params */, 1 /* prio */, NULL /* handle */);
	xTaskCreate(keyboardTask, "Keyboard task", 32, NULL, 1, NULL);

	xTaskCreate(robotTask, "Robot task", 80, NULL, 1, NULL);
	xTaskCreate(shiftTask, "Shift task", 50, NULL, 1, NULL);
	xTaskCreate(pressTask, "Press task", 50, NULL, 1, NULL);
	xTaskCreate(LCDTask, "LCD task", 100, NULL, 1, NULL);


	/*--- start FreeRTOS kernel ---*/
        term_send_str_crlf("FreeRTOS: Spusteni planovace FreeRTOS.");
	vTaskStartScheduler();

	return 0;
}
