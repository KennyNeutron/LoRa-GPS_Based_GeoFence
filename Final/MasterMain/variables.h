//Store All Variables Here
uint32_t ihap = 0;
uint8_t GeoSerialDATA = 0xFF;

uint16_t NumberOfBytesToRead = 0;

char ch_GeoSerialDATA[100] = "";


float GeoFenceMidPoint_Latitude = 0.00;
float GeoFenceMidPoint_Longitude = 0.00;
uint16_t GeoFenceRadius = 0;

String str_GeoFenceMidPoint_Latitude = "";
String str_GeoFenceMidPoint_Longitude = "";
String str_GeoFenceRadius = "";
String str_DeviceID_Recieved = "";
String str_DevicePW_Recieved = "";


char SlaveID_fromMaster[6] = "xxxxxx";
char SlavePW_fromMaster[5] = "0000";
bool PairCommand_fromApp = false;
