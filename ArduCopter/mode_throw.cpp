#include "Copter.h"

#if MODE_THROW_ENABLED == ENABLED

// throw_init - initialise throw controller
bool Copter::ModeThrow::init(bool ignore_checks)
{
#if FRAME_CONFIG == HELI_FRAME
    // do not allow helis to use throw to start
    return false;
#endif

    // do not enter the mode when already armed or when flying
    if (motors->armed()) {
        return false;
    }

    // init state
    stage = Launch_Disarmed;
    nextmode_attempted = false;

    return true;
}


// runs the throw to start controller
// should be called at 100hz or more
void Copter::ModeThrow::run()
{
    /* Throw State Machine
    Launch_Disarmed - motors are off
    Launch_Detecting -  vehicle armed, wating for launch detection
    Launch_Wait_for_PowerUp - vehicle is in the air waiting for power up criteria to be met
    Launch_Stabilizing - motors are turned on and the vehcile is trying to stabilize its self
    Launch_AltHold - vehicle will hold altitude before switching to another flight mode
    */

    // Don't enter THROW mode if interlock will prevent motors running
    if (!motors->armed() && motors->get_interlock()) {

    	// state machine entry is always from a disarmed state
        stage = Launch_Disarmed;

    } else if (stage == Launch_Disarmed && motors->armed()) {

        gcs().send_text(MAV_SEVERITY_INFO,"waiting for launch");
        stage = Launch_Detecting;
        stable = false;

    } else if (stage == Launch_Detecting && Launch_detected()){

    	gcs().send_text(MAV_SEVERITY_INFO,"launch detected");
        stage = Launch_Wait_for_PowerUp;
        set_land_complete(false);
        copter.set_auto_armed(true);
        start_time = (float)AP_HAL::millis();

        // Cancel the waiting for throw tone sequence
        AP_Notify::flags.waiting_for_throw = false;

    } else if (stage == Launch_Wait_for_PowerUp && Power_On()) {

        stage = Launch_Stabilizing;
        copter.set_auto_armed(true);
        set_land_complete(false);

        //clear timer to reuse for stabilize dwell
        start_time = (float)AP_HAL::millis();
        timer = 0.0f;
        stable = false;

    } else if (stage == Launch_Stabilizing && Check_Stabilized()) {

    	gcs().send_text(MAV_SEVERITY_INFO,"Stabilized");
        stage = Launch_AltHold;

        //clear timer to reuse for stabilize dwell
        start_time = (float)AP_HAL::millis();
        timer = 0.0f;
        stable = true;

        // initialize vertical speed and acceleration
        pos_control->set_speed_z(BRAKE_MODE_SPEED_Z, BRAKE_MODE_SPEED_Z);
        pos_control->set_accel_z(BRAKE_MODE_DECEL_RATE);

        // initialise the demanded height to additional above the throw height

        pos_control->set_alt_target(inertial_nav.get_altitude() + (fmaxf(g.add_climb, 0.0f)) );

        // set the initial velocity of the height controller demand to the measured velocity if it is going up
        // if it is going down, set it to zero to enforce a very hard stop
        pos_control->set_desired_velocity_z(fmaxf(inertial_nav.get_velocity_z(), 0.0f));

        // Set the auto_arm status to true to avoid a possible automatic disarm caused by selection of an auto mode with throttle at minimum
        copter.set_auto_armed(true);
        set_land_complete(false);

    } else if (stage == Launch_AltHold && Check_AltHold()) {

        pos_control->freeze_ff_z();

        if (!nextmode_attempted) {
            switch (g2.throw_nextmode) {
                case AUTO:
                case GUIDED:
                case RTL:
                case LAND:
                case BRAKE:
                case LOITER:
                    set_mode((control_mode_t)g2.throw_nextmode.get(), MODE_REASON_THROW_COMPLETE);
                    break;
                default:
                    // do nothing
                    break;
            }
            nextmode_attempted = true;
        }else if(!set_mode(POSHOLD, MODE_REASON_THROW_COMPLETE)){

        	set_mode(ALT_HOLD, MODE_REASON_THROW_COMPLETE);
        }
    }


    // Throw State Processing
    switch (stage) {

    case Launch_Disarmed:

    	 motors->set_desired_spool_state(AP_Motors::DESIRED_SHUT_DOWN);

		stable = false;
		timer = 0.0f;

        // demand zero throttle (motors will be stopped anyway) and continually reset the attitude controller
        attitude_control->set_throttle_out_unstabilized(0,true,g.throttle_filt);
        pos_control->relax_alt_hold_controllers(0.0f);
        pos_control->update_z_controller();
        break;

    case Launch_Detecting:

    	 motors->set_desired_spool_state(AP_Motors::DESIRED_SHUT_DOWN);

    // demand zero throttle (motors will be stopped anyway) and continually reset the attitude controller
         attitude_control->set_throttle_out_unstabilized(0,true,g.throttle_filt);
         pos_control->relax_alt_hold_controllers(0.0f);
         pos_control->update_z_controller();
         break;

    case Launch_Wait_for_PowerUp:

    	 motors->set_desired_spool_state(AP_Motors::DESIRED_SHUT_DOWN);

    	// demand zero throttle (motors will be stopped anyway) and continually reset the attitude controller
    	attitude_control->set_throttle_out_unstabilized(0,true,g.throttle_filt);
    	pos_control->relax_alt_hold_controllers(0.0f);
    	pos_control->update_z_controller();

    	//update timer
    	timer =  ((float)AP_HAL::millis()) - start_time;
        break;

    case Launch_Stabilizing:

    	// set motors to full range
    	motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    	//update timer only if stable, reset otherwise
    	if(stable){
    		timer =  ((float)AP_HAL::millis()) - start_time;
    	}else{
    		start_time = ((float)AP_HAL::millis());
    	}

        // demand a level roll/pitch attitude with zero yaw rate
        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw(0.0f, 0.0f, 0.0f);

        //Set throttle to hold at parameter value until stabilized
        attitude_control->set_throttle_out(g.thr_set_hold, false, g.throttle_filt);
        pos_control->relax_alt_hold_controllers(motors->get_throttle_hover());

        break;

    case Launch_AltHold:

        // set motors to full range
        motors->set_desired_spool_state(AP_Motors::DESIRED_THROTTLE_UNLIMITED);

    		timer =  ((float)AP_HAL::millis()) - start_time;

        // demand a level roll/pitch attitude with zero yaw rate
        attitude_control->input_euler_angle_roll_pitch_euler_rate_yaw(0.0f, 0.0f, 0.0f);

        // call height controller
        pos_control->update_z_controller();

        break;
    }

    // log at 10hz or if stage changes
    //changed logging to 20Hz
    uint32_t now = AP_HAL::millis();
    if ((stage != prev_stage) || (now - last_log_ms) > 50) {
        prev_stage = stage;
        last_log_ms = now;
        const float log_timer = timer;
        const float velocity_z = inertial_nav.get_velocity().z;
        const float accel = copter.ins.get_accel().z;
        const float ef_accel_z = ahrs.get_accel_ef().z;
        const bool launch_detect = (stage > Launch_Detecting) || Launch_detected();
        const bool pwr_on = (stage > Launch_Wait_for_PowerUp) || Power_On();
        const bool stabilize = (stage > Launch_Stabilizing) || Check_Stabilized();
        const bool alt_ok = (stage == Launch_AltHold);
        DataFlash_Class::instance()->Log_Write(
            "THRO",
            "TimeUS,Stage,log_timer,vel_z,Acc,AccEfZ,LchDet,PwrOn,Stab,PosOk",
            "s-nnoo----",
            "F-0000----",
            "QBffffbbbb",
            AP_HAL::micros64(),
            (uint8_t)stage,
            (double)log_timer,
            (double)velocity_z,
            (double)accel,
            (double)ef_accel_z,
			launch_detect,
			pwr_on,
			stabilize,
            alt_ok);
    }
}

