//Screen Variables
#define Screen_LoadingScreen 0x00
#define Screen_Main 0x01



uint8_t NumberOfBytesToRead = 0;
char ch_GeoSerialDATA[100] = "";
String str_GeoFenceMidPoint_Latitude = "";
String str_GeoFenceMidPoint_Longitude = "";
String str_GeoFenceRadius = "";

float GeoFenceMidPoint_Latitude = 0.00;
float GeoFenceMidPoint_Longitude = 0.00;
uint16_t GeoFenceRadius = 0;

bool SlaveStatus[7] = { false, false, false, false, false, false, false };

bool SlaveMeddle[7] = { false, false, false, false, false, false, false };
bool AlarmType[7] = { false, false, false, false, false, false, false };

bool MeddledOnly[7] = { false, false, false, false, false, false, false };

uint32_t last_millis = 0;