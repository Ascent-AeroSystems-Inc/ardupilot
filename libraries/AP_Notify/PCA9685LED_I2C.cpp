/*
  PCA9685LED I2C driver
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "PCA9685LED_I2C.h"

#include <AP_HAL/AP_HAL.h>

#define NAVIO_LED_BRIGHT 0x0    // full brightness
#define NAVIO_LED_MEDIUM 0x7F    // medium brightness
#define NAVIO_LED_DIM    0x4F    // dim brightness
#define NAVIO_LED_OFF    0xFF    // off

#define PCA9685_ADDRESS 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_PWM 0x6
#define PCA9685_MODE_SLEEP          (1 << 4)
#define PCA9685_MODE_AUTO_INCREMENT (1 << 5)

extern const AP_HAL::HAL& hal;

PCA9685LED_I2C::PCA9685LED_I2C() : 
    RGBLed(NAVIO_LED_OFF, NAVIO_LED_BRIGHT, NAVIO_LED_MEDIUM, NAVIO_LED_DIM)
{
}

bool PCA9685LED_I2C::hw_init()
{


/*

	 // first look for led on external bus
	    _dev = std::move(hal.i2c_mgr->get_device(0, PCA9685_ADDRESS));
	    if (!_dev) {
	        return false;
	    }
	    WITH_SEMAPHORE(_dev->get_semaphore());

	    _dev->set_retries(10);

	    // enable the led
	    bool ret = _dev->write_register(0x00, 0x20);
	    if (!ret) {
	        return false;
	    }



	    _dev->write_register(0xFA, 0x00);

	    _dev->write_register( 0xFB, 0x10);

	    _dev->write_register(0xFC, 0x00);

	    _dev->write_register(0xFD, 0x00);





	    _dev->set_retries(5);

	    if (ret) {
	    	 _dev->register_periodic_callback(20000, FUNCTOR_BIND_MEMBER(&PCA9685LED_I2C::_timer, void));
	    }

	    return ret;


*/




   _dev = hal.i2c_mgr->get_device(0, PCA9685_ADDRESS);

   _dev->get_semaphore()->take_blocking();

   _dev->set_retries(5);



   bool ret = _dev->write_register(0x00, 0x21);

  // _dev->write_register(0xFA, 0x00);

  // _dev->write_register( 0xFB, 0x10);

  // _dev->write_register(0xFC, 0x00);

 //  _dev->write_register(0xFD, 0x00);


   _dev->set_retries(1);

   _dev->get_semaphore()->give();

   if (ret) {
   	 _dev->register_periodic_callback(20000, FUNCTOR_BIND_MEMBER(&PCA9685LED_I2C::_timer, void));
   }

   return ret;


   /*

   const uint8_t config1[2] = {PCA9685_MODE1, 0xF};
   _dev->transfer(config1, sizeof(config1), nullptr, 0);





   const uint8_t config2[2]= {0xFA, 0x00};
   _dev->transfer(config2, sizeof(config2), nullptr, 0);


   const uint8_t config3[2] = {0xFB, 0x10};
   _dev->transfer(config3, sizeof(config3), nullptr, 0);



   const uint8_t config4[2] = {0xFC, 0x00};
   _dev->transfer(config4, sizeof(config4), nullptr, 0);



   const uint8_t config5[2] = {0xFD, 0x00};
   _dev->transfer(config5, sizeof(config5), nullptr, 0);




   _dev->set_retries(5);

   _dev->get_semaphore()->give();

*/

   /*
    if (!_dev) {

        return false;
    }



    _dev->get_semaphore()->take_blocking();

    _dev->set_retries(5);

    // read the current mode1 configuration
    uint8_t mode1 = 0;
    if (!_dev->read_registers(PCA9685_MODE1, &mode1, sizeof(mode1))) {
        _dev->get_semaphore()->give();
        return false;
    }

    // bring the device out of sleep, and enable auto register increment
    uint8_t new_mode1 = (mode1 | PCA9685_MODE_AUTO_INCREMENT) & ~PCA9685_MODE_SLEEP;
    const uint8_t config[2] = {PCA9685_MODE1, new_mode1};
    if (!_dev->transfer(config, sizeof(config), nullptr, 0)) {
        _dev->get_semaphore()->give();
        return false;
    }

    _dev->set_retries(1);

    _dev->get_semaphore()->give();

    _dev->register_periodic_callback(20000, FUNCTOR_BIND_MEMBER(&PCA9685LED_I2C::_timer, void));

    */

    return true;
}

