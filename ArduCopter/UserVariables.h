// user defined variables

// example variables used in Wii camera testing - replace with your own
// variables
#ifdef USERHOOK_VARIABLES

//KillSwitch
	uint8_t killswitch_counter = 0;

// Vehicle State Logic
	VehicleState CoaxState = Disarm;
//	bool take_off_complete = false;


// Arming Logic

	bool arming_hold = false;




#if WII_CAMERA == 1
WiiCamera           ircam;
int                 WiiRange=0;
int                 WiiRotation=0;
int                 WiiDisplacementX=0;
int                 WiiDisplacementY=0;
#endif  // WII_CAMERA

#endif  // USERHOOK_VARIABLES


