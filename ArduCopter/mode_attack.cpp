#include "Copter.h"

/*
 * Init and run calls for loiter flight mode
 */

// loiter_init - initialise loiter controller
bool ModeAttack::init(bool ignore_checks)
{
	    wp_nav->wp_and_spline_init();

	    // initialise yaw
	    auto_yaw.set_mode_to_default(false);

	    copter.camera_mount.get_roi_target(target_location);
	    float current_alt =  inertial_nav.get_altitude();

	   // current_alt = 2200;

	    target_location.set_alt_cm((int32_t)current_alt, Location::AltFrame::ABOVE_ORIGIN);
	    wp_nav->set_wp_destination(target_location);

	    copter.camera_mount.enable_RC_control(true);

    return true;
}




// loiter_run - runs the loiter controller
// should be called at 100hz or more
void ModeAttack::run()
{


    // if not armed set throttle to zero and exit immediately
    if (is_disarmed_or_landed()) {
        make_safe_spool_down();
        wp_nav->wp_and_spline_init();

        copter.Log_Write_Attack(1, Vector3f(target_location.lat, target_location.lng, target_location.alt),Vector3f());

        return;
    }


    // set motors to full range
    motors->set_desired_spool_state(AP_Motors::DesiredSpoolState::THROTTLE_UNLIMITED);

    // run waypoint controller
    copter.failsafe_terrain_set_status(wp_nav->update_wpnav());

    // call z-axis position controller (wpnav should have already updated it's alt target)
    pos_control->update_z_controller();


	// roll, pitch from waypoint controller, yaw heading from auto_heading()
	attitude_control->input_euler_angle_roll_pitch_yaw(wp_nav->get_roll(), wp_nav->get_pitch(), auto_yaw.yaw(), true);



	//updated when button is pressed in usercode.cpp
	if(copter.location_target_updated){

	    copter.camera_mount.get_roi_target(target_location);

	    float current_alt =  inertial_nav.get_altitude();

	    //current_alt = 3500;

	    target_location.set_alt_cm((int32_t)current_alt, Location::AltFrame::ABOVE_ORIGIN);

	    wp_nav->set_wp_destination(target_location);


		copter.location_target_updated = false;
	}


    copter.Log_Write_Attack(1, Vector3f(target_location.lat, target_location.lng, target_location.alt),Vector3f());


}