bool Copter::ModeThrow::Launch_detected()
{

	float accel_check = copter.ins.get_accel().z;


	//acceleration above 'launch_detect' in body frame will trigger launch detection
	if(accel_check <= -(g.launch_detect * GRAVITY_MSS)){
		return true;
	}

	return false;
}

bool Copter::ModeThrow::Power_On()
{


	if (timer <= g.launch_hold_time){

		// do nothing if not past the hold time
		return false;

	}else if(timer >= g.launch_timeout){

	    // power on if the timer is over the timeout period
		gcs().send_text(MAV_SEVERITY_INFO,"Pwr: Time-Out");
		timer = 0.0f;
		start_time = ((float)AP_HAL::millis());
		return true;

	}else{

		// power on if the timer is over the hold period and the minimum velocity is deteched
		if(fabsf(inertial_nav.get_velocity().z) <= g.min_velo_z_on){

			gcs().send_text(MAV_SEVERITY_INFO,"Pwr: Vz");
			timer = 0.0f;
			start_time = ((float)AP_HAL::millis());
			return true;
		}

	return false;
	}

}

bool Copter::ModeThrow::Check_Stabilized()
{

	if(stage < Launch_Stabilizing){

		return false;
	}

	//criteria for being upright

	 if((labs(ahrs.roll_sensor) < g.stab_angle_cd) and (labs(ahrs.pitch_sensor) < g.stab_angle_cd)){
		stable = true;
	}else{
		stable = false;
	}

	if(timer >= g.min_dwell) {

		timer = 0.0f;
		start_time = ((float)AP_HAL::millis());
		return true;

	}else{
		return false;
	}

}


bool Copter::ModeThrow::Check_AltHold()
{
	//will always return true once the dwell has expired

	if (timer <= g.min_dwell){

		// do nothing if not past the hold time
		return false;

	}else{
			return true;
	}


}



#endif
