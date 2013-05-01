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
const int	stereospread	= 23/2;

// These values assume 44.1KHz sample rate
// they will probably be OK for 48KHz sample rate
// but would need scaling for 96KHz (or other) sample rates.
// The values were obtained by listening tests.
const int combtuningL1		= 1116/2;
const int combtuningR1		= 1116/2+stereospread;
const int combtuningL2		= 1188/2;
const int combtuningR2		= 1188/2+stereospread;
const int combtuningL3		= 1277/2;
const int combtuningR3		= 1277/2+stereospread;
const int combtuningL4		= 1356/2;
const int combtuningR4		= 1356/2+stereospread;
const int combtuningL5		= 1422/2;
const int combtuningR5		= 1422/2+stereospread;
const int combtuningL6		= 1491/2;
const int combtuningR6		= 1491/2+stereospread;
const int combtuningL7		= 1557/2;
const int combtuningR7		= 1557/2+stereospread;
const int combtuningL8		= 1617/2;
const int combtuningR8		= 1617/2+stereospread;
const int allpasstuningL1	= 556/2;
const int allpasstuningR1	= 556/2+stereospread;
const int allpasstuningL2	= 441/2;
const int allpasstuningR2	= 441/2+stereospread;
const int allpasstuningL3	= 341/2;
const int allpasstuningR3	= 341/2+stereospread;
const int allpasstuningL4	= 225/2;
const int allpasstuningR4	= 225/2+stereospread;

}

#endif//_tuning_

//ends

