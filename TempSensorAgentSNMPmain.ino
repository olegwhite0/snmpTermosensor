#include <DallasTemperature.h>
#include <Streaming.h>         // Include the Streaming library
#include <UIPEthernet.h>       // Include the Ethernet library
#include <SPI.h>
#include <UIPAgentuino.h> 
#include <OneWire.h>


#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 10, 222, 128, 251 };


static char sysDescr[]       = "1.3.6.1.2.1.1.1.0";  // read-only  (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysObjectID (.1.3.6.1.2.1.1.2)
//static char sysObjectID[] PROGMEM   = "1.3.6.1.2.1.1.2.0";  // read-only  (ObjectIdentifier)
// .iso.org.dod.internet.mgmt.mib-2.system.sysUpTime (.1.3.6.1.2.1.1.3)
static char sysUpTime[]      = "1.3.6.1.2.1.1.3.0";  // read-only  (TimeTicks)
// .iso.org.dod.internet.mgmt.mib-2.system.sysContact (.1.3.6.1.2.1.1.4)
static char sysContact[]     = "1.3.6.1.2.1.1.4.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysName (.1.3.6.1.2.1.1.5)
static char sysName[]        = "1.3.6.1.2.1.1.5.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysLocation (.1.3.6.1.2.1.1.6)
static char sysLocation[]    = "1.3.6.1.2.1.1.6.0";  // read-write (DisplayString)
// .iso.org.dod.internet.mgmt.mib-2.system.sysServices (.1.3.6.1.2.1.1.7)
static char sysServices[]    = "1.3.6.1.4.1.49701.1.1.0";  // read-only  (Integer)
//
// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
//
//
// RFC1213 local values
static char locDescr[]              = "White-DS2SNMP";  // read-only (static)
//static char locObjectID[]         = "1.3.6.1.3.2009.0";                       // read-only (static)
static uint32_t locUpTime           = 0;                                        // read-only (static)
static char locContact[]          = "OID Themperature is 1.3.6.1.4.1.49701.1.1.0";                         // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locName[20]             = "White-DS2SNMP";                              // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locLocation[20]         = "RUSSIA";                         // should be stored/read from EEPROM - read/write (not done for simplicity)
static char locServices []         =  "-99.99";                                        // read-only (static)

uint32_t prevMillis = millis();
uint32_t PrevMillisTempTimer = millis();
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;
DeviceAddress tempDeviceAddress;
int resolution = 12;


void RequestDallas18b20()
{
unsigned long timing=0;
      while (!sensors.isConversionComplete()) {
     // Serial.print("*");// ����� ��������� �������� ��������
        if (millis() - timing > 10000){ // ������ 10000 ���������� ������ ��� �������� ����� 
          timing = millis(); 
         // Serial.println (timing);
        }
    }
  float celsius=sensors.getTempCByIndex(0);
  dtostrf(celsius,6, 2, locServices);
   // makes it async
  sensors.requestTemperatures();

 

}

