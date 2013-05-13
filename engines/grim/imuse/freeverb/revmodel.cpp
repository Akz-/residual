// Reverb model implementation
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "common/math.h"
#include "revmodel.hpp"

using namespace Freeverb;

revmodel::revmodel()
{
	// Tie the components to their buffers
	combL[0].setbuffer(bufcombL1,combtuningL1);
	combR[0].setbuffer(bufcombR1,combtuningR1);
	combL[1].setbuffer(bufcombL2,combtuningL2);
	combR[1].setbuffer(bufcombR2,combtuningR2);
	combL[2].setbuffer(bufcombL3,combtuningL3);
	combR[2].setbuffer(bufcombR3,combtuningR3);
	combL[3].setbuffer(bufcombL4,combtuningL4);
	combR[3].setbuffer(bufcombR4,combtuningR4);
	combL[4].setbuffer(bufcombL5,combtuningL5);
	combR[4].setbuffer(bufcombR5,combtuningR5);
	combL[5].setbuffer(bufcombL6,combtuningL6);
	combR[5].setbuffer(bufcombR6,combtuningR6);
	combL[6].setbuffer(bufcombL7,combtuningL7);
	combR[6].setbuffer(bufcombR7,combtuningR7);
	combL[7].setbuffer(bufcombL8,combtuningL8);
	combR[7].setbuffer(bufcombR8,combtuningR8);
	allpassL[0].setbuffer(bufallpassL1,allpasstuningL1);
	allpassR[0].setbuffer(bufallpassR1,allpasstuningR1);
	allpassL[1].setbuffer(bufallpassL2,allpasstuningL2);
	allpassR[1].setbuffer(bufallpassR2,allpasstuningR2);
	allpassL[2].setbuffer(bufallpassL3,allpasstuningL3);
	allpassR[2].setbuffer(bufallpassR3,allpasstuningR3);
	allpassL[3].setbuffer(bufallpassL4,allpasstuningL4);
	allpassR[3].setbuffer(bufallpassR4,allpasstuningR4);

	// Set default values
	allpassL[0].setfeedback(0.5f);
	allpassR[0].setfeedback(0.5f);
	allpassL[1].setfeedback(0.5f);
	allpassR[1].setfeedback(0.5f);
	allpassL[2].setfeedback(0.5f);
	allpassR[2].setfeedback(0.5f);
	allpassL[3].setfeedback(0.5f);
	allpassR[3].setfeedback(0.5f);
	setwet(1.0f/scalewet);
	setroomsize(initialroom);
	setdry(initialdry);
	setdamp(initialdamp);
	setwidth(initialwidth);
	setmode(initialmode);

	// Buffer will be full of rubbish - so we MUST mute them
	mute();
}

void revmodel::mute()
{
	if (getmode() >= freezemode)
		return;

	for (int i=0;i<numcombs;i++)
	{
		combL[i].mute();
		combR[i].mute();
	}
	for (int i=0;i<numallpasses;i++)
	{
		allpassL[i].mute();
		allpassR[i].mute();
	}
}

inline float highpass(float input, float dt, float rc)
{
	static float prevDry = 0.0f;
	static float prevWet = 0.0f;

	float alpha = rc / (rc + dt);
	float output = alpha * (prevWet + input - prevDry);
	prevWet = output;
	prevDry = input;
	return output;
}

inline float lowpass(float input, float dt, float rc)
{
	static float prevWet = 0.0f;

	float alpha = dt / (rc + dt);
	float output = prevWet + alpha * (input - prevWet);
	prevWet = output;
	return output;
}

inline float intercom(float input, const float a[], const float b[])
{
	static float xv[2];
	static float yv[2];

	xv[2] = xv[1];
	xv[1] = xv[0];
	xv[0] = input;

	yv[2] = yv[1];
	yv[1] = yv[0];

	yv[0] = b[2] * xv[2]
	      + b[1] * xv[1]
		  + b[0] * xv[0]
		  - a[2] * yv[2]
		  - a[1] * yv[1];

	return yv[0];
}

