//***********************************************************
//* glcd_menu.c
//***********************************************************

//***********************************************************
//* Includes
//***********************************************************

#include "compiledefs.h"
#include <avr/pgmspace.h> 
#include <stdlib.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include "io_cfg.h"
#include "vbat.h"
#include "adc.h"
#include "init.h"
#include "eeprom.h"
#include "acc.h"
#include "rc.h"
#include "mugui.h"
#include "glcd_driver.h"
#include "main.h"

//************************************************************
// Prototypes
//************************************************************

// Print an indexed text string from Program memory at a particular location
void LCD_Display_Text (uint16_t menuitem, const unsigned char* font,uint16_t x, uint16_t y);

// Print a string from at a particular location
void gLCDprint_Menu_P(const char *s, const unsigned char* font,uint16_t x, uint16_t y);

// Misc
void idle_screen(void);

//************************************************************
// Text to print (non-menu)
//************************************************************

//															// Status menu
const char StatusText0[]  PROGMEM = "Version: 1.5";			// <-- Change version number here !!!
const char StatusText1[]  PROGMEM = "Mode:";
const char StatusText3[]  PROGMEM = "Profile:";
const char StatusText4[]  PROGMEM = ".";
const char StatusText5[]  PROGMEM = "0";	
const char StatusText7[]  PROGMEM = "Battery:";
const char StatusText8[]  PROGMEM = "Pos:";
const char StatusText9[]  PROGMEM = "Jitter:";
//
const char MenuFrame0[] PROGMEM = "A"; 						// Down marker
const char MenuFrame2[] PROGMEM = "B";						// Right
const char MenuFrame3[] PROGMEM = "C";						// Left
const char MenuFrame4[] PROGMEM = "D";						// Cursor
const char MenuFrame1[] PROGMEM = "E";						// Up
const char MenuFrame5[] PROGMEM = "Menu";					// Menu
const char MenuFrame6[] PROGMEM = "Back";					// Back
const char MenuFrame7[] PROGMEM = "Def.";					// Default
const char MenuFrame8[] PROGMEM = "Save";					// Save
const char MenuFrame9[] PROGMEM = "Abort";					// Abort
//
const char MainMenuItem0[]  PROGMEM = "1. General"; 		// Main menu list text
const char MainMenuItem1[]  PROGMEM = "2. Receiver setup";
const char MainMenuItem9[]  PROGMEM = "3. Receiver inputs";
const char MainMenuItem7[]  PROGMEM = "4. Stick polarity";
const char MainMenuItem8[]  PROGMEM = "5. Sensor calibration";
const char MainMenuItem10[] PROGMEM = "6. Level meter";
const char MainMenuItem2[]  PROGMEM = "7. Flight profile 1";
const char MainMenuItem3[]  PROGMEM = "8. Flight profile 2";
const char MainMenuItem30[] PROGMEM = "9. Curves";
const char MainMenuItem25[] PROGMEM = "10. Output offsets";
const char MainMenuItem11[] PROGMEM = "11. OUT1 Mixer";
const char MainMenuItem12[] PROGMEM = "12. OUT2 Mixer";
const char MainMenuItem13[] PROGMEM = "13. OUT3 Mixer";
const char MainMenuItem14[] PROGMEM = "14. OUT4 Mixer";
const char MainMenuItem15[] PROGMEM = "15. OUT5 Mixer";
const char MainMenuItem16[] PROGMEM = "16. OUT6 Mixer";
const char MainMenuItem17[] PROGMEM = "17. OUT7 Mixer";
const char MainMenuItem18[] PROGMEM = "18. OUT8 Mixer";
const char MainMenuItem20[] PROGMEM = "19. Servo direction";
const char MainMenuItem22[] PROGMEM = "20. Neg. Servo trvl. (%)";
const char MainMenuItem23[] PROGMEM = "21. Pos. Servo trvl. (%)";
const char MainMenuItem32[] PROGMEM = "22. Custom Ch. order";
const char MainMenuItem31[] PROGMEM = "23. In/Out display";
const char MainMenuItem24[] PROGMEM = "24. Error log";
//
const char PText15[] PROGMEM = "Gyro";		 				// Sensors text
const char PText16[] PROGMEM = "Roll";
const char PText17[] PROGMEM = "Pitch";
const char PText18[] PROGMEM = "Yaw";
const char PText19[] PROGMEM = "Acc";
const char PText20[] PROGMEM = "IMU";
const char SensorMenuItem1[]  PROGMEM = "Cal.";	
const char SensorMenuItem2[]  PROGMEM = "Inv.";	
//
const char AutoMenuItem1[]  PROGMEM = "Roll P:";			// Profile
const char StabMenuItem2[]  PROGMEM = "Roll I:"; 
const char StabMenuItem10[]  PROGMEM = "Roll I limit:"; 
const char StabMenuItem3[]  PROGMEM =  "Roll I rate:";
const char AutoMenuItem20[]  PROGMEM = "Roll autolevel:";	
const char AutoMenuItem200[]  PROGMEM = "Yaw autolevel:";
const char AutoMenuItem7[]  PROGMEM = "Roll trim:";
const char AutoMenuItem4[]  PROGMEM = "Pitch P:";
const char StabMenuItem5[]  PROGMEM = "Pitch I:";
const char StabMenuItem11[]  PROGMEM = "Pitch I limit:";
const char StabMenuItem6[]  PROGMEM =  "Pitch I rate:"; 
const char AutoMenuItem21[]  PROGMEM = "Pitch autolevel:";
const char AutoMenuItem8[]  PROGMEM = "Pitch trim:";
const char StabMenuItem7[]  PROGMEM = "Yaw P:"; 
const char StabMenuItem8[]  PROGMEM = "Yaw I:";
const char StabMenuItem12[]  PROGMEM = "Yaw I limit:";
const char StabMenuItem9[]  PROGMEM =  "Yaw I rate:";
const char StabMenuItem30[]  PROGMEM = "Yaw trim:";
const char StabMenuItem13[]  PROGMEM = "Z-axis P:";		
const char StabMenuItem131[]  PROGMEM = "Z-axis I:";	
const char StabMenuItem132[]  PROGMEM = "Z-axis I limit:";	
//
const char AutoMenuItem11[] PROGMEM = "OFF";
const char AutoMenuItem15[] PROGMEM = "ON"; 
//
const char RCMenuItem1[]  PROGMEM = "RX type:";				// RC setup text
const char RCMenuItem20[] PROGMEM = "PWM sync:"; 	
const char RCMenuItem0[]  PROGMEM = "Ch. order:"; 	
const char RCMenuItem2[]  PROGMEM = "Profile chan.:";
const char Transition[] PROGMEM = "Transition:";
const char Transition_P1[] PROGMEM = "Transition low:";
const char Transition_P1n[] PROGMEM = "Transition mid:";
const char Transition_P2[] PROGMEM = "Transition high:";
const char RCMenuItem30[] PROGMEM = "Vibration display:";
const char TransitionOut[] PROGMEM = "Outbound trans.:";
const char TransitionIn[] PROGMEM = "Inbound trans.:";
const char RCMenuItem300[] PROGMEM = "AccVert filter:";
//
const char RXMode0[]  PROGMEM = "CPPM"; 					// RX mode text
const char RXMode1[]  PROGMEM = "PWM";
const char RXMode2[]  PROGMEM = "S-Bus";
const char RXMode3[]  PROGMEM = "Spektrum";
const char RXMode4[]  PROGMEM = "Xtreme";
const char RXMode5[]  PROGMEM = "Mode B/UDI";
const char RXMode6[]  PROGMEM = "HoTT SUMD";
//
const char RCMenuItem6[]  PROGMEM = "JR/Spektrum"; 			// Channel order
const char RCMenuItem7[]  PROGMEM = "Futaba"; 
const char RCMenuItem8[]  PROGMEM = "Multiplex"; 
const char RCMenuItem9[]  PROGMEM = "Custom"; 
//
const char MixerMenuItem0[]  PROGMEM = "Orientation:";		// General text
const char GeneralText100[]  PROGMEM = "Tail sitter:";		// �No�, �Earth ref.�, and �Vert AP ref.�
const char Contrast[]  PROGMEM = "Contrast:";
const char AutoMenuItem2[]  PROGMEM = "Safety:";
const char GeneralText2[]  PROGMEM = "Disarm time:";
const char GeneralText3[]  PROGMEM = "PWM rate:";
const char GeneralText6[] PROGMEM =  "Acc. LPF:";
const char GeneralText16[] PROGMEM =  "Gyro LPF:";
const char GeneralText7[] PROGMEM =  "AL correct:";
const char BattMenuItem2[]  PROGMEM = "Low V alarm:";
const char GeneralText20[] PROGMEM =  "Preset:";
const char GeneralText21[] PROGMEM =  "Buzzer:";
//
const char MixerMenuItem1[]  PROGMEM = "P1 orientn.:";		// Advanced text
const char MixerMenuItem8[]  PROGMEM = "P1 refrnce.:";
const char AdvancedDummy0[]  PROGMEM = " ";
const char AdvancedDummy1[]  PROGMEM = " ";

