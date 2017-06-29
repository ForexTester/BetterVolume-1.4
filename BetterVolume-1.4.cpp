//---------------------------------------------------------------------------
// Better Volume 1.4
//---------------------------------------------------------------------------

#include <windows.h>
#include <cmath>
#include "IndicatorInterfaceUnit.h"
#include "TechnicalFunctions.h"

// External variables
int NumberOfBars;
int MAPeriod;
int LookBack;

// Buffers
TIndexBuffer red, blue, yellow, green, white, magenta, v4;

//---------------------------------------------------------------------------
// Initialize indicator
//---------------------------------------------------------------------------
EXPORT void __stdcall Init() {
	// define properties
	IndicatorShortName("Better Volume 1.4");
	SetOutputWindow(ow_SeparateWindow);

	// register options
	AddSeparator("Common");

	RegOption("Number of bars (0 means display all bars)", ot_Integer, &NumberOfBars);
	SetOptionRange("Number of bars (0 means display all bars)", 1, MaxInt);
	NumberOfBars = 500;

	RegOption("MA Period", ot_Integer, &MAPeriod);
	SetOptionRange("MA Period", 1, MaxInt);
	MAPeriod = 100;

	RegOption("Look back", ot_Integer, &LookBack);
	SetOptionRange("Look back", 1, MaxInt);
	LookBack = 20;

	// create buffers
	red     = CreateIndexBuffer();
	blue    = CreateIndexBuffer();
	yellow  = CreateIndexBuffer();
	green   = CreateIndexBuffer();
	white   = CreateIndexBuffer();
	magenta = CreateIndexBuffer();
	v4      = CreateIndexBuffer();

	IndicatorBuffers(7);

	SetIndexBuffer(0, red);
	SetIndexStyle(0, ds_Histogram, psSolid, 2, clRed);
	SetIndexLabel(0, "Climax High");

	SetIndexBuffer(1, blue);
	SetIndexStyle(1, ds_Histogram, psSolid, 2, clBlue);
	SetIndexLabel(1, "Neutral");

	SetIndexBuffer(2, yellow);
	SetIndexStyle(2, ds_Histogram, psSolid, 2, clYellow);
	SetIndexLabel(2, "Low");

	SetIndexBuffer(3, green);
	SetIndexStyle(3, ds_Histogram, psSolid, 2, clGreen);
	SetIndexLabel(3, "High Churn");

	SetIndexBuffer(4, white);
	SetIndexStyle(4, ds_Histogram, psSolid, 2, clWhite);
	SetIndexLabel(4, "Climax Low");

	SetIndexBuffer(5, magenta);
	SetIndexStyle(5, ds_Histogram, psSolid, 2, clMagenta);
	SetIndexLabel(5, "Climax Churn");

	SetIndexBuffer(6, v4);
	SetIndexStyle(6, ds_Line, psSolid, 2, clMaroon);
	SetIndexLabel(6, "Average(MA Period)");
}

//---------------------------------------------------------------------------
// Calculate requested bar
//---------------------------------------------------------------------------
EXPORT void __stdcall Calculate(int i) {
	double VolLowest, Range, Value2, Value3, HiValue2, HiValue3, LoValue3, tempv2, tempv3, tempv;

	red[i] = 0; blue[i] = Volume(i); yellow[i] = 0; green[i] = 0; white[i] = 0; magenta[i] = 0;
	Value2 = 0; Value3 = 0; HiValue2 = 0; HiValue3 = 0; LoValue3 = 99999999; tempv2 = 0; tempv3 = 0; tempv = 0;

	VolLowest = Volume(iLowest(NULL, 0, MODE_VOLUME, 20, i));
	
	if (Volume(i) == VolLowest) {
		yellow[i] = round(Volume(i)); //NormalizeDouble(Volume(i), 0);
		blue[i] = 0;
	}

	Range = (High(i) - Low(i));
	Value2 = Volume(i) * Range;

	if (Range != 0) { Value3 = Volume(i) / Range; }

	for (int n = i; n < i + MAPeriod; n++) {
		tempv = Volume(n) + tempv;
	}
	
	v4[i] = round(tempv / MAPeriod); //NormalizeDouble(tempv / MAPeriod, 0);


	for (int n = i; n<i + LookBack; n++) {
		tempv2 = Volume(n) * ((High(n) - Low(n)));
		if (tempv2 >= HiValue2) { HiValue2 = tempv2; }

		if (Volume(n) * ((High(n) - Low(n))) != 0) {
			tempv3 = Volume(n) / ((High(n) - Low(n)));
			if (tempv3 > HiValue3) { HiValue3 = tempv3; }
			if (tempv3 < LoValue3) { LoValue3 = tempv3; }
		}
	}

	if (Value2 == HiValue2  && Close(i) >(High(i) + Low(i)) / 2) {
		red[i] = round(Volume(i)); //NormalizeDouble(Volume[i], 0);
		blue[i] = 0;
		yellow[i] = 0;
	}

	if (Value3 == HiValue3) {
		green[i] = round(Volume(i)); //NormalizeDouble(Volume[i], 0);
		blue[i] = 0;
		yellow[i] = 0;
		red[i] = 0;
	}
	
	if (Value2 == HiValue2 && Value3 == HiValue3) {
		magenta[i] = round(Volume(i)); //NormalizeDouble(Volume[i], 0);
		blue[i] = 0;
		red[i] = 0;
		green[i] = 0;
		yellow[i] = 0;
	}
	
	if (Value2 == HiValue2  && Close(i) <= (High(i) + Low(i)) / 2) {
		white[i] = round(Volume(i)); //NormalizeDouble(Volume[i], 0);
		magenta[i] = 0;
		blue[i] = 0;
		red[i] = 0;
		green[i] = 0;
		yellow[i] = 0;
	}

}
