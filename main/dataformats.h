/*

Packet Processor

*/

#pragma once


#ifdef USE_CAYENNE
    // CAYENNE DF
    static uint8_t txBuffer[11] = {0x03, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    void buildPacket(uint8_t txBuffer[11])
    {
        LatitudeBinary = ((_gps.location.lat() + 90) / 180.0) * 16777215;
        LongitudeBinary = ((_gps.location.lng() + 180) / 360.0) * 16777215;
        int32_t Height = gps.altitude.meters() * 100;

        sprintf(t, "Lat: %f", _gps.location.lat());
        Serial.println(t);
        sprintf(t, "Lng: %f", _gps.location.lng());
        Serial.println(t);        
        sprintf(t, "Alt: %f", _gps.altitude.meters());
        Serial.println(t);        
        
        txBuffer[2] = ( LatitudeBinary >> 16 ) & 0xFF;
        txBuffer[3] = ( LatitudeBinary >> 8 ) & 0xFF;
        txBuffer[4] = LatitudeBinary & 0xFF;

        txBuffer[5] = ( LongitudeBinary >> 16 ) & 0xFF;
        txBuffer[6] = ( LongitudeBinary >> 8 ) & 0xFF;
        txBuffer[7] = LongitudeBinary & 0xFF;

        txBuffer[8] = Height >> 16;
        txBuffer[9] = Height >> 8;
        txBuffer[10] = Height;
    }    
#else
uint8_t txBuffer[9];

void buildPacket(uint8_t txBuffer[9])
{
  LatitudeBinary = ((_gps.location.lat() + 90) / 180.0) * 16777215;
  LongitudeBinary = ((_gps.location.lng() + 180) / 360.0) * 16777215;
  
  sprintf(t, "Lat: %f", _gps.location.lat());
  Serial.println(t);
  
  sprintf(t, "Lng: %f", _gps.location.lng());
  Serial.println(t);
  
  txBuffer[0] = ( LatitudeBinary >> 16 ) & 0xFF;
  txBuffer[1] = ( LatitudeBinary >> 8 ) & 0xFF;
  txBuffer[2] = LatitudeBinary & 0xFF;

  txBuffer[3] = ( LongitudeBinary >> 16 ) & 0xFF;
  txBuffer[4] = ( LongitudeBinary >> 8 ) & 0xFF;
  txBuffer[5] = LongitudeBinary & 0xFF;

  altitudeGps = _gps.altitude.meters();
  txBuffer[6] = ( altitudeGps >> 8 ) & 0xFF;
  txBuffer[7] = altitudeGps & 0xFF;

  hdopGps = _gps.hdop.value()/10;
  txBuffer[8] = hdopGps & 0xFF;
}

#endif