// set_rgb - set color as a combination of red, green and blue values
bool PCA9685LED_I2C::hw_set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    rgb.r = red;
    rgb.g = green;
    rgb.b = blue;
    _need_update = true;
    return true;
}

void PCA9685LED_I2C::_timer(void)
{



	/*
	_dev->write_register(0x06, 0x00);
	_dev->write_register(0x07, 0x10);
	_dev->write_register(0x08, 0xFF);
	_dev->write_register(0x09, 0x00);




    if (!_need_update) {
        return;
    }

    _need_update = false;




	   _dev->write_register(0xFA, 0x00);

	  _dev->write_register( 0xFB, 0x10);

	   _dev->write_register(0xFC, 0x00);

	   _dev->write_register(0xFD, 0x00);




    uint16_t red_adjusted = rgb.r * 0x10;
    uint16_t green_adjusted = rgb.g * 0x10;
    uint16_t blue_adjusted = rgb.b * 0x10;
*/

    uint8_t red_on;
    uint8_t red_off;

    uint8_t green_on;
    uint8_t green_off;

    uint8_t blue_on;
    uint8_t blue_off;


    if(rgb.r > 0){
        red_on = 0x10;
        red_off = 0x00;
    }else{
        red_on = 0x00;
        red_off = 0x10;
    }



    if(rgb.b > 0){
        blue_on = 0x10;
        blue_off = 0x00;
    }else{
    	blue_on = 0x00;
    	blue_off = 0x10;
    }


    if(rgb.g != 0){
        green_on = 0x10;
        green_off = 0x00;
    }else{
    	 green_on = 0x00;
    	 green_off = 0x10;
    }


/*
    uint8_t blue_channel_lsb = blue_adjusted & 0xFF;
    uint8_t blue_channel_msb = blue_adjusted >> 8;

    uint8_t green_channel_lsb = green_adjusted & 0xFF;
    uint8_t green_channel_msb = green_adjusted >> 8;

    uint8_t red_channel_lsb = red_adjusted & 0xFF;
    uint8_t red_channel_msb = red_adjusted >> 8;
*/


    uint8_t transaction[] = {PCA9685_PWM, 0x00, red_on, 0xFF, red_off,
    			 0x00, green_on, 0xFF, green_off,
				 0x00, blue_on, 0xFF, blue_off};




    /*
     *
     *     uint8_t transaction[] = {PCA9685_PWM, 0x00, 0x00, blue_channel_lsb, blue_channel_msb,
			     0x00, 0x00, green_channel_lsb, green_channel_msb,
			     0x00, 0x00, red_channel_lsb, red_channel_msb};


    uint8_t transaction[] = {PCA9685_PWM, 0x00, 0x10, 0xFF, 0x00,
											0x00, 0x10, 0xFF, 0x00,
											0x00, 0x00, 0xFF, 0x00};
*/

   _dev->transfer(transaction, sizeof(transaction), nullptr, 0);




	/*

	_dev->write_register(0x06, 0x00);
	_dev->write_register(0x07, 0x10);
	_dev->write_register(0x08, 0xFF);
	_dev->write_register(0x09, 0x00);


	_dev->write_register(0x0A, 0x00);
	_dev->write_register(0x0B, 0x00);
	_dev->write_register(0x0C, green_channel_lsb);
	_dev->write_register(0x0D, green_channel_msb);


	_dev->write_register(0x0E, 0x00);
	_dev->write_register(0x0F, 0x00);
	_dev->write_register(0x10, red_channel_lsb);
	_dev->write_register(0x11, red_channel_msb);

*/




return;
	   /*





	   _dev->write_register(0xFA, 0x00);

	  _dev->write_register( 0xFB, 0x10);

	   _dev->write_register(0xFC, 0x00);

	   _dev->write_register(0xFD, 0x00);

*/

}
