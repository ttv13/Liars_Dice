#include "PmodOLEDrgb.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "akira.h"
#include "badapple.h"
#include "PmodKYPD.h"
#include "xtime_l.h"
#include <stdint.h>

#ifndef __bool_true_false_are_defined
typedef enum { false = 0, true = 1 } bool;
#endif

void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();
void playvid(const uint8_t (*video)[12288], int len, int *frame, bool *paused, int spd);

PmodOLEDrgb oledrgb;
PmodKYPD myDevice;

u8 rgbUserFont[] = {
   0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00, // 0x00
   0x0E, 0x1F, 0x15, 0x1F, 0x17, 0x10, 0x1F, 0x0E, // 0x01
   0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00, // 0x02
   0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, // 0x03
   0x07, 0x0C, 0xFA, 0x2F, 0x2F, 0xFA, 0x0C, 0x07  // 0x04
};

int main(void) {
   xil_printf("starting.\r\n");
   DemoInitialize();
   DemoRun();
   DemoCleanup();
   return 0;
}

void DemoInitialize() {
   xil_printf("cache enable start.\r\n");
   EnableCaches();
   xil_printf("cache enabled.\r\n");
   OLEDrgb_begin(&oledrgb, XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR,
         XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);
   xil_printf("OLED initialized.\r\n");
   KYPD_begin(&myDevice, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
   xil_printf("KYPD initialized.\r\n");
   KYPD_loadKeyTable(&myDevice, (u8*) "0FED789C456B123A");
   xil_printf("Key table loaded.\r\n");
}

void DemoRun() {
   char ch;
   xil_printf("in demorun.\r\n");
   for (ch = 0; ch < 5; ch++) {
      OLEDrgb_DefUserChar(&oledrgb, ch, &rgbUserFont[ch * 8]);
   }

   OLEDrgb_SetCursor(&oledrgb, 2, 1);
   OLEDrgb_PutString(&oledrgb, "AKIRA");
   OLEDrgb_SetCursor(&oledrgb, 4, 4);
   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(0, 0, 255));
   OLEDrgb_PutString(&oledrgb, "OledRGB");

   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(200, 200, 44));
   OLEDrgb_SetCursor(&oledrgb, 1, 6);
   OLEDrgb_PutChar(&oledrgb, 4);

   OLEDrgb_SetFontColor(&oledrgb, OLEDrgb_BuildRGB(200, 12, 44));
   OLEDrgb_SetCursor(&oledrgb, 5, 6);
   OLEDrgb_PutString(&oledrgb, ":)");
   OLEDrgb_PutChar(&oledrgb, 0);

   sleep(5);

   int badapple_len = sizeof(badapple) / sizeof(badapple[0]);
   int akira_len = sizeof(akira) / sizeof(akira[0]);
   int current_frame = 0;
   int spd = 2;
   bool paused = false;
   bool playing_badapple = true;
   bool playing_akira = false;
   XTime tStart, tEnd;
   const u64 KEYPAD_CHECK_INTERVAL = 50000;

   XTime lastKeyPressTime = 0;
   const u64 DEBOUNCE_TIME = 300000; // 300ms debounce time in microseconds
   u8 lastKeyPressed = 0;

   xil_printf("Starting main loop. Press 1 to pause/resume, 2 to switch video, 3 to exit.\r\n");

   XTime_GetTime(&tStart);

   while (1) {
      XTime_GetTime(&tEnd);
      u64 elapsed = (tEnd - tStart) / (COUNTS_PER_SECOND / 1000000);

      if (elapsed >= KEYPAD_CHECK_INTERVAL) {
         u16 keystate = KYPD_getKeyStates(&myDevice);
         u8 key;
         XStatus status = KYPD_getKeyPressed(&myDevice, keystate, &key);

         if (status == KYPD_SINGLE_KEY) {
            XTime currentTime;
            XTime_GetTime(&currentTime);
            u64 timeSinceLastPress = (currentTime - lastKeyPressTime) / (COUNTS_PER_SECOND / 1000000);

            if (key != lastKeyPressed || timeSinceLastPress > DEBOUNCE_TIME) {
               xil_printf("Key pressed: %c\r\n", key);
               lastKeyPressed = key;
               lastKeyPressTime = currentTime;

               switch (key) {
                  case '7':
                     paused = !paused;
                     xil_printf("Video %s\r\n", paused ? "paused" : "resumed");
                     break;
                  case '1':
                     playing_badapple = true;
                     playing_akira = false;
                     current_frame = 0;
                     xil_printf("Switched to bad apple video\r\n");
                     break;
                  case '2':
                	  playing_badapple = false;
                	  playing_akira = true;
                	  current_frame = 0;
                	  xil_printf("Switched to akira video\r\n");
                	  break;
                  case '8':
                     xil_printf("Restarting\r\n");
                     current_frame = 0;
                     break;
                  case '4':
                     if (spd < 4){
                        xil_printf("Speedup\r\n");
                        spd = spd * 2;
                     }
                     xil_printf("Spd = %d\r\n", spd);
                     break;
                  case '0':
                     if (spd > 1){
                        xil_printf("Slowed\r\n");
                        spd = spd/2;
                     }
                     xil_printf("Spd = %d\r\n", spd);
                     break;
                  case '5':
                	 current_frame = current_frame + 150;
                	 xil_printf("Frameskip 150", spd);
                	 break;
                  case 'F':
                	  current_frame = current_frame - 150;
                	  if (current_frame < 0){
                		  current_frame = 0;
                	  }
                	  xil_printf("Framegoback 150", spd);
                	  break;
               }
            }
         }
         XTime_GetTime(&tStart);
      }

      if (!paused) {
         if (playing_badapple) {
            playvid(badapple, badapple_len, &current_frame, &paused, spd);
         } else if (playing_akira){
            playvid(akira, akira_len, &current_frame, &paused, spd);
         }
      }

      usleep(1000);
   }
}

void playvid(const uint8_t (*video)[12288], int len, int *frame, bool *paused, int spd) {\
   if (*frame > len) {
	  *frame = 0;
   }
   OLEDrgb_DrawBitmap(&oledrgb, 0, 0, 95, 63, (u8*) video[*frame]);
   usleep(16666);
   (*frame) = (*frame) + (1*spd);
}

void DemoCleanup() {
   DisableCaches();
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#endif
}
