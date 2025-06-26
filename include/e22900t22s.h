/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Introduction
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @file      e22900t22s.h
 * 
 * @version   1.0
 *
 * @date      09-04-2025
 *
 * @brief     LoRa E-Byte E22-900T2SS transceiver driver
 *  
 * @author    Fábio D. Pacheco, 
 * @email     fabio.d.pacheco@inesctec.pt or pacheco.castro.fabio@gmail.com
 *
 * @copyright Copyright (c) [2025] [Fábio D. Pacheco]
 * 
 * @note      Manuals:
 * 
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Definition file
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

#ifndef E22900T22S_H
#define E22900T22S_H

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Imported libraries
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

#include <mixip.h>
#include <serialposix.h>
#include <gpiod.h>

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Data structures
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

 typedef enum{
  E22900T22S_PACKET_32  = 3,
  E22900T22S_PACKET_64  = 2,
  E22900T22S_PACKET_128 = 1,
  E22900T22S_PACKET_240 = 0,
} e22900t22s_packet_size_t;

typedef enum{
  E22900T22S_DBM_22 = 0,
  E22900T22S_DBM_17 = 1,
  E22900T22S_DBM_13 = 2,
  E22900T22S_DBM_10 = 3,
} e22900t22s_transmission_power_t;

typedef enum{
  E22900T22S_WOR_500 =  0,
  E22900T22S_WOR_1000 = 1,
  E22900T22S_WOR_1500 = 2,
  E22900T22S_WOR_2000 = 3,
  E22900T22S_WOR_2500 = 4,
  E22900T22S_WOR_3000 = 5,
  E22900T22S_WOR_3500 = 6,
  E22900T22S_WOR_4000 = 7,
} e22900t22s_wor_cycle_t;

typedef struct{
  uint16_t address;                                         // High byte, example: 0xFF
  uint8_t netid;                                            // Network identification, example: 0xAA
  baudRate_t baudrate;                                      // Baud rate, example: 19200 
  baudRate_t airrate;                                       // Air rate, example: 300
  parity_t parity;                                          // Parity bit, example: 'E'
  e22900t22s_packet_size_t packet_size;                     // Packet size, example: 64
  e22900t22s_transmission_power_t transmit_power;           // Transimit power, example: 13
  uint8_t ambient_noise;                                    // RSSI Ambient noise, ENABLE=1,DISABLE=0 
  uint8_t rssi;                                             // RSSI, ENABLE=1,DISABLE=0
  uint8_t fixed;                                            // Fixed point transmission, ENABLE=1,DISABLE=0
  uint8_t repeater;                                         // Reply, ENABLE=1,DISABLE=0
  uint8_t lbt;                                              // LBT byte, ENABLE=1,DISABLE=0
  uint8_t wor;                                              // WOR transceiver control, WOR trans=1,WOR rec=0
  e22900t22s_wor_cycle_t wor_cycle;                         // WOR cycle, example: 3500 
  uint8_t channel;                                          // Channel, example: 23
  uint16_t encryption;                                      // Symmetrical encryption key 
  uint8_t pid[7];                                           // Product Information
} e22900t22s_eeprom_t;

typedef struct{
  uint8_t offset;
  struct gpiod_line * ptr;
} gpiod_line2_t;

typedef struct{
  char name[NAME_MAX];
  struct gpiod_chip * ptr;
} gpiod_chip2_t; 

typedef struct{
  gpiod_line2_t m0;
  gpiod_line2_t m1;
  gpiod_line2_t aux;
  gpiod_chip2_t chip;
} e22900t22s_pinmode_t;

typedef struct{
  e22900t22s_eeprom_t  cfg;
  serial_manager_t     *serial;
  e22900t22s_pinmode_t gpio;
} e22900t22s_t;

// Mode switching can only be valid when AUX output is 1, otherwise it will delay switching.
// If AUX output is 1 then after the switching the the module is idle for 2 ms.
typedef enum{
  E22900T22S_MODE_NORMAL,    // UART and wireless channel are open, transparent transmission is on, Supports configuration over air via special command
  E22900T22S_MODE_WOR,       // Can be defined as WOR transmitter and WOR receiver, Supports wake up over air
  E22900T22S_MODE_CONFIG,    // Users can access the register through the serial port to control the working state of the module
  E22900T22S_MODE_SLEEP,     // Sleep mode
} e22900t22s_mode_t;

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * EEPROM Memory Spaces
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

