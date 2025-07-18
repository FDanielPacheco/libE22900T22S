#include <mixip.h> 
#include <time.h>
#include <unistd.h>

#include <e22900t22s/core.h>
#include <e22900t22s/metrics.h>
#include <e22900t22s/mixip.h>

e22900t22s_t driver;

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char *
gettime( void ){
  time_t t;
  struct tm * info;
  time( &t );
  info = localtime( &t );

  char * tm = malloc( 12 ); 
  if( !tm ){
    perror("malloc");
    return NULL;
  }

  strftime( tm, sizeof(tm), "%H:%M:%S", info );
  return tm;
}

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
void
printPacket( const uint8_t * pk, const size_t len, const char * cover, int columnsize ){
  if( !pk ) return;

  int cont = columnsize;
  printf("<-----%s Packet [%s]----->\n", cover, gettime( ) );
  for( size_t i = 0 ; i < len ; ++i ){
    if( !cont || (i + 1 == len) ){
      printf( "%04zu:%02X\n", i, pk[i] );
      cont = columnsize;
    }
    else {
      printf( "%04zu:%02X ", i, pk[i] );
      cont--;
    }
  }
  printf("<-----%s Packet----->\n\n", cover);
}

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int dsetup( serial_manager_t * serial, const char * name ){
  driver.serial = serial;
  e22900t22s_eeprom_t eeprom;
  e22900t22s_pinmode_t pinout;
  e22900t22s_mixip_t translator;

  printf("[%d] Setup on %s ...\n", getpid( ), getenv(name) );

  int8_t ret = e22900t22s_load_config( getenv(name), &eeprom, &pinout );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Load XML configuration [e22900t22s_load_config]");
    return -1;
  }

  ret = e22900t22s_load_mixip_config( getenv(name), &translator );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Load XML configuration [e22900t22s_load_mixip_config]");
    return -1;
  }

  ret = e22900t22s_connect_mixip( name, &translator );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Connect driver to the translator");
    printf("Path given: %s_tx_param\n", name);
    return -1;
  }

  ret = e22900t22s_update_mixip_config( &translator );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Update the translator from the driver");
    return -1;
  }

  ret = e22900t22s_set_pinout( &pinout, &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Set the pinout");
    e22900t22s_gpio_close( &driver );
    return -1;
  }
    
  ret = e22900t22s_set_config( &eeprom, 0, &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Set the EEPROM configuration");
    e22900t22s_gpio_close( &driver );
    return -1;
  }

  ret = e22900t22s_update_eeprom( &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Updating the EEPROM");
    e22900t22s_gpio_close( &driver );
    return -1;
  }

  ret = e22900t22s_get_config( &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Retriving the EEPROM");
    e22900t22s_gpio_close( &driver );
    return -1;
  }

  e22900t22s_print_config( 1, &driver );
  printf("[%d] Device configured...\n", getpid( ) );

  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int dloop( flow_t * flow ){
  // Runs in loop, in a separeted process, consider limiting the CPU poll with a sleep...
  // To stop the other process (read/write) use halt_network( flow ), and continue_network( flow )
  
  sleep( 1 );
  if( driver.cfg.ambient_noise ){
    e22900t22s_rssi_t rssi;
    if( -1 == e22900t22s_get_rssi( &rssi, &driver ) ){
      perror("e22900t22s_get_rssi");
      return -1;
    }
    printf("[%d][%s] Past: %3.2f [dBm], Current: %3.2f [dBm]\n", getpid( ), gettime( ), rssi.past, rssi.current );
  }

  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int dread( buffer_t * buf ){
  
  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int dwrite( buffer_t * buf ){
  if( -1 == e22900t22s_while_busy( 100, &driver ) ){
    perror("e22900t22s_while_busy");
    return -1;
  }
  return 0; 
}

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int dexit( void ){
  if( -1 == e22900t22s_gpio_close( &driver ) ){
    perror("e22900t22s_gpio_close");
    return -1;
  }
  return 0;
}
