// Reverb model tuning values
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#ifndef _tuning_
#define _tuning_

namespace Freeverb {

const int	numcombs		= 8;
const int	numallpasses	= 4;
const float	muted			= 0;
const float	fixedgain		= 0.015f;
const float scalewet		= 3.0f;
const float scaledry		= 2;
const float scaledamp		= 0.4f;
const float scaleroom		= 0.28f;
const float offsetroom		= 0.7f;
const float initialroom		= 0.75f;
const float initialdamp		= 0.5f;
const float initialdry		= 0.5;
const float initialwidth	= 1.0f;
const float initialmode		= 0;
const float freezemode		= 0.5f;

const int div = 2;

const int	stereospread	= 23/div;

// These values assume 44.1KHz sample rate
// they will probably be OK for 48KHz sample rate
// but would need scaling for 96KHz (or other) sample rates.
// The values were obtained by listening tests.
const int combtuningL1		= 1116/div;
const int combtuningR1		= 1116/div+stereospread;
const int combtuningL2		= 1188/div;
const int combtuningR2		= 1188/div+stereospread;
const int combtuningL3		= 1277/div;
const int combtuningR3		= 1277/div+stereospread;
const int combtuningL4		= 1356/div;
const int combtuningR4		= 1356/div+stereospread;
const int combtuningL5		= 1422/div;
const int combtuningR5		= 1422/div+stereospread;
const int combtuningL6		= 1491/div;
const int combtuningR6		= 1491/div+stereospread;
const int combtuningL7		= 1557/div;
const int combtuningR7		= 1557/div+stereospread;
const int combtuningL8		= 1617/div;
const int combtuningR8		= 1617/div+stereospread;
const int allpasstuningL1	= 556/div;
const int allpasstuningR1	= 556/div+stereospread;
const int allpasstuningL2	= 441/div;
const int allpasstuningR2	= 441/div+stereospread;
const int allpasstuningL3	= 341/div;
const int allpasstuningR3	= 341/div+stereospread;
const int allpasstuningL4	= 225/div;
const int allpasstuningR4	= 225/div+stereospread;

}

#endif//_tuning_

//ends

