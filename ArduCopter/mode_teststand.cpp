#include "Copter.h"

/*
 * Init and run calls for stabilize flight mode
 */

void ModeTestStand::init(){
	//GPIOs for on/off to payload and critical systems

	hal.gpio->pinMode(50, 0);  //Configure as output

	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);
	test_stand_timer = AP_HAL::millis();
	slew_counter = 0;

	do_start_slew = false;
    do_pitch = false;
    do_roll = false;
    do_throttle = false;

    large_deflection_trigger = false;

}



// stabilize_run - runs the main stabilize controller
// should be called at 100hz or more
void ModeTestStand::run()
{

	/*
	float time;
	float small;
	float med;
	float large;

	float servos;
	float throttle_change;

	bool do_pitch;
	bool do_roll;
	bool do_throttle;

	bool do_slew;
	float servo_slew;
*/


	if(!hal.gpio->read(50)){

		if(test_state == SHUT_DOWN){
			activate();
			test_state = START_UP;
			test_stand_timer = AP_HAL::millis();
			startup_complete = false;

		}

		//activate_trigger = false;
	}else{
		test_state = SHUT_DOWN;
		shut_down();
		return;
	}


	if(test_state == START_UP and startup_complete){
		test_state = CHAOS;
		test_stand_timer = AP_HAL::millis();
		large_deflection_timer = AP_HAL::millis();
		//hal.console->print("\n");
		//hal.console->print("DO CHAOS");
		//hal.console->print("\n");

	//}else if(test_state == CHAOS and ((AP_HAL::millis() - test_stand_timer) > 300000)){
	}else if(test_state == CHAOS and ((AP_HAL::millis() - test_stand_timer) > 50000)){
		test_state = SLEW;
		do_start_slew = true;
		do_pitch = false;
		do_roll = false;
		do_throttle = false;
		slew_counter = 0;

	//	hal.console->print("\n");
	//	hal.console->print("DO SLEW");
	//	hal.console->print("\n");
	}


	switch(test_state){

		case SHUT_DOWN:

		break;


		case START_UP:

			attitude_control->set_throttle_out(0.65, true, g.throttle_filt);
			motors->set_pitch(0);
			motors->set_roll(0);
			motors->set_yaw(0);

			if((AP_HAL::millis() - test_stand_timer) > 5000){
				startup_complete = true;
				test_stand_timer = AP_HAL::millis();
				large_deflection_timer =  AP_HAL::millis();

			//	hal.console->print("\n");
			//	hal.console->print("\n");
			//  hal.console->print("STARTUP COMPLETE");
			//	hal.console->print("\n");
			//	hal.console->print("\n");
			}
			break;

		case CHAOS:
			chaos();
			break;

		case SLEW:
			run_sweep();
			break;
	}
}



void ModeTestStand::activate(){

	hal.console->print("\n");
	hal.console->print("\n");
	hal.console->print("ARMED");
	hal.console->print("\n");
	hal.console->print("\n");

	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);
	copter.arm_copter_for_stand();
	set_land_complete(false);
	attitude_control->set_test_stand_passthough(true);
	test_stand_timer = AP_HAL::millis();
	large_deflection_timer = AP_HAL::millis();


}


void ModeTestStand::chaos(){

	float time = (float)AP_HAL::millis()/1000.0f;

	float small_amp = 0.05*sinf(6.2*5*time);
	float small = small_amp*sinf(6.2*2*time);

	float med_amp = 0.15*sinf(6.2*2*time);
	float med = med_amp*sinf(6.2*1*time);

	float rolling = 0.2*sinf(6.2*0.25*time);

	float large = 0;

	if(large_deflection_trigger){

		//hal.console->print("\n");
		//hal.console->print("LARGE");
		//hal.console->print("\n");

		large = 0.40*sinf(6.2*1.0*((AP_HAL::millis() - large_deflection_timer)/1000.0f));


		//hal.console->print("\n");
		//hal.console->printf("Timer: %ld", (AP_HAL::millis() - large_deflection_timer));
		//hal.console->print("\n");

		if(AP_HAL::millis() - large_deflection_timer >= 400){
			large_deflection_trigger = false;
			large_deflection_timer = AP_HAL::millis();

		}

	}else{

		//hal.console->print("\n");
		//hal.console->print("No Large");
		//hal.console->print("\n");

		large = 0;

		if(AP_HAL::millis() - large_deflection_timer >= 12000){
			large_deflection_trigger = true;
			large_deflection_timer = AP_HAL::millis();
		}


		//hal.console->print("\n");
		//hal.console->printf("Timer: %ld", (AP_HAL::millis() - large_deflection_timer));
		//hal.console->print("\n");

	}


	float servos = constrain_float(small+med+large+rolling, -0.9f, 0.9f);

	float throttle_change = constrain_float(0.55+small, 0.25f, 0.95f);


	//motors->set_radio_passthrough(servos,servos,0,0);
	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);
	//motors->set_throttle(throttle_change);
    attitude_control->set_throttle_out(throttle_change,
                                       true,
                                       g.throttle_filt);
	motors->set_pitch(servos);
	motors->set_roll(servos);
	motors->set_yaw(0);

}


