/*
 * Author: Alexander Rajula
 * E-mail: superrajula@gmail.com
 * Description:
 *  This class tries to provide a high-level abstraction layer and helper functions for controlling the Lawicel CANUSB adapter.
 *  In essence, what this class does is to find any attached Lawicel CANUSB adapter, and then transform that adapter into a
 *  regular network interface adapter.
 *
 *  This makes it possible to use the regular socket mechanisms to transmit data to and receive data from a CAN.
 *
 * Kudos to:
 *  The guys and gals behind slcan_attach.c - a part of the set of userspace tools
 *  for serial line CAN interface driver SLCAN.
 */

#ifndef _lawicel_canusb_h
#define _lawicel_canusb_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <termios.h>
#include <netinet/in.h>
#include <linux/limits.h>
#include "../logging/logger.hpp"

namespace canusb_devices{

#define MAX_TTY_TX_SIZE             256
#define LDISC_N_SLCAN               17
#define ADAPTER_RECEIVE_BUFFER_SIZE 128

typedef enum can_speed{
	Kbit_10,
	Kbit_20,
	Kbit_50,
	Kbit_100,
	Kbit_125,
	Kbit_250,
	Kbit_500,
	Kbit_800,
	Mbit_1
} can_speed;

class lawicel_canusb{
  private:
    int ttyfd;
    char tty_tx_buf[MAX_TTY_TX_SIZE];
    char serial_device_path[PATH_MAX];
    char interface_name[IFNAMSIZ];
    char receive_buffer[ADAPTER_RECEIVE_BUFFER_SIZE];
    const char* adapter_name = "Lawicel CAN-USB adapter";

    logging_services::logger  log;

    int poll_adapter_response(void);
    int parse_adapter_status(void);

  public:
    lawicel_canusb();
    ~lawicel_canusb();

    /*
     * This function probes the system to find any attached LAWICEL CANUSB dongles.
     * Returns 0 on failure.
     */
    int find_lawicel_canusb_devices();

    /*
     * This is used to select one device from the device found by find_lawicel_canusb_devices
     * find_lawicel_canusb_devices()
     * Returns 0 on failure.
     */
    int set_lawicel_canusb_device(const char* tty);

    /*
     * This function sets the correct speed and TTY options to facilitate communication.
     * Returns 0 on failure.
     */
    int set_lawicel_canusb_speed(can_speed Speed);

    /* 
     * This function will transform the serial interface to an ethernet interface.
     * Returns 0 on failure.
     */
    int create_lawicel_canusb_interface();

    /*
     * This function will remove the previously created ethernet interface.
     * Returns 0 on failure.
     */
    int delete_lawicel_canusb_interface();

    /*
     * Once the LAWICEL CANUSB dongle has been found, the communication bus must be opened before use.
     * Returns 0 on failure.
     */
    int open_lawicel_canusb();

    /*
     * Use this to close CAN communication to Trionic.
     * Returns 0 on failure.
     */
    int close_lawicel_canusb();

    void unset_lawicel_canusb_device();

    /*
     * Only use this if you are a lazy SOB.
     * Returns 0 on failure.
     */
    int auto_setup();

    /*
     * Use this if you want a pointer to the Ethernet interface name the Lawicel CANUSB adapter is mapped to.
     */
    const char* get_interface_name(void);

    /* This may be used before calling create_lawicel_canusb_interface, but not after. 
     * Once that function has been called, the file descriptor will be associated with an "Ethernet"
     * interface, and cannot be communicated with using regular read/write operations
     */
    int check_adapter_status(void);
};

}

#endif
