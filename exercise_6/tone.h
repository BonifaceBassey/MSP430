//		note    period  frequency   duty
#define  C4     3830    // 261 Hz	1915
#define  D4     3400    // 294 Hz	1700
#define  E4     3038    // 329 Hz	1519
#define  F4     2864    // 349 Hz	1432
#define  G4     2550    // 392 Hz	1275
#define  A4     2272    // 440 Hz	1136
#define  B4     2028    // 493 Hz	1014
#define  C5     1912    // 523 Hz	956
#define  B4b    2146	// 466 Hz	1073
#define  E5     1517	// 659 Hz	759

unsigned int HappyBirthday[25] = {
			C4, C4, D4, C4, F4, E4,
			C4, C4, D4, C4, G4, F4,
			C4, C4, C5, A4, F4, E4, D4,
			B4b, B4b, A4, F4, G4, F4
};
unsigned short interval1[25] = {
		    200, 200, 270, 270, 270, 670,
			200, 200, 270, 270, 270, 590,
			200, 200, 270, 270, 270, 300, 670,
			250, 250, 300, 300, 300, 590
};

unsigned int OTannenBaum[16] = {
		D4, G4, G4, G4, A4, B4, B4, B4,
		B4, A4, B4, C5, E5, A4, G4
};

unsigned int interval2[16] = {
	    400, 200, 200, 400, 400, 200, 200, 400,
		200, 200, 200, 400, 400, 400, 400
};
