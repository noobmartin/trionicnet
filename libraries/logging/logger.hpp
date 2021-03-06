/*
 * Author: Alexander Rajula
 * E-mail: superrajula@gmail.com
 * Description:
 *  This class enables an application to log data.
 *  The data may be output to a terminal, to a file, to a network or all of these combined.
 *
 *  The class may seem bloated, but so what...
 */

#ifndef _logger_hpp_
#define _logger_hpp_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace logging_services{

#define MAX_PREFIX_STRING_LENGTH  256
#define MAX_LOG_STRING_LENGTH     2048
#define MAX_LOG_NUMBER_LENGTH     128
#define MAX_TOTAL_LOG_STRING_LENGTH MAX_PREFIX_STRING_LENGTH+MAX_LOG_STRING_LENGTH

typedef enum{
  Terminal  = 0,
  File      = 1,
  Network   = 2
}Data_Destination_Type;

class logger{
  private:
    unsigned int prefix_length;
    char  prefix_string[MAX_PREFIX_STRING_LENGTH];
    char  log_string[MAX_TOTAL_LOG_STRING_LENGTH];
    char  log_number[MAX_LOG_NUMBER_LENGTH];

    bool  terminal_output_enabled;
    bool  file_output_enabled;
    bool  network_output_enabled;

    bool  file_output_available;
    bool  terminal_output_available;
    bool  network_output_available;

    bool  insert_timestamp;
    bool  insert_prefix;

    int   file_output_descriptor;
    int   network_output_descriptor;

  public:
    logger();
    ~logger();

    /*
     * This function enables setting of a prefix string which will be
     * inserted at the start of recorded strings.
     *
     * The maximum size of the prefix_string has been capped to MAX_PREFIX_STRING_LENGTH bytes.
     *
     * Returns 0 on failure.
     */
    int set_prefix(const char* string, unsigned int string_length);

    /*
     * This function enables selection of the file name which shall be used if outputting data to a file.
     * Returns 0 on failure.
     */
    int set_file_output(const char* name);

    /*
     * These functions enable selection of a destination IP/port pair to which the data shall be transmitted.
     */
    int set_network_output(unsigned int address, unsigned int port);
    int set_network_output(const char* address, unsigned int port);

    /*
     * This function gracefully unhooks file output.
     * Returns 0 on failure.
     */
    int unset_file_output(void);

    /*
     * This function gracefully unhooks network output.
     * Returns 0 on failure.
     */
    int unset_network_output(void);

    /*
     * This function enables insertion of a timestamp in all recorded data.
     */
    void enable_timestamping(void);

    /*
     * This function disables insertion of a timestamp in all recorded data.
     */
    void disable_timestamping(void);

    void enable_prefix(void);

    void disable_prefix(void);

    /*
     * This function turns on distribution of data to a specific type of destination.
     */
    void enable_data_destination(Data_Destination_Type  destination);

    /*
     * This function turns off distribution of data to a specific type of destination.
     */
    void disable_data_destination(Data_Destination_Type destination);

    void log(const char* string);
    void log(const char* string, unsigned int length);
    void log(unsigned int parameter);
    void log(int parameter);
    void log(float parameter);

    /* Logs a byte array one byte at a time as hexadecimal values.  */
    void log_byte_array(const char* byte_array, const unsigned int length);
};

}

#endif