typedef enum{
  // Memory address
  E22900T22S_MEM_ADDH    = 0x00,
  E22900T22S_MEM_ADDL    = 0x01,
  E22900T22S_MEM_NETID   = 0x02,
  E22900T22S_MEM_REG0    = 0x03,
  E22900T22S_MEM_REG1    = 0x04,
  E22900T22S_MEM_REG2    = 0x05,
  E22900T22S_MEM_REG3    = 0x06,
  E22900T22S_MEM_CRYPTH  = 0x07,
  E22900T22S_MEM_CRYPTL  = 0x08,
  E22900T22S_MEM_PID     = 0x80,

  // Single byte shift
  E22900T22S_SHF_UART    = 5,
  E22900T22S_SHF_PARITY  = 3,
  E22900T22S_SHF_AIRDATA = 0,
  E22900T22S_SHF_PKTSZ   = 6,
  E22900T22S_SHF_AMBNS   = 5,
  E22900T22S_SHF_POWER   = 0,
  E22900T22S_SHF_RSSI    = 7,
  E22900T22S_SHF_FIXED   = 6,
  E22900T22S_SHF_REPLY   = 5,
  E22900T22S_SHF_LBT     = 4,
  E22900T22S_SHF_WOR     = 3,
  E22900T22S_SHF_WORCYC  = 0,

  // Register commands
  E22900T22S_SET_REG     = 0xC0,
  E22900T22S_READ_REG    = 0xC1,
  E22900T22S_SET_TMP_REG = 0xC2,
  E22900T22S_WIRELESS    = 0xCF,

  // Lookup tables size
  E22900T22S_LUT_SIZE_UART    = 8,
  E22900T22S_LUT_SIZE_PARITY  = 4,
  E22900T22S_LUT_SIZE_AIRRATE = 8,
  E22900T22S_LUT_SIZE_PACKET  = 4,
  E22900T22S_LUT_SIZE_POWER   = 4,
  E22900T22S_LUT_SIZE_WORCYCLE= 8,

  // Others
  E22900T22S_PID_SIZE = 7,
} e22900t22s_eeprom_mem_t;

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Lookup tables
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

 typedef struct{
  uint8_t    bin;
  char       text[32];
  baudRate_t code;
} lut_baudrate_t;

static const 
lut_baudrate_t lut_baudrate[ E22900T22S_LUT_SIZE_UART ] = {
  {0, "1200"  , B1200},
  {1, "2400"  , B2400},
  {2, "4800"  , B4800},
  {3, "9600"  , B9600},
  {4, "19200" , B19200},
  {5, "38400" , B38400},
  {6, "57600" , B57600},
  {7, "115200", B115200},
};

typedef struct{
  uint8_t  bin;
  char     text[32];
  parity_t code;
} lut_parity_t;

static const 
lut_parity_t lut_parity[ E22900T22S_LUT_SIZE_PARITY ] = {
  {0, "8N1", BPARITY_NONE},
  {1, "8O1", BPARITY_ODD},
  {2, "8E1", BPARITY_EVEN},
};

typedef struct{
  uint8_t    bin;
  char       text[32];
  baudRate_t code;
} lut_airrate_t;

static const 
lut_airrate_t lut_airrate[ E22900T22S_LUT_SIZE_AIRRATE ] = {
  {0, "300"  , B300},
  {1, "1200" , B1200},
  {2, "2400" , B2400},
  {3, "4800" , B4800},
  {4, "9600" , B9600},
  {5, "19200", B19200},
  {6, "38400", B38400},
  {7, "62500", B62500},
};

typedef struct{
  char                     text[32];
  e22900t22s_packet_size_t code;
} lut_packet_t;

static const 
lut_packet_t lut_packetsize[ E22900T22S_LUT_SIZE_PACKET ] = {
  {"240", E22900T22S_PACKET_240},
  {"128", E22900T22S_PACKET_128},
  {"64",  E22900T22S_PACKET_64},
  {"32",  E22900T22S_PACKET_32},
};

typedef struct{
  char                            text[32];
  e22900t22s_transmission_power_t code;
} lut_power_t;

static const 
lut_power_t lut_power[ E22900T22S_LUT_SIZE_POWER ] = {
  {"22", E22900T22S_DBM_22},
  {"17", E22900T22S_DBM_17},
  {"13", E22900T22S_DBM_13},
  {"10", E22900T22S_DBM_10},
};

typedef struct{
  char                   text[32];
  e22900t22s_wor_cycle_t code;
} lut_worcycle_t;

