#include <mixip.h> 
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#include <e22900t22s/core.h>
#include <e22900t22s/metrics.h>
#include <e22900t22s/mixip.h>

e22900t22s_t      driver;
e22900t22s_log_t  * logs; 

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
const char *
gettime( void ){
  time_t t;
  struct tm * info;
  time( &t );
  info = localtime( &t );
  
  const uint8_t len = 12;
  char * tm = malloc( len ); 
  if( !tm ){
    perror("malloc");
    return NULL;
  }

  strftime( tm, len, "%H:%M:%S", info );
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
int 
dsetup( serial_manager_t * serial, const char * name ){
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
    printf("[%d] ", getpid( ));
    printf("Path given: %s_tx_param\n", name);
    return -1;
  }

  enum{
    IS_TRANSMITTER = 0,
    IS_RECEIVER = 1,  
  };

  switch( ret ){
    case IS_TRANSMITTER:
      // This driver is a transmitter
      ret = e22900t22s_update_mixip_config( &translator );
      if( -1 == ret ){
        printf("[%d] ", getpid( ));
        perror("Update the translator from the driver");
        return -1;
      }
      break;

    default:
    case IS_RECEIVER:
      // This driver is a receiver
      break;      
  }

  ret = e22900t22s_set_pinout( &pinout, &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Set the pinout");
    return -1;
  }
    
  ret = e22900t22s_set_config( &eeprom, 0, &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Set the EEPROM configuration");
    return -1;
  }

  ret = e22900t22s_update_eeprom( &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Updating the EEPROM");
    return -1;
  }

  ret = e22900t22s_get_config( &driver );
  if( -1 == ret ){
    printf("[%d] ", getpid( ));
    perror("Retriving the EEPROM");
    return -1;
  }

  e22900t22s_print_config( 1, &driver );
  printf("[%d] Device configured...\n", getpid( ) );

  logs = (e22900t22s_log_t *) mmap( NULL, sizeof( e22900t22s_log_t ), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
  if( MAP_FAILED == logs ){
    printf("[%d] ", getpid( ));
    perror("Initializing logs");
    return -1;  
  }
  memset( logs, 0, sizeof( e22900t22s_log_t ) );

  if( 0 == (logs->No = e22900t22s_get_noise_rssi( &driver, 1 ) ) ){
    if( ECANCELED == errno ){
      printf("[%d] ", getpid( ));
      perror("e22900t22s_get_noise_rssi");
      return -1;        
    }
  }
  printf("[%d][%s] Noise floor: %3.2f [dBm]\n", getpid( ), gettime( ), logs->No );    

  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int 
dloop( flow_t * flow ){
  // Runs in loop, in a separeted process, consider limiting the CPU poll with a sleep...
  // To stop the other process (read/write) use halt_network( flow ), and continue_network( flow )
  
  sleep( 10 );
 
  /*
  mixip_halt( flow );  
  printf("[%d][%s] Past: %3.2f [dBm], Current: %3.2f [dBm]\n", getpid( ), gettime( ), rssi.past, rssi.current );    
  mixip_continue( flow );
  */
  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int 
dread( buffer_t * buf ){
  // Clear the previous samples
  memset( logs->sample, 0, sizeof( e22900t22s_rx_metric_t ) * NSEG_MAX );
  logs->n_samples = 0;

  // If first is set it means the previous buffer had the last byte being EOF, so now the first byte of buf->data is 100% the RSSI  
  if( logs->tmp.first )
    logs->sample[ logs->n_samples++ ].Pr = e22900t22s_get_signal_rssi( buf->data[0] );
  
  if( -1 == e22900t22s_identify_segments( buf->data, buf->len, &logs->tmp ) ){
    printf("[%d] ", getpid( ));
    perror("e22900t22s_identify_segments");
    return -1;      
  }

  logs->n_received += logs->tmp.length;
  uint8_t n = logs->tmp.length;
  if( logs->tmp.first )
    n --;

  for( uint8_t i = 0 ; i < n ; ++i ){
    if( logs->tmp.segment->end + 1 < buf->len )
      logs->sample[ logs->n_samples ++ ].Pr = e22900t22s_get_signal_rssi( buf->data[ logs->tmp.segment->end + 1 ] );
    else
      printf("[%d] sample indicates the RSSI outside the buffer's available space\n", getpid( ));
  }

  for( uint8_t i = 0 ; i < logs->n_samples ; ++i )
    logs->sample[ i ].SNR = logs->sample[ i ].Pr / logs->No;

  for( uint8_t i = 0 ; i < logs->n_samples ; ++i )
    printf("[%d][%s][sample: %d] Pr: %3.2f [dBm], No: %3.2f [dBm], SNR: %3.2f\n", getpid( ), gettime( ), i, logs->sample[ i ].Pr, logs->No ,logs->sample[ i ].SNR  );          
  printf("[%d][%s] Received: %d (#)\n", getpid( ), gettime( ), logs->n_received );        

  return 0; 
}
 
/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int 
dwrite( buffer_t * buf ){
  logs->n_sent++;
  printf("[%d][%s] Sent: %d (#)\n", getpid( ), gettime( ), logs->n_sent );        

  if( -1 == e22900t22s_while_busy( 100, &driver ) ){
    perror("e22900t22s_while_busy");
    return -1;
  }
  return 0; 
}

/**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
int 
dexit( void ){
  if( -1 == e22900t22s_gpio_close( &driver ) ){
    perror("e22900t22s_gpio_close");
    return -1;
  }
  return 0;
}
