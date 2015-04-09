/* Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty. This file is offered as-is,
 * without any warranty.
 */

/*! @file process_frame.c
 * @brief Contains the actual algorithm and calculations.
 */

/* Definitions specific to this application. Also includes the Oscar main header file. */
#include "template.h"
#include <string.h>
#include <stdlib.h>

void CalcDeriv();
void AvgDeriv(int Index);

#define IMG_SIZE NUM_COLORS*(OSC_CAM_MAX_IMAGE_WIDTH/2)*(OSC_CAM_MAX_IMAGE_HEIGHT/2)

const int nc = OSC_CAM_MAX_IMAGE_WIDTH / 2;
const int nr = OSC_CAM_MAX_IMAGE_HEIGHT / 2;

int TextColor;
int avgDxy[3][IMG_SIZE];

void ResetProcess() {
	//called when "reset" button is pressed
	if (TextColor == CYAN)
		TextColor = MAGENTA;
	else
		TextColor = CYAN;
}

void ProcessFrame() {
	uint32 t1, t2;
	char Text[] = "hallo world";
	//initialize counters
	if (data.ipc.state.nStepCounter == 1) {
		//use for initialization; only done in first step
		memset(data.u8TempImage[THRESHOLD], 0, IMG_SIZE);
		TextColor = CYAN;
	} else {
		//example for time measurement
		t1 = OscSupCycGet();
		//example for copying sensor image to background image
		memcpy(data.u8TempImage[BACKGROUND], data.u8TempImage[SENSORIMG],
		IMG_SIZE);
		CalcDeriv();
		//example for time measurement
		t2 = OscSupCycGet();

		//example for log output to console
		OscLog(INFO, "required = %d us\n", OscSupCycToMicroSecs(t2 - t1));

		//example for drawing output
		//draw line
		DrawLine(10, 100, 200, 20, RED);
		//draw open rectangle
		DrawBoundingBox(20, 10, 50, 40, false, GREEN);
		//draw filled rectangle
		DrawBoundingBox(80, 100, 110, 120, true, BLUE);
		DrawString(200, 200, strlen(Text), TINY, TextColor, Text);
	}

}

void CalcDeriv() {
	int c, r;
	avgDxy = 0;

	for (r = nc; r < nr * nc - nc; r += nc) {/* we skip the first and last line */
		for (c = 1; c < nc - 1; c++) {
			/* do pointer arithmetics with respect to center pixel location */
			unsigned char* p = &data.u8TempImage[SENSORIMG][r + c];

			/* implement Sobel filter in x direction */
			int dx = -1 * (int) *(p - nc - 1) + 1 * (int) *(p - nc + 1)
					- 2 * (int) *(p - 1) + 2 * (int) *(p + 1)
					- 1 * (int) *(p + nc - 1) + 1 * (int) *(p + nc + 1);

			/* implement Sobel filter in y direction */
			int dy = -1 * (int) *(p - nc - 1) - 2 * (int) *(p - nc) - 1 * (int) *(p - nc + 1)
					+ 1 * (int) *(p + nc - 1) + 2 * (int) *(p + nc) + 1 * (int) *(p + nc + 1);

			//Division durch 4 (2xbitshift)
			data.u8TempImage[BACKGROUND][r + c] = (uint8) MIN(255,MAX(0, (dx*dx)>>2));
			data.u8TempImage[THRESHOLD][r + c] = (uint8) MIN(255, MAX(0, (dy*dy)>>2));

			avgDxy[0][r + c] = dx * dx;
			avgDxy[1][r + c] = dy * dy;
			avgDxy[2][r + c] = dx * dy;


			/*originale Pixelwerte sind nur 8bit, oben wird aber mit int (32bit) gerechnet.
			 * Daher sind die Werte auf das Intervall  [0,255] zu beschränken.  */
			//data.u8TempImage[BACKGROUND][r+c] 	= (uint8) MIN(255, MAX(0, 128+dx));
			//data.u8TempImage[THRESHOLD][r+c] 	= (uint8) MIN(255, MAX(0, 128+dy));
			data.u8TempImage[BACKGROUND][r + c] = (uint8) MIN(255,MAX(0, avgDxy[0]>>10));
			data.u8TempImage[THRESHOLD][r + c] = (uint8) MIN(255, MAX(0, avgDxy[1]>>10));
		} /*inner for*/
	} /*outer for*/


	/*Bilde Mittelwerte*/
	AvgDeriv(0);
	AvgDeriv(1);
	AvgDeriv(2);

	int Mc[]= (avgDxy[0]*avgDxy[1]-avgDxy[2]*avgDxy[2])-(5 *((avgDxy[0]+avgDxy[1])*(avgDxy[0]+avgDxy[1])) << 7);


} /*calc deriv*/



void AvgDeriv(int Index){
	//Bestimme Mittelwerte für I2x, I2y,Ixy.
	int c, r;
	int Border = 6;
	int helpBuf[];
	//do average in x-dir
	for(r = nc; r < nr*nc-nc; r+= nc){ /*skip first and last lines (empty*/
		for(c= Border + 1; c < nc-(Border+1); c++){ /*+1 because we have one empty border column*/
			/*do pointer arithmetics with respect to center pixel location*/
			int* p = &avgDxy[Index][r+c];

			/*Version 1 mit Multiplikation
			int sx = (*(p-6) + *(p+6))*1 + (*(p-5) + *(p+5))*4 +(*(p-4)+ *(p+4))*11 +
					(*(p-3)+ *(p+3))*27 + (*(p-2)+ *(p+2))*50 + (*(p-1)+ *(p+1))*72 +(*p)*82;*/

			/*Version 2 mit Bitshift (schneller)*/
			int sx = (*(p-6) + *(p+6)) + ((*(p-5) + *(p+5)) << 2 ) + ((*(p-4)+ *(p+4)) << 3) +
					((*(p-3)+ *(p+3)) << 5) + ((*(p-2)+ *(p+2)) << 6) + ((*(p-1)+ *(p+1)) << 6) + (*p << 7);

			//now averaged
			helpBuf[r+c] = (sx >> 8);
		}
	}

	//do average in y-direction
	for(c = nr; c < nr*nc-nr; c+= nr){
		for(r = Border + 1; r < nr - (Border+1); r++){
			int* p = &helpBuf[r+c];

			int sy = (*(p-6) + *(p+6)) + ((*(p-5) + *(p+5)) << 2 ) + ((*(p-4)+ *(p+4)) << 3) +
					((*(p-3)+ *(p+3)) << 5) + ((*(p-2)+ *(p+2)) << 6) + ((*(p-1)+ *(p+1)) << 6) + (*p << 7);

			avgDxy[Index][r+c] = (sy >> 8);
		}
	}


	//Skalieren der grössen

}





