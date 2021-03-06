#include "can/bus.hpp"
#include "can/trionic5/messages.hpp"
#include "adapters/lawicel-canusb.hpp"

#include <time.h>

int main(){
  unsigned int incoming_frame_id;
  char receive_data[20];
  memset(receive_data, 0x0, 20);

  canusb_devices::lawicel_canusb adapter;
  adapter.auto_setup();

  can::bus canbus;
  canbus.set_name(IFNAMSIZ, adapter.get_interface_name());

  canbus.open();

	struct timespec sleep_time;
	sleep_time.tv_sec = 1;
	sleep_time.tv_nsec = 16700;

  do{
		printf("Requesting AD_trot\n");
		canbus.send(&trionic5net::AD_trot);
		nanosleep(&sleep_time, NULL);
  	//canbus.receive(19, receive_data, &incoming_frame_id);
  	//printf("I read the following from Trionic: %s\n", receive_data);
		
		printf("Requesting Rpm\n");
		canbus.send(&trionic5net::Rpm);
		nanosleep(&sleep_time, NULL);
	//	canbus.receive(19, receive_data, &incoming_frame_id);
  	//printf("I read the following from Trionic: %s\n", receive_data);
  }while(1);

  return 0;
}