void pduReceived()
{
    
 //   Serial.print("*PDU*");
    
//         end recieve
  SNMP_PDU pdu;
  api_status = Agentuino.requestPdu(&pdu);
  //Serial.println(oid);
  //

  if ((pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT || pdu.type == SNMP_PDU_SET)
    && pdu.error == SNMP_ERR_NO_ERROR && api_status == SNMP_API_STAT_SUCCESS ) {
    //
    pdu.OID.toString(oid);

    // Implementation SNMP GET NEXT
    if ( pdu.type == SNMP_PDU_GET_NEXT ) {
      char tmpOIDfs[SNMP_MAX_OID_LEN];
      if ( strcmp( oid, sysDescr ) == 0 ) {  
        strcpy ( oid, sysUpTime ); 
        strcpy ( tmpOIDfs, sysUpTime );        
        pdu.OID.fromString(tmpOIDfs);  
      } else if ( strcmp(oid, sysUpTime ) == 0 ) {  
        strcpy ( oid, sysContact );  
        strcpy ( tmpOIDfs, sysContact );        
        pdu.OID.fromString(tmpOIDfs);          
      } else if ( strcmp(oid, sysContact ) == 0 ) {  
        strcpy ( oid, sysName );  
        strcpy ( tmpOIDfs, sysName );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp(oid, sysName ) == 0 ) {  
        strcpy ( oid, sysLocation );  
        strcpy ( tmpOIDfs, sysLocation );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp(oid, sysLocation ) == 0 ) {  
        strcpy ( oid, sysServices );  
        strcpy ( tmpOIDfs, sysServices );        
        pdu.OID.fromString(tmpOIDfs);                  
      } else if ( strcmp(oid, sysServices ) == 0 ) {  
        strcpy ( oid, "1.0" );  
      } else {
          int ilen = strlen(oid);
          if ( strncmp(oid, sysDescr, ilen ) == 0 ) {
            strcpy ( oid, sysDescr ); 
            strcpy ( tmpOIDfs, sysDescr );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp(oid, sysUpTime, ilen ) == 0 ) {
            strcpy ( oid, sysUpTime ); 
            strcpy ( tmpOIDfs, sysUpTime );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp(oid, sysContact, ilen ) == 0 ) {
            strcpy ( oid, sysContact ); 
            strcpy ( tmpOIDfs, sysContact );        
            pdu.OID.fromString(tmpOIDfs); 
          } else if ( strncmp(oid, sysName, ilen ) == 0 ) {
            strcpy ( oid, sysName ); 
            strcpy ( tmpOIDfs, sysName );        
            pdu.OID.fromString(tmpOIDfs);   
          } else if ( strncmp(oid, sysLocation, ilen ) == 0 ) {
            strcpy ( oid, sysLocation ); 
            strcpy ( tmpOIDfs, sysLocation );        
            pdu.OID.fromString(tmpOIDfs);    
            } else if ( strncmp(oid, sysServices, ilen ) == 0 ) {
            strcpy ( oid, sysServices ); 
            strcpy ( tmpOIDfs, sysServices );        
            pdu.OID.fromString(tmpOIDfs);  
            }            
      } 
    }
    // End of implementation SNMP GET NEXT / WALK
    //Serial.println("mainOidFunc");
    //Serial.println( strcmp( oid, sysDescr ));
    if ( strcmp(oid, sysDescr)== 0  ) {
      // handle sysDescr (set/get) requests
     
        // response packet from get-request - locDescr
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locDescr);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else if ( strcmp(oid, sysUpTime ) == 0 ) {
      // handle sysName (set/get) requests
      
        // response packet from get-request - locUpTime
        status = pdu.VALUE.encode(SNMP_SYNTAX_TIME_TICKS, locUpTime);       
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else if ( strcmp(oid, sysName ) == 0 ) {
      // handle sysName (set/get) requests
      
        // response packet from get-request - locName
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locName);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else if ( strcmp(oid, sysContact ) == 0 ) {
      // handle sysContact (set/get) requests
      
        // response packet from get-request - locContact
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locContact);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else if ( strcmp(oid, sysLocation ) == 0 ) {
      // handle sysLocation (set/get) requests
     
        // response packet from get-request - locLocation
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locLocation);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else if ( strcmp(oid, sysServices) == 0 ) {
      // handle sysServices (set/get) requests
     
        // response packet from get-request - locServices
         
        status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, locServices);
        pdu.type = SNMP_PDU_RESPONSE;
        pdu.error = status;
      
      //
    } else {
      // oid does not exist
      // response packet - object not found
      pdu.type = SNMP_PDU_RESPONSE;
      //status = pdu.VALUE.encode(SNMP_SYNTAX_OCTETS, "1.3.6.1.2.1.1.7.0");
      pdu.error = SNMP_ERR_NO_SUCH_NAME;
      //pdu.error = status;
    }
    //
    Agentuino.responsePdu(&pdu);
  }
  //
  Agentuino.freePdu(&pdu);
}

void setup()
{
  Serial.begin(115200);
  
  //ds.search(addr);
  sensors.begin();
  sensors.requestTemperatures();
  sensors.setWaitForConversion(false);
  RequestDallas18b20();
  //
  Ethernet.begin(mac, ip);
  api_status = Agentuino.begin();
  //
  if ( api_status == SNMP_API_STAT_SUCCESS ) {
    //
    Agentuino.onPduReceive(pduReceived);
    
    delay(10);
    //
    return;
  }
  //
  delay(10);
}

void loop()
{
  // listen/handle for incoming SNMP requests
  Agentuino.listen();
  
  //
  // sysUpTime - The time (in hundredths of a second) since
  // the network management portion of the system was last
  // re-initialized.
  if (millis() - prevMillis > 60000){ // ������ 10000 ���������� ������ ��� �������� ����� 
          prevMillis = millis(); 
          RequestDallas18b20();
    // increment up-time counter
          locUpTime += 6000;  
          if (locUpTime>1299250100){locUpTime=0;}
        }
  
}