static const 
lut_worcycle_t lut_worcycle[ E22900T22S_LUT_SIZE_WORCYCLE ] = {
  {"500" , E22900T22S_WOR_500 },
  {"1000", E22900T22S_WOR_1000},
  {"1500", E22900T22S_WOR_1500},
  {"2000", E22900T22S_WOR_2000},
  {"2500", E22900T22S_WOR_2500},
  {"3000", E22900T22S_WOR_3000},
  {"3500", E22900T22S_WOR_3500},
  {"4000", E22900T22S_WOR_4000},
};

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Prototypes
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

 /**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration with the object internal configuration.
 *  
 * @param[in] dev The E22900T22S object used to access the serial port and the mode pins.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_update_eeprom( e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the configuration structure passed as argument, depending on the flags.
 *  
 * @param[in] config A structure with the new EEPROM configuration parameters.
 * @param[in] update If `update` is greater than 0 it will update the eeprom in the end of the function, otherwise the user has to call `e22900t22s_update_eeprom` after.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM configuration filled with the `config` parameters.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_config( const e22900t22s_eeprom_t * config, const uint8_t update, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S pinout configuration or just the E22900T22S object based on the configuration structure passed as argument, depending on the flags.
 *  
 * @param[in] pinout A structure with the new pinout configuration.
 * @param[out] dev The E22900T22S object, upon success it will have its GPIO configuration filled with the `pinout` parameters.
 * 
 * @return Upon success, it will update the GPIO parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_pinout( const e22900t22s_pinmode_t * pinout, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Loads E22900T22S EEPROM and pinout configuration from configuration XML file.
 *  
 * @param[in] filename The path to the configuration file.
 * @param[out] config The new filled configuration loadded.
 * @param[out] pinout The new filled pinout configuration loadded.
 * 
 * @return Upon success, it will fill the `config` struct, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_load_config( const char * filename, e22900t22s_eeprom_t * config, e22900t22s_pinmode_t * pinout );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration structure on the E22900T22S object to the default values by the manufacturer.
 *  
 * @param[in] update If `update` is greater than 0 it will update the eeprom in the end of the function, otherwise the user has to call `e22900t22s_update_eeprom` after.
 * @param[in] dev The E22900T22S object used to access the serial port and the mode pins.
 * 
 * @return Upon success, it will update the configuration parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_default_config( const uint8_t update, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the address passed as argument, depending on the flags.
 *  
 * @param[in] address The address of the device, 2 bytes long.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM address configuration filled with the `adresss` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_address_identification( const uint16_t address, e22900t22s_t * dev );


/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the network address passed as argument, depending on the flags.
 *  
 * @param[in] address The network address identification of the device, 1 byte long.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM network identifications configuration filled with the `adresss` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_network_identification( const uint8_t address, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the baud rate passed as argument, depending on the flags.
 *  
 * @param[in] baudrate The baud rate of the device.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM baudrate configuration filled with the `baudrate` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_baudrate( const baudRate_t baudrate, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the air data rate passed as argument, depending on the flags.
 *  
 * @param[in] airrate The air data rate of the device.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM airrate configuration filled with the `airrate` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_airrate( const baudRate_t airrate, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the parity passed as argument, depending on the flags.
 *  
 * @param[in] parity The parity bit option of the device.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM parity configuration filled with the `parity` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_parity( const parity_t parity, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the packet size passed as argument, depending on the flags.
 *  
 * @param[in] size The packet size for the LoRa burst message.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM packet_size configuration filled with the `size` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_packet_size( const e22900t22s_packet_size_t size, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the transmission power passed as argument, depending on the flags.
 *  
 * @param[in] power The transmission power for the module's antenna.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM transmission_power configuration filled with the `power` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_transmission_power( const e22900t22s_transmission_power_t power, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the wor state passed as argument, depending on the flags.
 *  
 * @param[in] option Enable Ultra-low power consumption (1), WOR monitors the overall power consumption of the adjustable module, introduces maximum configurable response delay of 4 s. \n
 *                   Disable (0), normal operation.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM wor configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_wor( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the wor cycle passed as argument, depending on the flags.
 *  
 * @param[in] cycle The longer the WOR monitoring interval period, the lower the average power consumption, but the greater the data delay. period = T = (1 + WOR) x 500 ms \n
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM channel configuration filled with the `channel` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_wor_cycle( const e22900t22s_wor_cycle_t cycle, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the ambient noise state passed as argument, depending on the flags.
 *  
 * @param[in] option Enable ambient noise signal strength over serial (1), it can be used to implement listen before talk functions manually. \n
 *                   Disable (0), normal operation.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM ambient noise configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_ambient_noise( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the rssi state passed as argument, depending on the flags.
 *  
 * @param[in] option Enable signal strength indication function over serial (1), it can be used to assess signal quality, improve communication networks, and ranging. \n
 *                   Disable (0), normal operation.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM rssi configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_rssi( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the fixed point state passed as argument, depending on the flags.
 *  
 * @param[in] option The host can transmit data to any address, any channel, to achieve networking, repeater and other application (1). \n
 *                   Disable (0), broadcast on a single channel always.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM fixed_point configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_fixed_point( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the repeater state passed as argument, depending on the flags.
 *  
 * @param[in] option The repeater can forward data in both directions between two NETIDs (1). \n
 *                   Disable (0), broadcast on a single channel always.
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM repeater configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_repeater( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the listen before talk state passed as argument, depending on the flags.
 *  
 * @param[in] option The module actively monitors the channel environment noise before transmitting, sort of a carrier sense (1). \n
 *                   Disable (0), just sends it, like ALOHA.
 *   
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM lbt configuration filled with the `option` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_listen_before_talk( const uint8_t option, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the channel passed as argument, depending on the flags.
 *  
 * @param[in] channel The channel used for the communication, ranging between 0 - 80, 81 channels: carrier_freq = 850.125 MHz + channel x 1 MHz.
 *   
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM channel configuration filled with the `channel` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_channel( const uint8_t channel, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Updates the E22900T22S EEPROM configuration or just the E22900T22S object based on the encryption key passed as argument, depending on the flags.
 *  
 * @param[in] key The symmetrical encryption key for a peer to peer communication.
 *   
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM encryption key configuration filled with the `key` parameter.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_encryption_key( const uint16_t key, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Gets the E22900T22S EEPROM configuration and fills the configuration structure inside the device object passed as argument.
 *  
 * @param[out] dev The E22900T22S object, upon success it will have its EEPROM configuration filled with the current device parameters.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_get_config( e22900t22s_t * dev );


/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Print the paramters inside the E22900T22S object passed as argument.
 *  
 * @param[in] console If greater than 0 it will print the object configuration on the console.
 * @param[in] dev The E22900T22S object to print.
 * 
 * @return Upon success, it will update the EEPROM parameters, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * e22900t22s_print_config( uint8_t console, e22900t22s_t * dev );


/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Tries to find the RSSI in the passed array `data`, it increments the counter 0 for each 0 (mixip format), so it can still find the RSSI in multiple arrays.
 *  
 * @param[in] data The data array received from the serial port.
 * @param[out] rssi The RSSI array that will be filled with the RSSI values found on the `data`. 
 * @param[out] rssilen The number of RSSI values found and stored inside the RSSI array.
 * @param[out] dev The E22900T22S object because of the counter.
 * 
 * @return Upon success finding the RSSI it returns 0. \n 
 *         If no error was found but no RSSI was found either returns 1. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_get_rssi( const buffer_t * data, uint8_t * rssi, uint8_t * rssilen, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Tries to find the ambient noise in the passed array `data`, it increments the counter 0 for each 0 (mixip format), so it can still find the ambient noise in multiple arrays.
 *  
 * @param[in] data The data array received from the serial port.
 * @param[out] ambnoise The RSSI array that will be filled with the RSSI values found on the `data`. 
 * @param[out] ambnoiselen The number of RSSI values found and stored inside the RSSI array.
 * @param[out] dev The E22900T22S object because of the counter.
 * 
 * @return Upon success finding the Ambient Noise it returns 0. \n 
 *         If no error was found but no Ambient Noise was found either returns 1. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_get_ambient_noise( const buffer_t * data, uint8_t * ambnoise, uint8_t * ambnoiselen, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Sets up the GPIO chip name.
 *  
 * @param[in] chip_name The chip name (e.g., gpiochip0)
 * @param[in] len The chip name length.
 * @param[out] dev The E22900T22S object that will get its internal pin structure updated.
 * 
 * @return Upon success, it will setup the chip name, and return 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_gpio_chip_name( const char * chip_name, const uint8_t len, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Sets up the E22900T22S device's operational mode, it might hold the program until it does so.
 *  
 * @param[in] mode The mode that the E22900T22S should be working (`e22900t22s_mode_t`).
 * @param[out] dev The E22900T22S object that will get its internal pin structure updated.
 * 
 * @return Upon success, it will change to the requested mode, and return 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_set_mode( const e22900t22s_mode_t mode , e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Sets up the E22900T22S device's GPIO.
 *  
 * @param[in] chip_name The gpio chip name.
 * @param[in] m0 The m0 gpio pin number.
 * @param[in] m1 The m1 gpio pin number.
 * @param[in] aux The aux gpio pin number.
 * @param[out] dev The E22900T22S object that will get its internal pin structure updated.
 * 
 * @return Upon success, it will initialize the pins and being able to change between modes, and return 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_gpio_init( const char * chip_name, uint8_t m0, uint8_t m1, uint8_t aux, e22900t22s_t * dev );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Closes E22900T22S device's GPIO connection.
 *  
 * @param[in] dev The E22900T22S object that will get its internal pin structure closed.
 * 
 * @return Upon success, it will close the GPIO request, and return 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_gpio_close( e22900t22s_t * dev );

#endif

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * End file
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