//
const char P1text[] PROGMEM = "P1";
const char P2text[] PROGMEM = "P1.n";
const char P3text[] PROGMEM = "P2";
const char P4text[] PROGMEM = "P1 - P1.n";
const char P5text[] PROGMEM = "P1.n - P2";
//
// Mixer menu items
const char MixerItem1[]  PROGMEM = "Device:";
const char MixerItem20[] PROGMEM = "P1 Offset:";
const char MixerItem36[] PROGMEM = "P1.n % of trans:";
const char MixerItem35[] PROGMEM = "P1.n Offset:";
const char MixerItem34[] PROGMEM = "P2 Offset:";
const char MixerItem23[] PROGMEM = "P1 Thr. volume:";
const char MixerItem33[] PROGMEM = "P2 Thr. volume:";
const char Mixeritem50[] PROGMEM = "Trans. curve:";
//
const char MixerItem51[] PROGMEM = "P1 Ail. volume:";
const char MixerItem54[] PROGMEM = "P2 Ail. volume:";
const char MixerItem52[] PROGMEM = "P1 Ele. volume:";
const char MixerItem55[] PROGMEM = "P2 Ele. volume:";
const char MixerItem53[] PROGMEM = "P1 Rud. volume:";
const char MixerItem56[] PROGMEM = "P2 Rud. volume:";
//
const char MixerItem4[]  PROGMEM = "P1 Roll gyro:";
const char MixerItem24[] PROGMEM = "P2 Roll gyro:"; 
const char MixerItem5[]  PROGMEM = "P1 Pitch gyro:";
const char MixerItem25[] PROGMEM = "P2 Pitch gyro:";
const char MixerItem6[]  PROGMEM = "P1 Yaw gyro:";
const char MixerItem26[] PROGMEM = "P2 Yaw gyro:";
const char MixerItem7[]  PROGMEM = "P1 Roll AL:";
const char MixerItem27[] PROGMEM = "P2 Roll AL:";
const char MixerItem700[]  PROGMEM = "P1 Yaw AL:";
const char MixerItem3[]  PROGMEM = "P1 Pitch AL:";
const char MixerItem28[] PROGMEM = "P2 Pitch AL:";
const char MixerItem42[] PROGMEM = "P1 Alt. damp:";
const char MixerItem43[] PROGMEM = "P2 Alt. damp:";
//
const char MixerItem0[]  PROGMEM = "P1 Source A:";			
const char MixerItem2[]  PROGMEM = "P1 Volume:";
const char MixerItem29[] PROGMEM = "P2 Source A:";			
const char MixerItem30[] PROGMEM = "P2 Volume:";
const char MixerItem21[] PROGMEM = "P1 Source B:";
const char MixerItem31[] PROGMEM = "P2 Source B:";
//
// Generic sensors for universal mixer
const char MixerItem70[] PROGMEM = "GyroRoll"; 
const char MixerItem71[] PROGMEM = "GyroPitch";
const char MixerItem72[] PROGMEM = "GyroYaw";
const char MixerItem73[] PROGMEM = "AccRoll";
const char MixerItem730[] PROGMEM = "AccYaw";
const char MixerItem74[] PROGMEM = "AccPitch";

