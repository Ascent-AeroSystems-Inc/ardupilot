#include "Copter.h"

#ifdef USERHOOK_INIT
void Copter::userhook_init()
{
    // put your initialisation code here
    // this will be called once at start-up





	AP_Notify::flags.flight_mode_change_fail = false;

	AP_Notify::flags.flight_mode_change = false;

	AP_Notify::flags.arm_failed = false;

	AP_Notify::flags.LED_counter = 0;

	AP_Notify::flags.arming = false;


}
#endif

#ifdef USERHOOK_FASTLOOP
void Copter::userhook_FastLoop()
{
    // put your 100Hz code here


/*
	if(hal.mag_flag_1){

		gcs().send_text(MAV_SEVERITY_INFO, "Trying to initialize to PCA9685");

		hal.mag_flag_1 = false;

	}


	if(hal.mag_flag_2){

		gcs().send_text(MAV_SEVERITY_INFO, "Failed to init");

		hal.mag_flag_2 = false;

	}


	if(hal.mag_flag_3){

		gcs().send_text(MAV_SEVERITY_INFO, "Updating...");

		hal.mag_flag_3 = false;

	}


*/



}
#endif

#ifdef USERHOOK_50HZLOOP
void Copter::userhook_50Hz()
{
    // put your 50Hz code here
}
#endif

#ifdef USERHOOK_MEDIUMLOOP
void Copter::userhook_MediumLoop()
{
    // put your 10Hz code here



}
#endif

#ifdef USERHOOK_SLOWLOOP
void Copter::userhook_SlowLoop()
{
    // put your 3.3Hz code here
}
#endif

#ifdef USERHOOK_SUPERSLOWLOOP
void Copter::userhook_SuperSlowLoop()
{
    // put your 1Hz code here
}
#endif

#ifdef USERHOOK_AUXSWITCH
void Copter::userhook_auxSwitch1(uint8_t ch_flag)
{
    // put your aux switch #1 handler here (CHx_OPT = 47)
}

void Copter::userhook_auxSwitch2(uint8_t ch_flag)
{
    // put your aux switch #2 handler here (CHx_OPT = 48)
}

void Copter::userhook_auxSwitch3(uint8_t ch_flag)
{
    // put your aux switch #3 handler here (CHx_OPT = 49)
}
#endif