void ModeTestStand::run_sweep(){

	//hal.console->print("\n");
	//hal.console->print("SLEW");
	//hal.console->print("\n");

	float servo_slew;

			if(do_start_slew){

				if(slew_counter >= (uint32_t)801){
					do_start_slew = false;
					slew_counter = 0;
					do_pitch = true;
					do_roll = false;
					do_throttle = false;
					test_stand_timer = AP_HAL::millis();
					large_deflection_timer = AP_HAL::millis();

				}else{

					slew_counter++;
				}

			    attitude_control->set_throttle_out(0.55, true, g.throttle_filt);
				motors->set_pitch(0);
				motors->set_roll(0);
				motors->set_yaw(0);

			}else if(do_pitch){

				//hal.console->print("\n");
				//hal.console->print("DO PITCH");
				//hal.console->printf("Slew Timer: %ld", slew_counter);
				//hal.console->print("\n");

				servo_slew = 0.80*sinf(6.2*0.25*(((float)slew_counter)/400.0f));
				servo_slew = constrain_float(servo_slew, -0.8f, 0.8f);

			    attitude_control->set_throttle_out(0.55, false, g.throttle_filt);
				motors->set_pitch(servo_slew);
				motors->set_roll(0);
				motors->set_yaw(0);

				if(slew_counter >= (uint32_t)1601){

					slew_counter = 0;
					do_pitch = false;
					do_roll = true;
					do_throttle = false;

					//hal.console->print("\n");
					//hal.console->print("slew timer > 1600");
					//hal.console->print("\n");
				}else{

					//hal.console->print("\n");
					//hal.console->print("slew timer ++");
					//hal.console->print("\n");

					slew_counter++;
				}


			}else if(do_roll){

				servo_slew = 0.80*sinf(6.2*0.25*(((float)slew_counter)/400.0f));
				servo_slew = constrain_float(servo_slew, -0.8f, 0.8f);

				hal.console->print("\n");
				hal.console->print("DO ROLL");
				hal.console->print("\n");

			    attitude_control->set_throttle_out(0.55, false, g.throttle_filt);
				motors->set_pitch(0);
				motors->set_roll(servo_slew);
				motors->set_yaw(0);

				if(slew_counter >= (uint32_t)1601){
					slew_counter = 0;
					do_pitch = false;
					do_roll = false;
					do_throttle = true;

				}else{

					slew_counter++;
				}


			}else if(do_throttle){

				servo_slew = 0.55 + 0.35*sinf(6.2*0.5*((float)slew_counter/400.0f));
				servo_slew = constrain_float(servo_slew, 0.20, 1.0);

			    attitude_control->set_throttle_out(servo_slew, false, g.throttle_filt);
				motors->set_pitch(0);
				motors->set_roll(0);
				motors->set_yaw(0);

				if(slew_counter >= (uint32_t)801){
					slew_counter = 0;
					do_pitch = false;
					do_roll = false;
					do_throttle = false;
				}else{

					slew_counter++;
				}

			}else{


				if(slew_counter >= (uint32_t)801){
					slew_counter = 0;
					do_pitch = false;
					do_roll = false;
					do_throttle = false;
					test_stand_timer = AP_HAL::millis();
					large_deflection_timer = AP_HAL::millis();
					test_state = CHAOS;
				}else{

					slew_counter++;
				}

			    attitude_control->set_throttle_out(0.55, true, g.throttle_filt);
				motors->set_pitch(0);
				motors->set_roll(0);
				motors->set_yaw(0);
			}


}



void ModeTestStand::shut_down(){


	attitude_control->set_test_stand_passthough(false);
	motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::SHUT_DOWN);
	set_land_complete(true);
	copter.disarm_copter_for_stand();
	test_stand_timer = millis();




}