const char MixerItemP73[] PROGMEM = "Acc X";				// Alternate text for universal accs
const char MixerItemP74[] PROGMEM = "Acc Y";
const char MixerItemP730[] PROGMEM = "Acc Z";

const char MixerItem80[] PROGMEM = "AL Roll";
const char MixerItem81[] PROGMEM = "AL Pitch";
const char MixerItem800[] PROGMEM = "AL Yaw";
const char MixerItem420[] PROGMEM = "Alt. damp";

//
const char MixerItem40[] PROGMEM = "A.Servo";
const char MixerItem41[] PROGMEM = "D.Servo";
const char MixerItem49[] PROGMEM = "Motor";
const char MixerItem60[] PROGMEM = "Linear";
const char MixerItem61[] PROGMEM = "Sine";
const char MixerItem62[] PROGMEM = "SqrtSine";
//
const char ChannelRef0[] PROGMEM = "Throttle";				// RC channel text
const char ChannelRef1[] PROGMEM = "Aileron"; 
const char ChannelRef2[] PROGMEM = "Elevator"; 
const char ChannelRef3[] PROGMEM = "Rudder"; 
const char ChannelRef4[] PROGMEM = "Gear"; 
const char ChannelRef5[] PROGMEM = "AUX1"; 
const char ChannelRef6[] PROGMEM = "AUX2"; 
const char ChannelRef7[] PROGMEM = "AUX3"; 
const char ChannelRef8[] PROGMEM = "None";
const char ChannelRef10[] PROGMEM = "Throt.";				// RC channel text (Abbr.)
const char ChannelRef12[] PROGMEM = "Elev."; 
//
const char ErrorText3[] PROGMEM = "No";						// Error text
const char ErrorText4[] PROGMEM = "Signal";	
const char ErrorText5[] PROGMEM = "Error";
const char ErrorText6[] PROGMEM = "Lost";
const char ErrorText7[] PROGMEM = "Model";
const char Status3[] 	PROGMEM = "Battery";
const char ErrorText10[] PROGMEM =  "Low";
const char Disarmed[]	PROGMEM = "Disarmed";
const char ErrorText8[] PROGMEM = "Brownout";
const char ErrorText9[] PROGMEM = "Occurred";
//
const char PText0[]  PROGMEM = "OpenAero2";					// Init
const char PText1[]  PROGMEM = "Reset";	
const char PText2[]  PROGMEM = "Hold steady";
const char PText3[]  PROGMEM = "ESC calibrate";
const char PText4[]  PROGMEM = "Cal. failed";
const char PText5[]  PROGMEM = "Updating";
const char PText6[]  PROGMEM = "settings";
//
const char WizardText0[] PROGMEM = "No RX signal"; 			// Wizard screen
const char WizardText1[] PROGMEM = "Hold as shown";
const char WizardText2[] PROGMEM = "Done";
//
const char Status0[] PROGMEM = "Press";						// Idle text
const char Status2[] PROGMEM = "for status.";
const char Status4[] PROGMEM = "(Armed)";
const char Status5[] PROGMEM = "(Disarmed)";
//
const char MOUT1[] PROGMEM = "OUT1";						// Outputs
const char MOUT2[] PROGMEM = "OUT2";	
const char MOUT3[] PROGMEM = "OUT3";	
const char MOUT4[] PROGMEM = "OUT4";	
const char MOUT5[] PROGMEM = "OUT5";	
const char MOUT6[] PROGMEM = "OUT6";	
const char MOUT7[] PROGMEM = "OUT7";	
const char MOUT8[] PROGMEM = "OUT8";	
//
const char MixerMenuItem2[]  PROGMEM = "Forward";			// Orientation
const char MixerMenuItem3[]  PROGMEM = "Vertical";
const char MixerMenuItem4[]  PROGMEM = "Inverted";
const char MixerMenuItem5[]  PROGMEM = "Aft";
const char MixerMenuItem6[]  PROGMEM = "Sideways";
const char MixerMenuItem7[]  PROGMEM = "PitchUp";

