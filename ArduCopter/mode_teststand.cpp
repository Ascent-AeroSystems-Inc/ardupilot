#include "Copter.h"

/*
 * Init and run calls for stabilize flight mode
 */

void ModeTestStand::init(){
	//GPIOs for on/off to payload and critical systems

	hal.gpio->pinMode(50, 0);  //Configure as output

	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);

	activate = false;
}

// stabilize_run - runs the main stabilize controller
// should be called at 100hz or more
void ModeTestStand::run()
{

//Servos

float time = (float)AP_HAL::millis()/1000.0f;

float small = 0.05*sinf(6.2*3*time);
float med = 0.10*sinf(6.2*1*time);
float large = 0.5*sinf(6.2*0.05*time);

float servos = constrain_float(small+med+large, -1.0f, 1.0f);

float throttle_change = constrain_float(0.50+small+med, 0.0f, 1.0f);

if(!hal.gpio->read(50)){

	if(!activate){
		hal.console->print("\n");
		hal.console->print("\n");
		hal.console->print("ARMED");
		hal.console->print("\n");
		hal.console->print("\n");
		motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);
		copter.arm_copter_for_stand();
		set_land_complete(false);
		attitude_control->set_test_stand_passthough(true);
		activate = true;
	}

	//motors->set_radio_passthrough(servos,servos,0,0);
	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);
	//motors->set_throttle(throttle_change);
    attitude_control->set_throttle_out(throttle_change,
                                       true,
                                       g.throttle_filt);
	motors->set_pitch(servos);
	motors->set_roll(servos);
	motors->set_yaw(0);


}else{
	attitude_control->set_test_stand_passthough(false);
	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::SHUT_DOWN);
	set_land_complete(true);
	activate = false;
    copter.disarm_copter_for_stand();

}


}
