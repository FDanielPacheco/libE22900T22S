/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Introduction
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/**********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************//**
 * @file      e22900t22s.c
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
 * Imported libraries
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

#include <e22900t22s/core.h>
#include <e22900t22s/metrics.h>
#include <e22900t22s/mixip.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Prototypes
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

int8_t update_parameter( e22900t22s_t * dev, void * value, size_t dimension, size_t offset, uint8_t cfg );
int8_t check( e22900t22s_t * dev );

uint8_t lookup_table_baudrate_2bin( const baudRate_t baud_code );
const char * lookup_table_baudrate_2text( const baudRate_t baud_code );
baudRate_t lookup_table_baudrate_2code( const uint8_t baud_bin );
baudRate_t lookup_table_baudrate_fromtext_2code( const char * baud_text );

uint8_t lookup_table_parity_2bin( const parity_t parity_code );
const char * lookup_table_parity_2text( const parity_t parity_code );
parity_t lookup_table_parity_2code( const uint8_t parity_bin );
parity_t lookup_table_parity_fromtext_2code( const char * parity_text );

uint8_t lookup_table_airrate_2bin( const baudRate_t baud_code );
const char * lookup_table_airrate_2text( const baudRate_t baud_code );
baudRate_t lookup_table_airrate_2code( const uint8_t baud_bin );
baudRate_t lookup_table_airrate_fromtext_2code( const char * baud_text );

const char * lookup_table_packet_2text( const e22900t22s_packet_size_t code );
e22900t22s_packet_size_t lookup_table_packet_fromtext( const char * size_text );

const char * lookup_table_power_2text( const e22900t22s_transmission_power_t code );
e22900t22s_transmission_power_t lookup_table_power_fromtext( const char * power_text );

const char * lookup_table_worcycle_2text( const e22900t22s_wor_cycle_t code );
e22900t22s_wor_cycle_t lookup_table_worcycle_fromtext( const char * cycle_text );

int8_t gpiod_init( const char * name, gpiod_chip2_t * chip );
int8_t gpiod_pin_mode( gpiod_chip2_t * chip, gpiod_line2_t * gpio, uint8_t direction );
int8_t gpiod_digital_write( gpiod_line2_t * gpio, uint8_t value );
int8_t gpiod_digital_read( gpiod_line2_t * gpio );

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Global variables
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

uint32_t delay_us = 100;

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * Functions
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
check( e22900t22s_t * dev ){
  if( !dev ){
    errno = EINVAL;
    return 0;
  }
  return 1;  
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
update_parameter( e22900t22s_t * dev, void * value, size_t dimension, size_t offset, uint8_t cfg  ){
  if( !dev || !value ){
    errno = EINVAL;
    return -1;
  }
  
  uint8_t * struct_field = NULL;
  if( cfg )
    struct_field  = (uint8_t *) &( dev->cfg ) + offset;
  else
    struct_field = (uint8_t *) &( dev->gpio ) + offset;
  
  if( !struct_field ){
    errno = EADDRNOTAVAIL;
    return -1;
  }

  memcpy( struct_field, (uint8_t *) value, dimension );
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_address_identification( const uint16_t _address, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_address, sizeof(uint16_t), offsetof(e22900t22s_eeprom_t, address), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t
e22900t22s_set_network_identification( const uint8_t _address, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_address, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, netid), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_baudrate( const baudRate_t _baudrate, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_baudrate, sizeof(baudRate_t), offsetof(e22900t22s_eeprom_t, baudrate), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_airrate( const baudRate_t _airrate, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_airrate, sizeof(baudRate_t), offsetof(e22900t22s_eeprom_t, airrate), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_parity( const parity_t _parity, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_parity, sizeof(parity_t), offsetof(e22900t22s_eeprom_t, parity), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_packet_size( const e22900t22s_packet_size_t _size, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_size, sizeof(e22900t22s_packet_size_t), offsetof(e22900t22s_eeprom_t, packet_size), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_transmission_power( const e22900t22s_transmission_power_t _power, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_power, sizeof(e22900t22s_transmission_power_t), offsetof(e22900t22s_eeprom_t, transmit_power), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_wor( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, wor), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_wor_cycle( const e22900t22s_wor_cycle_t _cycle, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_cycle, sizeof(e22900t22s_wor_cycle_t), offsetof(e22900t22s_eeprom_t, wor_cycle), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_ambient_noise( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, ambient_noise), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_rssi( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, rssi), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_fixed_point( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, fixed), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_repeater( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, repeater), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_listen_before_talk( const uint8_t _option, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_option, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, lbt), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_channel( const uint8_t _channel, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_channel, sizeof(uint8_t), offsetof(e22900t22s_eeprom_t, channel), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_encryption_key( const uint16_t _key, e22900t22s_t * dev ){
  return update_parameter( dev, (void *) &_key, sizeof(uint16_t), offsetof(e22900t22s_eeprom_t, encryption), 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_config( const e22900t22s_eeprom_t * config, const uint8_t update, e22900t22s_t * dev ){
  if( !config || !dev ){
    errno = EINVAL;
    return -1;
  }
  memcpy( &dev->cfg, config, sizeof(e22900t22s_eeprom_t) );

  if( update )
    return e22900t22s_update_eeprom( dev );
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_pinout( const e22900t22s_pinmode_t * pinout, e22900t22s_t * dev ){
  if( !pinout || !dev ){
    errno = EINVAL;
    return -1;
  }
  // printf("Chip:%s, %d, %d, %d\n", pinout->chip.name, pinout->m0.offset, pinout->m1.offset, pinout->aux.offset );
  return e22900t22s_gpio_init( pinout->chip.name, pinout->m0.offset, pinout->m1.offset, pinout->aux.offset, dev );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_load_config( const char * filename, e22900t22s_eeprom_t * config, e22900t22s_pinmode_t * pinout ){
  if( !pinout || !config || !filename ){
    errno = EINVAL;
    return -1;
  }
  
  xmlDoc * docfile = xmlReadFile( filename, NULL, 0 );
  if( !docfile ){
    errno = EINVAL;
    perror("xmlReadFile");
    return -1;
  }
  
  xmlNode * root_element = xmlDocGetRootElement(docfile);
  if( !root_element ){
    errno = EINVAL;
    perror("xmlDocGetRootElement");
    xmlFreeDoc( docfile );
    return -1;
  }

  memset( config, 0, sizeof(e22900t22s_eeprom_t) );
  memset( pinout, 0, sizeof(e22900t22s_pinmode_t) );

  xmlNode * address = NULL;
  xmlNode * network = NULL;
  xmlNode * serial = NULL;
  xmlNode * rf = NULL;
  xmlNode * pin = NULL;

  if( !strcmp( (char *) root_element->name, "e22900t22s" ) ){
    for( xmlNode * current_node = root_element->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "address" ) )
          address = current_node;
        if( !strcmp( (char *) current_node->name, "network" ) )
          network = current_node;
        if( !strcmp( (char *) current_node->name, "serial" ) )
          serial = current_node;
        if( !strcmp( (char *) current_node->name, "rf" ) )
          rf = current_node;        
        if( !strcmp( (char *) current_node->name, "pin" ) )
          pin = current_node;        
      }        
    }
  }  
    
  if( NULL != address )
    config->address = (uint16_t) atoi( (const char *) xmlNodeGetContent( address ) );

  if( NULL != network ){
    xmlNode * id = NULL;
    xmlNode * key = NULL;

    for( xmlNode * current_node = network->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "id" ) )
          id = current_node;        
        if( !strcmp( (char *) current_node->name, "key" ) )
          key = current_node;        
      }
    }

    if( NULL != id )
      config->netid = (uint8_t) atoi( (const char *) xmlNodeGetContent( id ) );
    if( NULL != key )
      config->encryption = (uint16_t) atoi( (const char *) xmlNodeGetContent( key ) );
  }

  if( NULL != serial ){
    xmlNode * baudrate;
    xmlNode * parity;

    for( xmlNode * current_node = serial->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "baudrate" ) )
          baudrate = current_node;        
        if( !strcmp( (char *) current_node->name, "parity" ) )
          parity = current_node;        
      }
    }    

    if( NULL != baudrate )
      config->baudrate = lookup_table_baudrate_fromtext_2code( (const char *) xmlNodeGetContent( baudrate ) );
    if( NULL != parity )
      config->parity = lookup_table_parity_fromtext_2code( (const char *) xmlNodeGetContent( parity ) );    
  }

  if( NULL != rf ){
    xmlNode * baudrate = NULL;
    xmlNode * size = NULL;
    xmlNode * power = NULL;
    xmlNode * channel = NULL;
    xmlNode * modes = NULL;
    xmlNode * stats = NULL;

    for( xmlNode * current_node = rf->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "baudrate" ) )
          baudrate = current_node;        
        if( !strcmp( (char *) current_node->name, "size" ) )
          size = current_node;        
        if( !strcmp( (char *) current_node->name, "power" ) )
          power = current_node;        
        if( !strcmp( (char *) current_node->name, "channel" ) )
          channel = current_node;        
        if( !strcmp( (char *) current_node->name, "modes" ) )
          modes = current_node;        
        if( !strcmp( (char *) current_node->name, "stats" ) )
          stats = current_node;        
      }
    }

    if( NULL != baudrate )
      config->airrate = lookup_table_airrate_fromtext_2code( (const char *) xmlNodeGetContent( baudrate ) );
    if( NULL != size )
      config->packet_size = lookup_table_packet_fromtext( (const char *) xmlNodeGetContent( size ) );
    if( NULL != power )
      config->transmit_power = lookup_table_power_fromtext( (const char *) xmlNodeGetContent( power ) );
    if( NULL != channel )
      config->channel = (uint8_t) atoi( (const char *) xmlNodeGetContent( channel ) );

    if( NULL != modes ){
      xmlNode * fixed = NULL;
      xmlNode * repeater = NULL;
      xmlNode * lbt = NULL;
      xmlNode * wor = NULL;

      for( xmlNode * current_node = modes->children ; current_node != NULL ; current_node = current_node->next ){
        if( XML_ELEMENT_NODE == current_node->type ){
          if( !strcmp( (char *) current_node->name, "fixed" ) )
            fixed = current_node;        
          if( !strcmp( (char *) current_node->name, "repeater" ) )
            repeater = current_node;        
          if( !strcmp( (char *) current_node->name, "lbt" ) )
            lbt = current_node;        
          if( !strcmp( (char *) current_node->name, "wor" ) )
            wor = current_node;        
        }
      }

      if( NULL != fixed )
        config->fixed = !atoi( (const char *) xmlNodeGetContent( fixed ) ) ? 0 : 1;
      if( NULL != repeater )
        config->repeater = !atoi( (const char *) xmlNodeGetContent( repeater ) ) ? 0 : 1;
      if( NULL != lbt )
        config->lbt = !atoi( (const char *) xmlNodeGetContent( lbt ) ) ? 0 : 1;

      if( NULL != wor ){
        xmlNode * state;
        xmlNode * cycle;

        for( xmlNode * current_node = wor->children ; current_node != NULL ; current_node = current_node->next ){
          if( XML_ELEMENT_NODE == current_node->type ){
            if( !strcmp( (char *) current_node->name, "state" ) )
              state = current_node;        
            if( !strcmp( (char *) current_node->name, "cycle" ) )
              cycle = current_node;        
          }
        }

        if( NULL != state )
          config->wor = !atoi( (const char *) xmlNodeGetContent( state ) ) ? 0 : 1;
        if( NULL != cycle )
          config->wor_cycle = lookup_table_worcycle_fromtext( (const char *) xmlNodeGetContent( cycle ) );
      }
    }

    if( NULL != stats ){
      xmlNode * rssi;
      xmlNode * noise;

      for( xmlNode * current_node = stats->children ; current_node != NULL ; current_node = current_node->next ){
        if( XML_ELEMENT_NODE == current_node->type ){
          if( !strcmp( (char *) current_node->name, "rssi" ) )
            rssi = current_node;        
          if( !strcmp( (char *) current_node->name, "noise" ) )
            noise = current_node;        
        }
      }

      if( NULL != rssi )
        config->rssi = !atoi( (const char *) xmlNodeGetContent( rssi ) ) ? 0 : 1;
      if( NULL != noise )
        config->ambient_noise = !atoi( (const char *) xmlNodeGetContent( noise ) ) ? 0 : 1;
    }
  }

  if( NULL != pin ){
    xmlNode * chip;
    xmlNode * aux;
    xmlNode * m0;
    xmlNode * m1;

    for( xmlNode * current_node = pin->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "chip" ) )
          chip = current_node;        
        if( !strcmp( (char *) current_node->name, "aux" ) )
          aux = current_node;        
        if( !strcmp( (char *) current_node->name, "m0" ) )
          m0 = current_node;        
        if( !strcmp( (char *) current_node->name, "m1" ) )
          m1 = current_node;        
      }
    }

    if( NULL != chip )
      strncpy( pinout->chip.name, (const char *) xmlNodeGetContent( chip ), NAME_MAX );
    if( NULL != aux )
      pinout->aux.offset = (uint8_t) atoi( (const char *) xmlNodeGetContent( aux ) );   
    if( NULL != m0 )
      pinout->m0.offset = (uint8_t) atoi( (const char *) xmlNodeGetContent( m0 ) );   
    if( NULL != m1 )
      pinout->m1.offset = (uint8_t) atoi( (const char *) xmlNodeGetContent( m1 ) );   
  }

  xmlFreeDoc( docfile );
  xmlCleanupParser( );
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_default_config( const uint8_t update, e22900t22s_t * dev ){
  if( !check( dev ) )
    return -1;
  
  memset( &dev->cfg, 0, sizeof(e22900t22s_eeprom_t) );
  dev->cfg.baudrate = B9600;
  dev->cfg.airrate = B2400;
  dev->cfg.parity = BPARITY_NONE;
  dev->cfg.packet_size = E22900T22S_PACKET_240;
  dev->cfg.transmit_power = E22900T22S_DBM_22;
  dev->cfg.wor_cycle = E22900T22S_WOR_2000;
  dev->cfg.channel = 0x32;

  if( update )
    return e22900t22s_update_eeprom( dev );
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
lookup_table_baudrate_2bin( const baudRate_t baud_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_UART ; ++i )
    if( lut_baudrate[ i ].code == baud_code )
      return lut_baudrate[ i ].bin;
  return 0xFF;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_baudrate_2text( const baudRate_t baud_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_UART ; ++i )
    if( lut_baudrate[ i ].code == baud_code )
      return lut_baudrate[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
baudRate_t 
lookup_table_baudrate_2code( const uint8_t baud_bin ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_UART ; ++i )
    if( lut_baudrate[ i ].bin == baud_bin )
      return lut_baudrate[ i ].code;
  return 0xFF;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
baudRate_t 
lookup_table_baudrate_fromtext_2code( const char * baud_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_UART ; ++i )
    if( !strcmp( lut_baudrate[ i ].text, baud_text ) )
      return lut_baudrate[ i ].code;
  return B9600;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
lookup_table_parity_2bin( const parity_t parity_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PARITY ; ++i )
    if( lut_parity[ i ].code == parity_code )
      return lut_parity[ i ].bin;
  return 0xFF;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_parity_2text( const parity_t parity_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PARITY ; ++i )
    if( lut_parity[ i ].code == parity_code )
      return lut_parity[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
parity_t 
lookup_table_parity_2code( const uint8_t parity_bin ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PARITY ; ++i )
    if( lut_parity[ i ].bin == parity_bin )
      return lut_parity[ i ].code;
  return 0xFF;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
parity_t 
lookup_table_parity_fromtext_2code( const char * parity_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PARITY ; ++i )
    if( !strcmp( lut_parity[ i ].text, parity_text ) )
      return lut_parity[ i ].code;
  return BPARITY_NONE;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
lookup_table_airrate_2bin( const baudRate_t baud_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_AIRRATE ; ++i )
    if( lut_airrate[ i ].code == baud_code )
      return lut_airrate[ i ].bin;
  return 0xFF;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_airrate_2text( const baudRate_t baud_code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_AIRRATE ; ++i )
    if( lut_airrate[ i ].code == baud_code )
      return lut_airrate[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
baudRate_t
lookup_table_airrate_2code( const uint8_t baud_bin ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_AIRRATE ; ++i )
    if( lut_airrate[ i ].bin == baud_bin )
      return lut_airrate[ i ].code;
  return 0xFF;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
baudRate_t 
lookup_table_airrate_fromtext_2code( const char * baud_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_AIRRATE ; ++i )
    if( !strcmp( lut_airrate[ i ].text, baud_text ) )
      return lut_airrate[ i ].code;
  return B9600;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_packet_2text( const e22900t22s_packet_size_t code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PACKET ; ++i )
    if( lut_packetsize[ i ].code == code )
      return lut_packetsize[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
e22900t22s_packet_size_t 
lookup_table_packet_fromtext( const char * size_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_PACKET ; ++i )
    if( !strcmp( lut_packetsize[ i ].text, size_text ) )
      return lut_packetsize[ i ].code;
  return E22900T22S_PACKET_32;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_power_2text( const e22900t22s_transmission_power_t code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_POWER ; ++i )
    if( lut_power[ i ].code == code )
      return lut_power[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
e22900t22s_transmission_power_t 
lookup_table_power_fromtext( const char * power_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_POWER ; ++i )
    if( !strcmp( lut_power[ i ].text, power_text) )
      return lut_power[ i ].code;
  return E22900T22S_DBM_22;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
lookup_table_worcycle_2text( const e22900t22s_wor_cycle_t code ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_WORCYCLE ; ++i )
    if( lut_worcycle[ i ].code == code )
      return lut_worcycle[ i ].text;
  return "";
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
e22900t22s_wor_cycle_t 
lookup_table_worcycle_fromtext( const char * cycle_text ){
  for( uint8_t i = 0 ; i < E22900T22S_LUT_SIZE_WORCYCLE ; ++i )
    if( !strcmp( lut_worcycle[ i ].text, cycle_text) )
      return lut_worcycle[ i ].code;
  return E22900T22S_WOR_2000;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_update_eeprom( e22900t22s_t * dev ){
  if( !check( dev ) )
    return -1;

  uint8_t cfg[NAME_MAX];

  // Overhead
  cfg[ 0 ] = E22900T22S_SET_REG;           // Command  
  cfg[ 1 ] = E22900T22S_MEM_ADDH;          // Starting address
  cfg[ 2 ] = E22900T22S_MEM_CRYPTL + 1;    // Length
  // Parameters
  cfg[ E22900T22S_MEM_ADDH ] = (uint8_t) (dev->cfg.address >> 7) & 0xFF;
  cfg[ E22900T22S_MEM_ADDL ] = (uint8_t) dev->cfg.address & 0xFF ;
  cfg[ E22900T22S_MEM_NETID ] = dev->cfg.netid ;
  cfg[ E22900T22S_MEM_REG0 ] = (uint8_t) ((uint8_t) lookup_table_baudrate_2bin( dev->cfg.baudrate ) << E22900T22S_SHF_UART   |
                                          (uint8_t) lookup_table_parity_2bin( dev->cfg.parity )     << E22900T22S_SHF_PARITY |
                                          (uint8_t) lookup_table_airrate_2bin( dev->cfg.airrate )   << E22900T22S_SHF_AIRDATA);
  cfg[ E22900T22S_MEM_REG1 ] = (uint8_t) ((uint8_t) dev->cfg.packet_size   << E22900T22S_SHF_PKTSZ  |
                                          (uint8_t) dev->cfg.ambient_noise  << E22900T22S_SHF_AMBNS | 
                                          (uint8_t) dev->cfg.transmit_power << E22900T22S_SHF_POWER );
  cfg[ E22900T22S_MEM_REG2 ] = dev->cfg.channel;
  cfg[ E22900T22S_MEM_REG3 ] = (uint8_t) ((uint8_t) dev->cfg.rssi      << E22900T22S_SHF_RSSI  |
                                          (uint8_t) dev->cfg.fixed     << E22900T22S_SHF_FIXED | 
                                          (uint8_t) dev->cfg.repeater  << E22900T22S_SHF_REPLY |
                                          (uint8_t) dev->cfg.lbt       << E22900T22S_SHF_LBT   |
                                          (uint8_t) dev->cfg.wor       << E22900T22S_SHF_WOR   |
                                          (uint8_t) dev->cfg.wor_cycle << E22900T22S_SHF_WORCYC );
  cfg[ E22900T22S_MEM_CRYPTH ] = (uint8_t) (dev->cfg.encryption >> 7) & 0xFF;
  cfg[ E22900T22S_MEM_CRYPTL ] = (uint8_t) dev->cfg.encryption & 0xFF;

  uint8_t len = e22900t22s_write_register( E22900T22S_MEM_ADDH, E22900T22S_MEM_CRYPTL + 1, cfg, dev );
  if( !len ){
    perror("e22900t22s_write_register");
    return -1;
  }

  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_get_config( e22900t22s_t * dev ){
  uint8_t cfg[NAME_MAX];
  uint8_t len = e22900t22s_read_register( E22900T22S_MEM_ADDH, E22900T22S_MEM_REG3 + 1, cfg, sizeof(cfg), dev );
  //                                Start address__/                     / 
  //                                                      Last address__/
  if( !len ){
    perror("e22900t22s_read_register");
    return -1;
  }

  dev->cfg.address = (uint16_t) (cfg[ E22900T22S_MEM_ADDH ] << 8 | cfg[ E22900T22S_MEM_ADDL ]);
  dev->cfg.netid = cfg[ E22900T22S_MEM_NETID ];
  dev->cfg.baudrate = lookup_table_baudrate_2code( (cfg[ E22900T22S_MEM_REG0 ] >> E22900T22S_SHF_UART) & (E22900T22S_LUT_SIZE_UART - 1) );
  dev->cfg.parity = lookup_table_parity_2code( (cfg[ E22900T22S_MEM_REG0 ] >> E22900T22S_SHF_PARITY) & (E22900T22S_LUT_SIZE_PARITY - 1) );
  dev->cfg.airrate = lookup_table_airrate_2code( (cfg[ E22900T22S_MEM_REG0 ] >> E22900T22S_SHF_AIRDATA) & (E22900T22S_LUT_SIZE_AIRRATE - 1) );
  dev->cfg.packet_size = (cfg[ E22900T22S_MEM_REG1 ] >> E22900T22S_SHF_PKTSZ) & (E22900T22S_LUT_SIZE_PACKET - 1);
  dev->cfg.ambient_noise = (cfg[ E22900T22S_MEM_REG1 ] >> E22900T22S_SHF_AMBNS) & 1; 
  dev->cfg.transmit_power = (cfg[ E22900T22S_MEM_REG1 ] >> E22900T22S_SHF_POWER) & (E22900T22S_LUT_SIZE_POWER - 1); 
  dev->cfg.channel = cfg[ E22900T22S_MEM_REG2 ];
  dev->cfg.rssi = (cfg[ E22900T22S_MEM_REG3 ] >> E22900T22S_SHF_RSSI) & 1;
  dev->cfg.fixed = (cfg[ E22900T22S_MEM_REG3 ] >> E22900T22S_SHF_FIXED) & 1;
  dev->cfg.repeater = (cfg[ E22900T22S_MEM_REG3 ] >> E22900T22S_SHF_REPLY) & 1;
  dev->cfg.lbt = (cfg[ E22900T22S_MEM_REG3 ] >> E22900T22S_SHF_LBT) & 1;
  dev->cfg.wor = (cfg[ E22900T22S_MEM_REG3 ] >> E22900T22S_SHF_WOR) & 1;
  dev->cfg.wor_cycle = (cfg[ E22900T22S_SHF_WORCYC ] >> E22900T22S_SHF_WOR) & (E22900T22S_LUT_SIZE_WORCYCLE - 1);

  len = e22900t22s_read_register( E22900T22S_MEM_PID, E22900T22S_PID_SIZE, cfg, sizeof(cfg), dev );
  if( !len ){
    perror("e22900t22s_read_register");
    return -1;
  }
  if( E22900T22S_PID_SIZE != len ){
    printf("[%d] E22900T22S_PID_SIZE != len ...\n", getpid( ));
    return -1;
  }

  memcpy( dev->cfg.pid, cfg, len );

  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_set_mode( const e22900t22s_mode_t mode , e22900t22s_t * dev ){
  if( !check( dev ) )
    return -1;

  if( -1 == e22900t22s_while_busy( delay_us, dev ) ){
    perror("e22900t22s_while_busy");
    return -1;
  }
  
  int8_t ret = 0;
  uint8_t flag = 0;
  switch( mode ){
    default:
    case E22900T22S_MODE_NORMAL:
      ret = gpiod_digital_write( &dev->gpio.m0, 0 );
      ret = gpiod_digital_write( &dev->gpio.m1, 0 );
      flag = 1;
      break;

    case E22900T22S_MODE_WOR:
      ret = gpiod_digital_write( &dev->gpio.m0, 1 );
      ret = gpiod_digital_write( &dev->gpio.m1, 0 );
      flag = 1;
      break;

    case E22900T22S_MODE_CONFIG:
      ret = gpiod_digital_write( &dev->gpio.m0, 0 );
      ret = gpiod_digital_write( &dev->gpio.m1, 1 );
      break;

    case E22900T22S_MODE_SLEEP:
      ret = gpiod_digital_write( &dev->gpio.m0, 1 );
      ret = gpiod_digital_write( &dev->gpio.m1, 1 );
      break;
  }

  if( -1 == ret ){
    perror("gpiod_digital_write");
    return -1;    
  }

  if( flag ){
    serial_set_baudrate( dev->cfg.baudrate, &dev->serial->sr );
    serial_set_parity( dev->cfg.parity, &dev->serial->sr );
    serial_set_rule( 0, 0, &dev->serial->sr );
  }
  else{
    serial_set_baudrate( B9600, &dev->serial->sr );
    serial_set_parity( BPARITY_NONE, &dev->serial->sr );
    serial_set_rule( 50, 0, &dev->serial->sr );
  }
  
  // Recomendation by the datasheet
  usleep( 2e3 );

  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
gpiod_init( const char * name, gpiod_chip2_t * chip ){
  if( !name || !chip ){
    errno = EINVAL;
    return -1;
  }
  chip->ptr = gpiod_chip_open_by_name( name );
  if( !chip->ptr ){
    printf("[%d] chipname:%s ", getpid( ), name );
    perror("gpiod_chip_open_by_name");
    return -1;
  }
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
gpiod_pin_mode( gpiod_chip2_t * chip, gpiod_line2_t * gpio, uint8_t direction ){
  if( !chip || !gpio ){
    errno = EINVAL;
    return -1;
  }
  gpio->ptr = gpiod_chip_get_line( chip->ptr, gpio->offset );
  if( !gpio->ptr ){
    gpiod_chip_close( chip->ptr );
    return -1;
  }

  int ret = -1;
  if( 0 < direction ) // Output
    ret = gpiod_line_request_output( gpio->ptr, "lora_driver", 0 );
    //                    line pointer _/   consumer_/          \_initial value
  else                // Input
    ret = gpiod_line_request_input_flags( gpio->ptr, "lora_driver", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP );

  if( -1 == ret ){
    printf("[%d] gpiod_line_request_input_flags: %d ...\n", getpid( ), gpio->offset );
    gpiod_chip_close( chip->ptr );
    return -1;
  }

  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
gpiod_digital_write( gpiod_line2_t * gpio, uint8_t value ){
  if( !gpio ){
    errno = EINVAL;
    return -1;
  }
  if( !gpio->ptr ){
    errno = EBADF;
    return -1;
  }
  return (int8_t) gpiod_line_set_value( gpio->ptr, !value ? 0 : 1 );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
gpiod_digital_read( gpiod_line2_t * gpio ){
  if( !gpio ){
    errno = EINVAL;
    return -1;
  }
  if( !gpio->ptr ){
    errno = EBADF;
    return -1;
  }
  return (int8_t) gpiod_line_get_value( gpio->ptr );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_gpio_init( const char * chip_name, uint8_t m0, uint8_t m1, uint8_t aux, e22900t22s_t * dev ){
  if( !chip_name || !check( dev ) ){
    errno = EINVAL;
    return -1;
  }
  if( -1 == gpiod_init( chip_name, &dev->gpio.chip ) ){
    perror("gpiod_init");
    return -1;
  }
  dev->gpio.m0.offset = m0;
  if( -1 == gpiod_pin_mode( &dev->gpio.chip, &dev->gpio.m0, 1 ) ){
    perror("gpiod_pin_mode(m0)");
    return -1;
  }
  dev->gpio.m1.offset = m1;
  if( -1 == gpiod_pin_mode( &dev->gpio.chip, &dev->gpio.m1, 1 ) ){
    perror("gpiod_pin_mode(m1)");
    return -1;
  }
  dev->gpio.aux.offset = aux;
  if( -1 == gpiod_pin_mode( &dev->gpio.chip, &dev->gpio.aux, 0 ) ){
    perror("gpiod_pin_mode(aux)");
    return -1;
  }
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_gpio_close( e22900t22s_t * dev ){
  if( !check( dev ) )
    return -1;
  
  e22900t22s_pinmode_t * gpio = &dev->gpio;
  gpiod_line_release( gpio->m0.ptr );
  gpiod_line_release( gpio->m1.ptr );
  gpiod_line_release( gpio->aux.ptr );
  gpiod_chip_close( gpio->chip.ptr );
  gpio->m0.ptr = NULL;
  gpio->m1.ptr = NULL;
  gpio->aux.ptr = NULL;
  gpio->chip.ptr = NULL;
  return 0;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char * 
e22900t22s_print_config( uint8_t console, e22900t22s_t * dev ){
  if( !check( dev ) )
    return NULL;

  char * message = (char *) malloc( 1024 );
  e22900t22s_eeprom_t * cfg = &dev->cfg;
  e22900t22s_pinmode_t * pin = &dev->gpio; 

  snprintf( message, 1024, 
    "[%d] Product information: %02X-%02X-%02X-%02X-%02X-%02X-%02X\n"
    "[%d] Module address: %hd\n"
    "[%d] Network identification: %hhd\n"
    "[%d] Baud rate (bps): %s\n"
    "[%d] Air data rate (bps): %s\n"
    "[%d] Parity bit: %s\n"
    "[%d] Packet size (B): %s\n"
    "[%d] Transmission power (dBm): %s\n"
    "[%d] Channel: %d\n"
    "[%d] Ambient noise: %s\n"
    "[%d] RSSI byte: %s\n"
    "[%d] Fixed point: %s\n"
    "[%d] Repeater: %s\n"
    "[%d] Listen before talk: %s\n"
    "[%d] WOR: %s\n"
    "[%d] WOR cycle (ms): %s\n"
    "[%d] Connected to %s: "
    "M0: %d, "
    "M1: %d, "
    "AUX: %d\n",
    getpid( ), cfg->pid[0], cfg->pid[1], cfg->pid[2], cfg->pid[3], cfg->pid[4], cfg->pid[5], cfg->pid[6],
    getpid( ), cfg->address, 
    getpid( ), cfg->netid,
    getpid( ), lookup_table_baudrate_2text( cfg->baudrate ),
    getpid( ), lookup_table_airrate_2text( cfg->airrate ),
    getpid( ), lookup_table_parity_2text( cfg->parity ),
    getpid( ), lookup_table_packet_2text( cfg->packet_size ),
    getpid( ), lookup_table_power_2text( cfg->transmit_power ),
    getpid( ), cfg->channel,
    getpid( ), cfg->ambient_noise ? "enabled" : "disabled",
    getpid( ), cfg->rssi ? "enabled" : "disabled",
    getpid( ), cfg->fixed ? "enabled" : "disabled",
    getpid( ), cfg->repeater ? "enabled" : "disabled",
    getpid( ), cfg->lbt ? "enabled" : "disabled",
    getpid( ), cfg->wor ? "enabled" : "disabled",
    getpid( ), lookup_table_worcycle_2text( cfg->wor_cycle ),
    getpid( ), pin->chip.name, pin->m0.offset, pin->m1.offset, pin->aux.offset
  );
  
  if( console )
    printf("%s", message );
  return message;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
e22900t22s_read_register( const uint8_t address, const uint8_t length, uint8_t * data, const uint8_t size, e22900t22s_t * dev ){
  if( !data ){
    errno = EINVAL;
    return 0;
  }
  
  for( uint16_t i = 0, tmp = (uint16_t) address ; i < 2 ; ++i ){
    if( ((0x08 < tmp) && (0x80 > tmp)) || (0x86 < tmp) ){
      errno = EADDRNOTAVAIL;
      return 0;
    }
    tmp += length - 1;
  }

  if( length > size ){
    errno = EINVAL;
    return 0;
  }

  if( -1 == e22900t22s_set_mode( E22900T22S_MODE_CONFIG, dev ) ){
    perror("e22900t22s_set_mode");
    return 0;
  }
  
  if( -1 == e22900t22s_while_busy( delay_us, dev ) ){
    perror("e22900t22s_while_busy");
    return 0;
  }
  
  uint8_t buf[NAME_MAX];

  // Overhead - Read Configuration memory block
  buf[ 0 ] = E22900T22S_READ_REG;          // Command  
  buf[ 1 ] = address;                      // Starting address
  buf[ 2 ] = length;                       // Length
  if( !serial_write( &dev->serial->sr, buf, 3 ) ){
    perror("serial_write");
    return 0;
  }
  serial_flush( &dev->serial->sr );
  
  const uint8_t overhead = 3;
  uint8_t buflen = overhead + length;

  uint8_t len = (uint8_t) serial_read( (char *) buf, sizeof(buf), 0, buflen , &dev->serial->sr );
  if( len != buflen ){
    perror("serial_read - not same size");
    return 0;
  }
   
  memcpy( data, &buf[overhead], length );

  if( -1 == e22900t22s_set_mode( E22900T22S_MODE_NORMAL, dev ) ){
    perror("e22900t22s_set_mode");
    return 0;
  }

  return length;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
e22900t22s_write_register( const uint8_t address, const uint8_t length, const uint8_t * data, e22900t22s_t * dev ){
  if( !data ){
    errno = EINVAL;
    return 0;
  }

  for( uint16_t i = 0, tmp = (uint16_t) address ; i < 2 ; ++i ){
    if( ((0x08 < tmp) && (0x80 > tmp)) || (0x86 < tmp) ){
      errno = EADDRNOTAVAIL;
      return 0;
    }
    tmp += length - 1;
  }


  if( -1 == e22900t22s_set_mode( E22900T22S_MODE_CONFIG, dev ) ){
    perror("e22900t22s_set_mode");
    return 0;
  }
  
  if( -1 == e22900t22s_while_busy( delay_us, dev ) ){
    perror("e22900t22s_while_busy");
    return 0;
  }
  
  uint8_t buf[NAME_MAX], ret[NAME_MAX];
  
  // Overhead
  buf[ 0 ] = E22900T22S_SET_REG;    // Command  
  buf[ 1 ] = address;               // Starting address
  buf[ 2 ] = length;                // Length
  memcpy( &buf[3], data, length );

  uint8_t buflen = 3 + length;
  //      Overhead_/      \_ Length

  if( !serial_write( &dev->serial->sr, buf, buflen ) ){
    perror("serial_write");
    return 0;
  }
  serial_flush( &dev->serial->sr );

  uint8_t len = (uint8_t) serial_read( (char *) ret, sizeof(ret), 0, buflen, &dev->serial->sr );
  if( len != buflen ){
    perror("serial_read - not same size");
    return 0;
  }

  //                      __ Since first byte changes from C0 to C1
  //                     /              _____ Since we are starting one index forward
  if( 0 != memcmp( &ret[1], &buf[1], len - 1 ) ){
    perror("serial_read - not match words");
    return 0;
  }


  if( -1 == e22900t22s_set_mode( E22900T22S_MODE_NORMAL, dev ) ){
    perror("e22900t22s_set_mode");
    return 0;
  }

  return length;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
uint8_t 
e22900t22s_read_rssi_register( const uint8_t address, const uint8_t length, uint8_t * data, const uint8_t size, e22900t22s_t * dev ){
  if( !data ){
    errno = EINVAL;
    return 0;
  }
  
  for( uint16_t i = 0, tmp = (uint16_t) address ; i < 2 ; ++i ){
    if( 0x01 < tmp ){
      errno = EADDRNOTAVAIL;
      return 0;
    }
    tmp += length - 1;
  }

  if( length > size ){
    errno = EINVAL;
    return 0;
  }  

  if( -1 == e22900t22s_while_busy( delay_us, dev ) ){
    perror("e22900t22s_while_busy");
    return 0;
  }

  serial_set_rule( 50, 0, &dev->serial->sr );

  uint8_t buf[NAME_MAX];

  // Overhead - Read Configuration memory block
  for( uint8_t i = 0 ; i < 4 ; ++i )
    buf[ i ] = 0xC0 + i;                   // Command: 0xC0 0xC1 0xC2 0xC3  
  buf[ 4 ] = address;                      // Starting address
  buf[ 5 ] = length;                       // Length

  const uint8_t cmd_len = 6;
  if( !serial_write( &dev->serial->sr, buf, cmd_len ) ){
    perror("serial_write");
    return 0;
  }
  serial_flush( &dev->serial->sr );
  
  // Response: 0xC1 + address + length + value(length)
  const uint8_t overhead = 3;
  uint8_t buflen = overhead + length;

  uint8_t len = (uint8_t) serial_read( (char *) buf, sizeof(buf), 0, buflen , &dev->serial->sr );
  if( len != buflen ){
    perror("serial_read - not same size");
    return 0;
  }

  serial_set_rule( 0, 0, &dev->serial->sr );
   
  memcpy( data, &buf[overhead], length );

  return length;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t  
e22900t22s_while_busy( const uint32_t delay, e22900t22s_t * dev ){
  if( !check( dev ) )
    return -1;

  int8_t aux;
  while( !( aux = gpiod_digital_read( &dev->gpio.aux ) ) ){
    if( -1 == aux ){
      perror("gpiod_digital_read");
      return -1;    
    }
    usleep( delay );
  }
  
  return 0;
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_connect_mixip( const char * name, e22900t22s_mixip_t * config ){
  if( !name || !config ){
    errno = EINVAL;
    return -1;
  }
  return mixip_translator_connect( name, &config->ptr );
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_load_mixip_config( const char * filename, e22900t22s_mixip_t * config ){
  if( !config || !filename ){
    errno = EINVAL;
    return -1;
  }

  xmlDoc * docfile = xmlReadFile( filename, NULL, 0 );
  if( !docfile ){
    errno = EINVAL;
    perror("xmlReadFile");
    return -1;
  }
  
  xmlNode * root_element = xmlDocGetRootElement(docfile);
  if( !root_element ){
    errno = EINVAL;
    perror("xmlDocGetRootElement");
    xmlFreeDoc( docfile );
    return -1;
  }

  memset( config, 0, sizeof(e22900t22s_mixip_t) );

  xmlNode * translator = NULL;

  if( !strcmp( (char *) root_element->name, "e22900t22s" ) ){
    for( xmlNode * current_node = root_element->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "translator" ) )
          translator = current_node;        
      }        
    }
  }  

  if( NULL != translator ){
    xmlNode * slots = NULL;
    xmlNode * srsize = NULL;

    for( xmlNode * current_node = translator->children ; current_node != NULL ; current_node = current_node->next ){
      if( XML_ELEMENT_NODE == current_node->type ){
        if( !strcmp( (char *) current_node->name, "slots" ) )
          slots = current_node;        
        if( !strcmp( (char *) current_node->name, "srsize" ) )
          srsize = current_node;        
      }
    }

    if( NULL != slots )
      config->tmp.size_rb = (uint8_t) atoi( (const char *) xmlNodeGetContent( slots ) );
    else
      config->tmp.size_rb = E22900T22S_DEF_RB; // Default value

    if( NULL != srsize )
      config->tmp.size_sls = (uint8_t) atoi( (const char *) xmlNodeGetContent( srsize ) );
    else
      config->tmp.size_sls = E22900T22S_DEF_SLS; // Default value
  }

  xmlFreeDoc( docfile );
  xmlCleanupParser( );
  return 0;  
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_update_mixip_config( const e22900t22s_mixip_t * config ){
  if( !config ){
    errno = EINVAL;
    return -1;
  }  

  if( -1 == mixip_translator_ring_buffer_size( config->tmp.size_rb, config->ptr ) ){
    perror("mixip_translator_ring_buffer_size");
    return -1;
  }

  if( -1 == mixip_translator_serial_link_segment_size( config->tmp.size_sls, config->ptr ) ){
    perror("mixip_translator_serial_link_segment_size");
    return -1;
  }

  if( -1 == mixip_translator_activate( config->ptr ) ){
    perror("mixip_translator_activate");
    return -1;
  }

  return 0;  
}

/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int8_t 
e22900t22s_get_rssi( e22900t22s_rssi_t * rssi, e22900t22s_t * dev ){
  if( !rssi ){
    errno = EINVAL;
    return -1;
  }

  uint8_t buf[2];
  if( !e22900t22s_read_rssi_register( E22900T22S_CURR_RSSI, E22900T22S_PAST_RSSI + 1, buf, sizeof(buf), dev ) ){
    perror("e22900t22s_read_rssi_register");
    return -1;
  }
  
  rssi->current = (float) buf[0] * (float) (-0.5);
  rssi->past = (float) buf[1] * (float) (-0.5);

  return 0;
}


/***************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * End file
 **************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