const char Orientation1[]  PROGMEM = "Top-Rear";			// 295 (24) Orientations
const char Orientation2[]  PROGMEM = "Top-Left";
const char Orientation3[]  PROGMEM = "Top-Front";
const char Orientation4[]  PROGMEM = "Top-Right";
const char Orientation5[]  PROGMEM = "Back-Down";
const char Orientation6[]  PROGMEM = "Back-Left";
const char Orientation7[]  PROGMEM = "Back-Up";
const char Orientation8[]  PROGMEM = "Back-Right";
const char Orientation9[]  PROGMEM = "Bott.-Back";
const char Orientation10[]  PROGMEM = "Bott.-Right";
const char Orientation11[]  PROGMEM = "Bott.-Front";
const char Orientation12[]  PROGMEM = "Bott.-Left";
const char Orientation13[]  PROGMEM = "Front-Down";
const char Orientation14[]  PROGMEM = "Front-Right";
const char Orientation15[]  PROGMEM = "Front-Up";
const char Orientation16[]  PROGMEM = "Front-Left";
const char Orientation17[]  PROGMEM = "Left-Down";
const char Orientation18[]  PROGMEM = "Left-Front";
const char Orientation19[]  PROGMEM = "Left-Up";
const char Orientation20[]  PROGMEM = "Left-Back";
const char Orientation21[]  PROGMEM = "Right-Down";
const char Orientation22[]  PROGMEM = "Right-Back";
const char Orientation23[]  PROGMEM = "Right-Up";
const char Orientation24[]  PROGMEM = "Right-Front";
//
const char MixerItem11[] PROGMEM = "Normal";				// Misc
const char MixerItem12[] PROGMEM = "Reversed";
const char MixerItem15[] PROGMEM = "Scaled";
const char MixerItem16[] PROGMEM = "RevScaled";
//
const char GeneralText5[] PROGMEM  =  "Sync RC";			// PWM output modes
//
const char Safety1[] PROGMEM =  "Armed";
const char Safety2[] PROGMEM =  "Armable";
const char Random1[] PROGMEM =  "High";
const char Random10[] PROGMEM =  "high";
const char Random11[] PROGMEM =  "low";
const char Random12[] PROGMEM =  "signal";
//
//
const char SWLPF1[] PROGMEM =  "67Hz";						// Software LPFs
const char SWLPF2[] PROGMEM =  "38Hz";
const char SWLPF3[] PROGMEM =  "27Hz";
const char SWLPF4[] PROGMEM =  "17Hz";

//
const char GeneralText10[] PROGMEM =  "MPU6050 LPF:";

const char MPU6050LPF1[] PROGMEM =  "5Hz";
const char MPU6050LPF2[] PROGMEM =  "10Hz";
const char MPU6050LPF3[] PROGMEM =  "21Hz";
const char MPU6050LPF4[] PROGMEM =  "44Hz";
const char MPU6050LPF5[] PROGMEM =  "94Hz";
const char MPU6050LPF6[] PROGMEM =  "184Hz";
const char MPU6050LPF7[] PROGMEM =  "260Hz";
//
const char Dummy0[] PROGMEM = "";
//
const char VBAT32[] PROGMEM =  "3.2V";
const char VBAT33[] PROGMEM =  "3.3V";
const char VBAT34[] PROGMEM =  "3.4V";
const char VBAT35[] PROGMEM =  "3.5V";
const char VBAT36[] PROGMEM =  "3.6V";
const char VBAT37[] PROGMEM =  "3.7V";
const char VBAT38[] PROGMEM =  "3.8V";
const char VBAT39[] PROGMEM =  "3.9V";
//
// Preset names
const char PRESET_0[] PROGMEM =  "Blank";
const char PRESET_1[] PROGMEM =  "Quad X";
const char PRESET_2[] PROGMEM =  "Quad P";
const char PRESET_3[] PROGMEM =  "Tricopter";
const char PRESET_4[] PROGMEM =  "Options";

const char WARNING_0[] PROGMEM =  "WARNING - Erases settings";
const char WARNING_1[] PROGMEM =  "REMOVE PROPS";

const char ERROR_0[] PROGMEM =  "Reboot";
const char ERROR_1[] PROGMEM =  "Manual disarm";
const char ERROR_2[] PROGMEM =  "No signal";
const char ERROR_3[] PROGMEM =  "Disarm timer";
const char ERROR_4[] PROGMEM =  "BOD reset";