void revmodel::processreplace(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;

	//setwet(1.0f/8);
	//setroomsize(0.75f);
	//setdry(1.0f);
	//setdamp(0.2);
	//setwidth(initialwidth);
	//setmode(initialmode);

	setwet(1.0f/2);
	setroomsize(0.2f - 0.7);
	setdry(0.5f);
	setdamp(0.9);
	setwidth(initialwidth);

	float f1 = 1326.0f / 22050.0f;
	float f2 = 4326.0f / 22050.0f;
	float t = 1 / 2.0f;

	f1 = 2 * M_PI * f1;
	f2 = 2 * M_PI * f2;
	f1 = 2/t * tan(f1 * t/2);
	f2 = 2/t * tan(f2 * t/2);

	float bw = f2 - f1;
	float center = sqrt(f1 * f2);

	float v1 = 2 * bw * t;
	float v2 = center * center * t * t;

	float a1 = v1 + v2 + 4;

	float b[] = {v1 / a1, 0, -v1 / a1};
	float a[] = {1, (2 * v2 - 8) / a1, (v2 - v1 + 4) / a1};

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * 0.5;

		float rc = 1 / (2 * 3.14159265f * 200.0f);
		float dt = 1 / 22050.0f;
		
		rc = 1 / (2 * 3.14159265f * 500.0f);
		float o1 = highpass(input, dt, rc);
		//input = lowpass(input, dt, rc) * 1;
		o1 = o1 * 2.0;

		rc = 1 / (2 * 3.14159265f * 326.0f);
		float o2 = lowpass(input, dt, rc);

		input = intercom(*inputL, a, b) * 1.5;

		// Accumulate comb filters in parallel
		/*for(int i=0; i<8; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(int i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}*/

		// Calculate output REPLACING anything already there
		*outputL = input;
		*outputR = input;

		if (*outputL > 1)
			*outputL = 0.999f;
		if (*outputL < -1)
			*outputL = -0.999f;
		if (*outputR > 1)
			*outputR = 0.999f;
		if (*outputR < -1)
			*outputR = -0.999f;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

void revmodel::processmix(float *inputL, float *inputR, float *outputL, float *outputR, long numsamples, int skip)
{
	float outL,outR,input;

	while(numsamples-- > 0)
	{
		outL = outR = 0;
		input = (*inputL + *inputR) * gain;

		// Accumulate comb filters in parallel
		for(int i=0; i<numcombs; i++)
		{
			outL += combL[i].process(input);
			outR += combR[i].process(input);
		}

		// Feed through allpasses in series
		for(int i=0; i<numallpasses; i++)
		{
			outL = allpassL[i].process(outL);
			outR = allpassR[i].process(outR);
		}

		// Calculate output MIXING with anything already there
		*outputL += outL*wet1 + outR*wet2 + *inputL*dry;
		*outputR += outR*wet1 + outL*wet2 + *inputR*dry;

		// Increment sample pointers, allowing for interleave (if any)
		inputL += skip;
		inputR += skip;
		outputL += skip;
		outputR += skip;
	}
}

void revmodel::update()
{
// Recalculate internal values after parameter change

	int i;

	wet1 = wet*(width/2 + 0.5f);
	wet2 = wet*((1-width)/2);

	if (mode >= freezemode)
	{
		roomsize1 = 1;
		damp1 = 0;
		gain = muted;
	}
	else
	{
		roomsize1 = roomsize;
		damp1 = damp;
		gain = fixedgain;
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setfeedback(roomsize1);
		combR[i].setfeedback(roomsize1);
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setdamp(damp1);
		combR[i].setdamp(damp1);
	}
}

// The following get/set functions are not inlined, because
// speed is never an issue when calling them, and also
// because as you develop the reverb model, you may
// wish to take dynamic action when they are called.

void revmodel::setroomsize(float value)
{
	roomsize = (value*scaleroom) + offsetroom;
	update();
}

float revmodel::getroomsize()
{
	return (roomsize-offsetroom)/scaleroom;
}

void revmodel::setdamp(float value)
{
	damp = value*scaledamp;
	update();
}

float revmodel::getdamp()
{
	return damp/scaledamp;
}

void revmodel::setwet(float value)
{
	wet = value*scalewet;
	update();
}

float revmodel::getwet()
{
	return wet/scalewet;
}

void revmodel::setdry(float value)
{
	dry = value*scaledry;
}

float revmodel::getdry()
{
	return dry/scaledry;
}

void revmodel::setwidth(float value)
{
	width = value;
	update();
}

float revmodel::getwidth()
{
	return width;
}

void revmodel::setmode(float value)
{
	mode = value;
	update();
}

float revmodel::getmode()
{
	if (mode >= freezemode)
		return 1;
	else
		return 0;
}

//ends
