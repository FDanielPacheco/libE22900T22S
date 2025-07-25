/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Introduction
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @file      e22900t22s/mixip.h
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

#ifndef E22900T22S_MIXIP_H
#define E22900T22S_MIXIP_H

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Imported libraries
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

#include <mixip.h>

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Data Structures
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

typedef enum{
  E22900T22S_DEF_RB  = 1,
  E22900T22S_DEF_SLS = 32,
} e22900t22s_mixip_default_t;

typedef struct{
  translator_parameters_t   tmp;
  translator_parameters_t * ptr;
} e22900t22s_mixip_t;

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Prototypes
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Connects to the translator associated with the current MIXIP-Instance. \n 
 *        So it can update the parameters of the translator, such as: The #slots in the ring buffer, the size of each segment during the segmantation.
 *  
 * @param[in] name The path to the shared memory object.
 * @param[out] config The new filled configuration object.
 * 
 * @return If the driver is configured as transmitter or default, upon success, it will fill the `config` struct, and return 0. \n
 *         If the driver is configured as receiver it will return 1. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_connect_mixip( const char * name, e22900t22s_mixip_t * config );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Loads the E22900T22S Translator parameters from configuration XML file.
 *  
 * @param[in] filename The path to the configuration file.
 * @param[out] config The new filled configuration loadded.
 * 
 * @return Upon success, it will fill the `config` struct, and it returns 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_load_mixip_config( const char * filename, e22900t22s_mixip_t * config );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @brief Attempts to update the translator with the parameters loaded into the `config` struct.
 *  
 * @param[in] config The new configuration of the Translator.
 * 
 * @return Upon success, it will update the Translator, and return 0. \n
 *         Otherwise, -1 is returned and `errno` is set to indicate the error.
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t e22900t22s_update_mixip_config( const e22900t22s_mixip_t * config );

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

#endif

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * End file
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