const char ERROR_MSG_0[] PROGMEM =  "Clear";

//const char Misc_Msg01[]  PROGMEM = "Alt. Damp:";
const char Misc_Msg01[]  PROGMEM = "Alt.";

// Reference
const char Reference_0[] PROGMEM =  "Vert. AP ref.";		// �No�, �Earth ref.�, and �Vert AP ref.� ErrorText3 = "No"
const char Reference_1[] PROGMEM =  "Earth ref.";

// Curves
const char Curve_0[] PROGMEM =  "P1 Throttle Curve";
const char Curve_1[] PROGMEM =  "P2 Throttle Curve";
const char Curve_2[] PROGMEM =  "P1 Collective Curve";
const char Curve_3[] PROGMEM =  "P2 Collective Curve";
const char Curve_4[] PROGMEM =  "Generic Curve C";
const char Curve_5[] PROGMEM =  "Generic Curve D";

// Misc
const char Misc_1[] PROGMEM =  "CRC:";

// Short channel names
const char Chan0[] PROGMEM = "THR";							// RC channel text
const char Chan1[] PROGMEM = "AIL";
const char Chan2[] PROGMEM = "ELE";
const char Chan3[] PROGMEM = "RUD";
const char Chan4[] PROGMEM = "GEAR";
const char Chan5[] PROGMEM = "AUX1";
const char Chan6[] PROGMEM = "AUX2";
const char Chan7[] PROGMEM = "AUX3";

// New universal mixer items
const char Uni1[] PROGMEM = "Collective";
const char Uni3[] PROGMEM = "Curve C";
const char Uni4[] PROGMEM = "Curve D";

// IO
const char I1[] PROGMEM = "1:";
const char I2[] PROGMEM = "2:";
const char I3[] PROGMEM = "3:";
const char I4[] PROGMEM = "4:";
const char I5[] PROGMEM = "5:";
const char I6[] PROGMEM = "6:";
const char I7[] PROGMEM = "7:";
const char I8[] PROGMEM = "8:";
const char I9[] PROGMEM = "Pn:";
const char I10[] PROGMEM = "OUT:";

const char O0[] PROGMEM = "THR:";
const char O1[] PROGMEM = "AIL:";
const char O2[] PROGMEM = "ELE:";
const char O3[] PROGMEM = "RUD:";
const char O4[] PROGMEM = "GER:";
const char O5[] PROGMEM = "AX1:";
const char O6[] PROGMEM = "AX2:";
const char O7[] PROGMEM = "AX3:";

// Custom channel inputs
const char Ch1[] PROGMEM = "TX chan. 1";
const char Ch2[] PROGMEM = "TX chan. 2";
const char Ch3[] PROGMEM = "TX chan. 3";
const char Ch4[] PROGMEM = "TX chan. 4";
const char Ch5[] PROGMEM = "TX chan. 5";
const char Ch6[] PROGMEM = "TX chan. 6";
const char Ch7[] PROGMEM = "TX chan. 7";
const char Ch8[] PROGMEM = "TX chan. 8";

