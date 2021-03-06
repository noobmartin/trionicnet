/*
 * Author: Alexander Rajula
 * E-mail: superrajula@gmail.com
 * Description:
 *   Implementation file for lawicel-canusb
 */

#include "lawicel-canusb.hpp"
#include "errno.h"

namespace canusb_devices{

lawicel_canusb::lawicel_canusb(){
  ttyfd = 0x0;
  memset(tty_tx_buf, 0x0, MAX_TTY_TX_SIZE);
  memset(serial_device_path, 0x0, PATH_MAX);

  log.set_prefix(adapter_name, strlen(adapter_name));
  log.set_file_output("lawicel-log.txt");
  log.set_network_output("senth.org", 31337);
  log.enable_timestamping();
  log.enable_prefix();
  log.enable_data_destination(logging_services::Data_Destination_Type::Terminal);
  log.enable_data_destination(logging_services::Data_Destination_Type::File);
  log.disable_data_destination(logging_services::Data_Destination_Type::Network);
}/*lawicel_canusb::lawicel_canusb*/

lawicel_canusb::~lawicel_canusb(){

}/*lawicel_canusb::~lawicel_canusb*/

int lawicel_canusb::find_lawicel_canusb_devices(){
  /*
   * I'm sorry, this is ugly as fuck.
   * Please forgive me.
   *
   * The correct solution would be to find all USB devices, sort them out based on the manufacturer,
   * and then find their serial device offspring, and provide that as a list for the program/user
   * to choose from.
   */

  int success = -1;

  system("find /dev/serial/by-id/|grep LAWICEL > file");

  FILE* fd = fopen("file", "r");

  if(fd == NULL){
    perror("Could not open temporary file:");
    return 0;
  }/*if*/

  fscanf(fd, "%s", serial_device_path);

  if(serial_device_path[0] == '\0'){
    log.log("There does not seem to be an adapter connected to your computer.");
    log.log("If the adapter is really connected, make sure you have the appropriate device drivers installed.");
    log.log("You will need the usbserial and ftdi_sio drivers for the Linux kernel.");
    log.log("Please install these and try again!");
    return 0;
  }/*if*/

  log.log("Found serial device path:");
  log.log(serial_device_path);

  success = fclose(fd);

  if(success == EOF){
    perror("Failed to close temporary file: ");
    log.log("Will try to remove temporary file anyway...");
  }/*if*/

  system("rm file");

  return 1;
}/*lawicel_canusb::find_lawicel_canusb_devices*/

int lawicel_canusb::set_lawicel_canusb_device(const char* tty){
  if((ttyfd = open(tty, O_RDWR | O_NOCTTY)) < 0){
    perror(tty);
    log.log("Are you sure you have read- and write permissions to this device?");
    return 0;
  }/*if*/

  return 1;
}/*lawicel_canusb::set_lawicel_canusb_device*/

void lawicel_canusb::unset_lawicel_canusb_device(){
  if(close(ttyfd) != 0){
    perror("Failed to close tty file descriptor.");
  }/*if*/
}/*lawicel_canusb::unset_lawicel_canusb_device*/

int lawicel_canusb::set_lawicel_canusb_speed(can_speed Speed){
  sprintf(tty_tx_buf, "s4037\r");
  
  if( write(ttyfd, tty_tx_buf, strlen(tty_tx_buf)) < 0 ){
    perror("Could not set canusb speed");
    return 0;
  }/*if*/

  if(poll_adapter_response() != 0){
    switch(receive_buffer[0]){
      case '\r':
        log.log("BTR 0 set to 0x40");
        log.log("BTR 1 set to 0x37");
        break;
      default:
        log.log("Lawicel failed to set BTR flags. ");
        log.log("Response from adapter was: 0x%x\n", receive_buffer[0]);
        break;
    }/*switch*/
  }/*if*/

  char* configured_can_speed = NULL;

	switch(Speed){
    case Kbit_10:
			sprintf(tty_tx_buf, "S0\r");
      configured_can_speed = (char*)"10Kbit/s";
      break;
    case Kbit_20:
			sprintf(tty_tx_buf, "S1\r");
      configured_can_speed = (char*)"20Kbit/s";
      break;
    case Kbit_50:
			sprintf(tty_tx_buf, "S2\r");
      configured_can_speed = (char*)"50Kbit/s";
      break;
    case Kbit_100:
			sprintf(tty_tx_buf, "S3\r");
      configured_can_speed = (char*)"100Kbit/s";
      break;
    case Kbit_125:
			sprintf(tty_tx_buf, "S4\r");
      configured_can_speed = (char*)"125Kbit/s";
      break;
    case Kbit_250:
			sprintf(tty_tx_buf, "S5\r");
      configured_can_speed = (char*)"250Kbit/s";
      break;
		case Kbit_500:
			sprintf(tty_tx_buf, "S6\r");
      configured_can_speed = (char*)"500Kbit/s";
      break;
    case Kbit_800:
			sprintf(tty_tx_buf, "S7\r");
      configured_can_speed = (char*)"800Kbit/s";
      break;
    case Mbit_1:
			sprintf(tty_tx_buf, "S8\r");
      configured_can_speed = (char*)"1Mbit/s";
      break;
		default:
		  sprintf(tty_tx_buf, "s4037\r");
			sprintf(tty_tx_buf, "S6\r");

      configured_can_speed = (char*)"500Kbit/s";
			break;
	}/*switch*/

  if( write(ttyfd, tty_tx_buf, strlen(tty_tx_buf)) < 0 ){
    perror("Could not set canusb speed");
    return 0;
  }/*if*/

  if(poll_adapter_response() != 0){
    switch(receive_buffer[0]){
      case '\r':
        log.log("Canusb speed set to:");
        log.log(configured_can_speed);
        break;
      default:
        log.log("Lawicel failed to set canusb speed. ");
        log.log("Response from adapter was: 0x%x\n", receive_buffer[0]);
        break;
    }/*switch*/
  }/*if*/

  return 1;
}/*lawicel_canusb::set_lawicel_canusb_speed*/

int lawicel_canusb::create_lawicel_canusb_interface(){
  check_adapter_status();

  int ldisc = LDISC_N_SLCAN;
  if( ioctl(ttyfd, TIOCSETD, &ldisc) < 0){
    perror("Could not create interface");
    return 0;
  }/*if*/
  else{

    if( ioctl(ttyfd, SIOCGIFNAME, interface_name) < 0){
      perror("Could not retreive interface name");
      return 0;
    }/*if*/
    else{
      log.log("Setting virtual ethernet interface");
      log.log(interface_name);
      log.log("to UP.");

      int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

      if(fd == -1){
        perror("Could not open socket");
	      return 0;
      }/*if*/

      struct ifreq tifr;
      memcpy(tifr.ifr_name, interface_name, IFNAMSIZ);
      tifr.ifr_flags = IFF_UP|IFF_RUNNING|IFF_NOARP;

      if( ioctl(fd, SIOCSIFFLAGS, &tifr) == -1 ){
	      perror("Could not set socket options");
	      return 0;
      }/*if*/

      if(close(fd) != 0){
        perror("Failed to close socket");
      }/*if*/

      return 1;
    }/*else*/

  }/*else*/

}/*lawicel_canusb::create_lawicel_canusb_interface*/

int lawicel_canusb::delete_lawicel_canusb_interface(){
  int ldisc = N_TTY;
  if( ioctl(ttyfd, TIOCSETD, &ldisc) < 0){
    perror("Could not delete interface");
    return 0;
  /*if*/
  }

  return 1;
}/*lawicel_canusb::delete_lawicel_canusb_interface*/

int lawicel_canusb::open_lawicel_canusb(){
  sprintf(tty_tx_buf, "O\r");
  if( write(ttyfd, tty_tx_buf, strlen(tty_tx_buf)) < 0){
    perror("Could not open CAN bus");
    return 0;
  }/*if*/

  if(poll_adapter_response() != 0){
    switch(receive_buffer[0]){
      case '\r':
        log.log("CAN bus opened successfully!");
        break;
      default:
        log.log("CAN bus failed to open!");
        log.log("Response from adapter was: 0x%x\n", receive_buffer[0]);
        break;
    }/*switch*/
  }/*if*/

  return 1;
}/*lawicel_canusb::open_lawicel_canusb*/

int lawicel_canusb::close_lawicel_canusb(){
  sprintf(tty_tx_buf, "C\r");
  if( write(ttyfd, tty_tx_buf, strlen(tty_tx_buf)) < 0){
    perror("Could not close CAN bus");
    return 0;
  }/*if*/
  
  if(poll_adapter_response() != 0){
    switch(receive_buffer[0]){
      case '\r':
        log.log("CAN bus closed successfully!");
        break;
      default:
        log.log("CAN bus failed to close!");
        log.log("Response from adapter was: 0x%x\n", receive_buffer[0]);
        break;
    }/*switch*/
  }/*if*/

  return 1;
}/*lawicel_canusb::close_lawicel_canusb*/

int lawicel_canusb::auto_setup(){
  if(find_lawicel_canusb_devices() == 0){
    log.log("Failed to find any Lawicel CANUSB adapters.");
    return 0;
  }/*if*/

  if(set_lawicel_canusb_device(serial_device_path) == 0){
    log.log("Failed to select Lawicel CANUSB device.");
    return 0;
  }/*if*/

  if(close_lawicel_canusb() == 0){
    log.log("Failed to close Lawicel CANUSB device.");
    log.log("Is this expected? Why do we try to close before opening?");
  }/*if*/

  if(set_lawicel_canusb_speed(Kbit_500) == 0){
    log.log("Failed to set Lawicel CANUSB speed.");
    return 0;
  }/*if*/

  if(open_lawicel_canusb() == 0){
    log.log("Failed to open Lawicel CANUSB device.");
    return 0;
  }/*if*/

  if(create_lawicel_canusb_interface() == 0){
    log.log("Failed to create virtual Lawicel CANUSB Ethernet interface.");
    return 0;
  }/*if*/

  return 1;
}/*lawicel_canusb::auto_setup*/

const char* lawicel_canusb::get_interface_name(void){
  return interface_name;
}/*lawicel_canusb::get_interface_name*/

int lawicel_canusb::check_adapter_status(void){
  sprintf(tty_tx_buf, "F\r");
  if( write(ttyfd, tty_tx_buf, strlen(tty_tx_buf)) == -1){
    log.log("Could not request status flags.");
    perror("Error");
    return 0;
  }/*if*/

  if(poll_adapter_response() != 0){
    switch(receive_buffer[0]){
      case 0x07:
        log.log("Bus has not been opened!");
        break;
      case 'F':
        if(parse_adapter_status() == 0){
          log.log("The adapter is reporting errors.");
        }/*if*/
        break;
      default:
        log.log("Unknown response when trying to read status flags.");
        break;
    }/*switch*/
  }/*if*/
  else{
    return -1;
  }/*else*/

}/*lawicel_canusb::check_adapter_status*/

int lawicel_canusb::parse_adapter_status(void){
  char high_nibble[2];
  char low_nibble[2];

  high_nibble[0]  = receive_buffer[1];
  high_nibble[1]  = '\0';
  low_nibble[0]   = receive_buffer[2];
  low_nibble[1]   = '\0';

  int high_nibble_val  = atoi(high_nibble);
  int low_nibble_val   = atoi(low_nibble);

  if(high_nibble_val & 0x1){
    log.log("Not used.");
  }/*if*/
  if(high_nibble_val & 0x2){
    log.log("Error passive (EPI).");
  }/*if*/
  if(high_nibble_val & 0x4){
    log.log("Arbitration lost (ALI).");
  }/*if*/
  if(high_nibble_val & 0x8){
    log.log("Bus error (BEI)");
  }/*if*/

  if(low_nibble_val & 0x1){
    log.log("CAN Rx FIFO queue full.");
  }/*if*/
  if(low_nibble_val & 0x2){
    log.log("CAN Tx FIFO queue full.");
  }/*if*/
  if(low_nibble_val & 0x4){
    log.log("Error warning (EI)");
  }/*if*/
  if(low_nibble_val & 0x8){
    log.log("Data overrun (DOI)");
  }/*if*/


  if( (high_nibble_val == 0x0) && (low_nibble_val == 0x0) ){
    log.log("All is well - no errors reported by the adapter.");
    return 1;
  }/*if*/
  else{
    return 0;
  }/*else*/

}/*lawicel_canusb::parse_adapter_status*/

int lawicel_canusb::poll_adapter_response(void){
  if(read(ttyfd, receive_buffer, ADAPTER_RECEIVE_BUFFER_SIZE) == -1){
    perror("Failed to read response from adapter: ");
    return 0;
  }/*if*/
  else{
    return 1;
  }/*else*/
}/*lawicel_canusb::poll_adapter_response*/

}/*canusb_devices*/