const char* const text_menu[] PROGMEM = 
	{
		AutoMenuItem11, VBAT32, VBAT33, VBAT34,	VBAT35, VBAT36, VBAT37, VBAT38, VBAT39,		// 0 to 8 Vbat cell voltages
		//
		MenuFrame0, MenuFrame1, MenuFrame2, MenuFrame3, MenuFrame4, MenuFrame5, 			// 9 to 17 Menu frame text
		MenuFrame6, MenuFrame7, MenuFrame8, 
		//
		Disarmed, ErrorText5,ErrorText6, ErrorText7,										// 18 to 21, Disarmed, Error, lost, model
		//
		Transition,																			// 22 Misc
		StatusText8, 
		StatusText9, 
		SensorMenuItem2,		
		//
		PText15, PText16, PText17, PText18, PText19, PText20, 								// 26 to 31 Sensors
		//
		ChannelRef1, ChannelRef2, ChannelRef0, ChannelRef3, ChannelRef4,					// 32 to 36 RC inputs
		//
		MPU6050LPF1, MPU6050LPF2, MPU6050LPF3, MPU6050LPF4,
		MPU6050LPF5, MPU6050LPF6, MPU6050LPF7, ChannelRef8,									// 37 to 44  MPU6050 LPF, 5Hz to 260Hz + None
		//
		Dummy0, 																			// 45 Spare 
		//
		MixerItem40, MixerItem41,															// 46 to 47 Device types - Servo/Motor													// 
		// 
		P1text, P2text, P3text, 															// 48 to 52 P1, P1.n, P2, P1 to P1.n, P1.n to P2
		P4text, P5text, 
		
		Safety1, 																			// 53, 54 Safety  
		Safety2,																			// 
		//
		Random1,  																			// 55 High
		//
		MixerItem60, MixerItem61, 															// 56 to 58 Linear, Sine, Sqrt Sine
		MixerItem62,
		
		//
		PText3,																				// 59 ESC Calibrate
		//
		SensorMenuItem1,																	// 60 calibrate
		//
		PText4, 																			// 61 Failed
		//
		ErrorText8, ErrorText9,																// 62, 63, "Brownout", "Occurred"
		//
		Dummy0, Dummy0,																		// 64 to 67 spare
		Dummy0, Dummy0,
		//
		AutoMenuItem11, AutoMenuItem15, MixerItem15, MixerItem12, MixerItem16,				// 68 to 71 off/on/scale/rev/revscale 
		//
		Misc_1, Dummy0,																		// 73 CRC:
		//
		ErrorText3, ErrorText4,																// 75 to 76 Error messages
		//
		MainMenuItem0, MainMenuItem1, MainMenuItem9, MainMenuItem7, MainMenuItem8, 
		MainMenuItem10, MainMenuItem2, MainMenuItem3,MainMenuItem30,						// 77 to 100 Main menu
		MainMenuItem25, MainMenuItem11,MainMenuItem12,MainMenuItem13,MainMenuItem14,
		MainMenuItem15,MainMenuItem16,MainMenuItem17,MainMenuItem18,		
		MainMenuItem20,MainMenuItem22, MainMenuItem23,MainMenuItem32, MainMenuItem31, 
		MainMenuItem24, 
		//
		Dummy0, Dummy0,																		// 101 to 104 Spare
		Dummy0, Dummy0,
		//
		ChannelRef0, ChannelRef1, ChannelRef2, ChannelRef3, ChannelRef4, 					// 105 to 115 Ch. names
		ChannelRef5, ChannelRef6, ChannelRef7, ChannelRef8,		
		ChannelRef10, ChannelRef12, 														// 114, 115 Ch.ref abbreviations
		//
		Dummy0, Dummy0, 																	// 116 to 117 Spare
		//
		ErrorText10, GeneralText5, Random1,													// 118 to 120 Low, Sync RC, High
		//
		Status0, Status2, Dummy0,															// 121 to 123 Press any button
		//
		MixerMenuItem2, MixerMenuItem3,	MixerMenuItem4,										// 124 to 129 H/V/UD/Aft/Sideways/PitchUp
		MixerMenuItem5, MixerMenuItem6,	MixerMenuItem7,
		//																
		RCMenuItem6, RCMenuItem7, RCMenuItem8, RCMenuItem9,									// 130 to 133 JR/Futaba/MPX/CUSTOM
		//
		Status3,																			// 134 Battery
		//
		WizardText0,WizardText1,WizardText2,												// 135 to 137
		//
		Status4,Status5,Dummy0,																// 138 to 140
		//
		MixerItem11,MixerItem12,															// 141 to 142 Norm/Rev
		//
		PText16,PText17,PText18,															// 143 to 145 X/Y/Z
		//
		RCMenuItem1, GeneralText3, RCMenuItem20, RCMenuItem0, RCMenuItem2, 					// 146 to 157 RC menu
		TransitionOut, TransitionIn, Transition_P1, Transition_P1n,
		Transition_P2, RCMenuItem30, RCMenuItem300,
		//
		MixerMenuItem0, GeneralText100, Contrast, AutoMenuItem2,							// 158 to 169 General
		GeneralText2, BattMenuItem2, GeneralText10, 
		GeneralText6, GeneralText16, GeneralText7, 
		GeneralText20, GeneralText21,
		//
		Dummy0, Dummy0,																		// 170 to 171 Spare
		//
		// Special Model reference text
		//
		//				 																	// 172 to 191 Flight menu (Earth)
		AutoMenuItem1, StabMenuItem2, StabMenuItem10, StabMenuItem3,						// Roll gyro
		AutoMenuItem20, AutoMenuItem7,														// Roll acc
		AutoMenuItem4, StabMenuItem5, StabMenuItem11, StabMenuItem6, 						// Pitch gyro
		AutoMenuItem21, AutoMenuItem8, 														// Pitch acc
		StabMenuItem7, StabMenuItem8, StabMenuItem12, StabMenuItem9,	 					// Yaw gyro
		StabMenuItem30,	StabMenuItem13,														// Yaw trim, Z-Acc, 
		StabMenuItem131, StabMenuItem132,													// Z-I, Z-I limit
		//
		// Special Model reference texts --->
		//
		//																					// Mixer items (Earth)
		MixerItem1,																			// 192 Motor marker (30 mixer items in total)
		MixerItem23,  																		// P1 Throttle
		MixerItem33, 																		// P2 Throttle
		Mixeritem50,																		// Throttle curve
		//	
		//																					// 196 to 201 mixers P1 + P2
		MixerItem51, MixerItem54,															// Aileron volume P1 + P2
		MixerItem52, MixerItem55,															// Elevator volume P1 + P2
		MixerItem53, MixerItem56,															// Rudder volume P1 + P2
		//
		MixerItem4, MixerItem24, MixerItem5, MixerItem25, MixerItem6, MixerItem26, 			// 202 Gyros and Acc P1 + P2
		MixerItem7, MixerItem27, MixerItem3, MixerItem28, MixerItem42,MixerItem43,
		//
		MixerItem0, MixerItem2, 															// 214 Source A and Volume P1
		MixerItem29, MixerItem30, 															// Source A and Volume P2
		MixerItem21, MixerItem2, 															// Source B and Volume P1
		MixerItem31, MixerItem30, 															// Source B and Volume P2
		Dummy0, Dummy0,
		//
		// <--- Special Model reference texts
		//

		Dummy0,Dummy0,																		// 224, 225
		
		MixerItem40, MixerItem41, MixerItem49,												// 226 to 228 Device types - AServo/Dservo/Motor	
		//
		Misc_Msg01,																			// 229 AccVert
		//
		MOUT1, MOUT2, MOUT3, MOUT4, MOUT5, MOUT6, MOUT7, MOUT8, 							// 230 to 237 Sources OUT1- OUT8, 

		Dummy0,Dummy0,Dummy0,Dummy0,Dummy0,													// Spare
		Dummy0,Dummy0,Dummy0,Dummy0,Dummy0,													// Spare
		Dummy0,Dummy0,Dummy0,Dummy0,Dummy0,													// Spare
		
		ChannelRef8, 																		// 253 + NONE 
		//
		MixerItem40, MixerItem41, MixerItem49,												// 254 to 256 Device types - AServo/Dservo/Motor	
		Dummy0, Dummy0,																		// 257, 258
		//
		PText5, PText6,																		// 259, 260 Updating settings
		//
		PText0,	PText1, PText2, 															// 261 Logo 262 Reset 263 Hold steady
		//
		StatusText0, StatusText1, RCMenuItem1, StatusText3, StatusText4, StatusText5,		// 264 to 269 Status menu	
		Random10, Random11, Random12,														// 270 - 272
		//	
		PRESET_1, PRESET_2,																	// 273 to 279 Mixer presets
		PRESET_3, PRESET_0, PRESET_4, Dummy0, Dummy0,	
		//
		MenuFrame9,																			// 280 Abort
		WARNING_0,	WARNING_1,																// 281 Warnings
		
		Dummy0,ERROR_0,ERROR_1,ERROR_2,ERROR_3,ERROR_4,										// 283 Error messages

		
		StatusText7,																		// 289 Battery:
		Dummy0,	
		
		ERROR_MSG_0,																		// 291 - 292 Log menu
		Dummy0, 	
		
		Dummy0, Dummy0,	
				
		Orientation1, Orientation2, Orientation3, Orientation4,								// 295 (24) Orientations
		Orientation5, Orientation6, Orientation7, Orientation8,
		Orientation9, Orientation10, Orientation11, Orientation12,
		Orientation13, Orientation14, Orientation15, Orientation16,
		Orientation17, Orientation18, Orientation19, Orientation20,
		Orientation21, Orientation22, Orientation23, Orientation24,
		Dummy0,	

		ErrorText3, Reference_1, Reference_0, Dummy0,										// 320 Orientation reference (No, Earth, Vert AP)
		MixerMenuItem1, MixerMenuItem8,	AdvancedDummy0, AdvancedDummy1,						// 324 Advanced menu

		//	
		// Special Model reference text
		//			 																		// 328 to 345 Flight menu (Model)
		AutoMenuItem1, StabMenuItem2, StabMenuItem10, StabMenuItem3,						// Roll gyro
		AutoMenuItem200, AutoMenuItem7,														// Roll acc
		AutoMenuItem4, StabMenuItem5, StabMenuItem11, StabMenuItem6, 						// Pitch gyro
		AutoMenuItem21, AutoMenuItem8, 														// Pitch acc
		StabMenuItem7, StabMenuItem8, StabMenuItem12, StabMenuItem9,	 					// Yaw gyro
		StabMenuItem30,	StabMenuItem13,														// Yaw trim, Z-Acc,
		StabMenuItem131, StabMenuItem132,													// Z-I, Z-I limit
		
		//
		// Special Model reference texts --->
		//
		//																					// Mixer items (Model)
		MixerItem1,																			// 348 Motor marker (34 mixer items in total)
		MixerItem23,  																		// P1 Throttle
		MixerItem33, 																		// P2 Throttle
		Mixeritem50,																		// Throttle curve
		//																					// 352 to 357 mixers P1 + P2
		MixerItem51, MixerItem54,															// Aileron volume P1 + P2
		MixerItem52, MixerItem55,															// Elevator volume P1 + P2
		MixerItem53, MixerItem56,															// Rudder volume P1 + P2
		MixerItem4, MixerItem24, MixerItem5, MixerItem25, MixerItem6, MixerItem26, 			// 358 Gyros and Acc P1 + P2
		MixerItem700, MixerItem27, MixerItem3, MixerItem28, MixerItem42,MixerItem43,
		MixerItem0, MixerItem2, 															// 370 Source A and Volume P1 (Model)
		MixerItem29, MixerItem30, 															// Source A and Volume P2
		MixerItem21, MixerItem2, 															// Source B and Volume P1
		MixerItem31, MixerItem30, 															// Source B and Volume P2
		//
		// <--- Special Model reference texts
		//
		Dummy0,	Dummy0,	
		Dummy0, Dummy0,	Dummy0,	Dummy0,														// 378 to 383 Spare
		//
		Ch1, Ch2, Ch3, Ch4,																	// 384 to 391 TX channel numbers												
		Ch5, Ch6, Ch7, Ch8,		
		
		//	
		Dummy0, Dummy0, Dummy0, Dummy0, 													// 392 to 395 Spare
		
		//
		RXMode0, RXMode1, RXMode2, RXMode3,													// 396 to 402 RX mode
		RXMode4, RXMode5, RXMode6, 
		//
		Curve_0,Curve_1,Curve_2,Curve_3,Curve_4,Curve_5,Dummy0,								// 403 Curves
		//
		Chan0,Chan1,Chan2,Chan3,Chan4,Chan5,Chan6,Chan7,ChannelRef8,						// 410 Short channel names
		Dummy0,Dummy0,
	
		// Universal sources (EARTH)
		// THROTTLE, CURVE A, CURVE B, COLLECTIVE, AILERON, ELEVATOR, RUDDER, GEAR, AUX1, AUX2, AUX3,
		// ROLLGYRO, PITCHGYO, YAWGYRO, ROLLSMOOTH, PITCHSMOOTH, ROLLACC, PITCHACC, NONE
		Chan0,Uni3,Uni4,Uni1,																// 421 THR to Collective,
		Chan0,Chan1,Chan2,Chan3,Chan4,Chan5,Chan6,Chan7,									// Throttle, Aileron to AUX3
		MixerItem70, MixerItem71, MixerItem72, MixerItemP73, MixerItemP74,					// Roll gyro to pitch acc
		MixerItem80, MixerItem81, MixerItem420, ChannelRef8, 								// AL Roll, AL Pitch, Alt. Damp, None +1

		// Universal sources (MODEL)
		// THROTTLE, CURVE A, CURVE B, COLLECTIVE, AILERON, ELEVATOR, RUDDER, GEAR, AUX1, AUX2, AUX3,
		// ROLLGYRO, PITCHGYO, YAWGYRO, YAWSMOOTH, PITCHSMOOTH, YAWACC, PITCHACC, NONE
		Chan0,Uni3,Uni4,Uni1,																// 442 THR to Collective,
		Chan0, Chan1,Chan2,Chan3,Chan4,Chan5,Chan6,Chan7,									// Throttle, Aileron to AUX3
		MixerItem70, MixerItem71, MixerItem72, MixerItemP730, MixerItemP74,					// Roll gyro to pitch acc
		MixerItem800, MixerItem81, MixerItem420, ChannelRef8,								// AL Roll, AL Pitch, Alt. Damp, None +1
		//
		//MPU6050LPF1, MPU6050LPF2, SWLPF4, SWLPF3, SWLPF2,									// 461 to 467 SW LPF (7) 5, 10, 17, 27, 38, 67, None (NOT USED - DELETE?)
		//SWLPF1, ChannelRef8,
		Dummy0,Dummy0,Dummy0,Dummy0,Dummy0,													// Spare
		//
		I1, I2, I3, I4, I5, I6, I7, I8, I9, I10,											// 468 to 477
		//
		O0, O1, O2, O3, O4, O5, O6, O7,														// 478 to 485
	}; 

//************************************************************
// GLCD text subroutines
//************************************************************

// Print Menuitem from Program memory at a particular location
void LCD_Display_Text (uint16_t menuitem, const unsigned char* font,uint16_t x, uint16_t y)
{
	gLCDprint_Menu_P((char*)pgm_read_word(&text_menu[menuitem]), font, x, y);
}

// Print a string from RAM at a particular location in a particular font
void gLCDprint_Menu_P(const char *s, const unsigned char* font,uint16_t x, uint16_t y)
{
	pgm_mugui_lcd_puts((const unsigned char*)s, font, x, y);
}

// Pop up the Idle screen
void idle_screen(void)
{
	clear_buffer(buffer);

	// Change Status screen depending on arm mode
	LCD_Display_Text(121,(const unsigned char*)Verdana14,41,3); 		// "Press"
	LCD_Display_Text(122,(const unsigned char*)Verdana14,24,23);		// "for status."

	// Display most important error
	if (General_error & (1 << LVA_ALARM))								// Low voltage
	{
		LCD_Display_Text(134,(const unsigned char*)Verdana14,12,43);	// "Battery"
		LCD_Display_Text(118,(const unsigned char*)Verdana14,80,43);	// "Low"
	}
	
	else if (General_error & (1 << NO_SIGNAL))							// No signal
	{
		LCD_Display_Text(75,(const unsigned char*)Verdana14,28,43);		// "No"
		LCD_Display_Text(76,(const unsigned char*)Verdana14,54,43);		// "Signal"
	}
	
	else if (General_error & (1 << THROTTLE_HIGH))						// Throttle high
	{
		LCD_Display_Text(105,(const unsigned char*)Verdana14,10,43)	;	// "Throttle"
		LCD_Display_Text(55,(const unsigned char*)Verdana14,81,43);		// "High"
	}
	
	else if (General_error & (1 << DISARMED))							// Disarmed
	{
		LCD_Display_Text(139,(const unsigned char*)Verdana14,20,43);	// "(Disarmed)"
	}
	
	else
	{
		LCD_Display_Text(138,(const unsigned char*)Verdana14,28,43);	// "(Armed)"
	}

	write_buffer(buffer);
}
