#define pie 3.14159265358979323846

bool valid_location = false;
String gps_lat = " ";
String gps_lon = " ";

uint32_t iter = 0;
int counter = 0;

float GPS_Lat = 0.0;
float GPS_Lon = 0.0;

uint32_t last_millis = 0;
bool BuzzerToggle = false;
uint16_t BuzzerInterval = 100;

uint32_t LoRa_last_millis = 0;
uint32_t LoRa_SendInterval = 10000;

#define BuzzerPin 5
bool DevicePaired = false;
uint8_t ThisDevice_SlaveAssignment = 0;
bool ReplyTerminate = false;

float GeoFenceMid_Lat = 0.0;
float GeoFenceMid_Lon = 0.0;
uint16_t GeoFence_Rad = 0;
float DistanceFrom_GeoFenceMidpoint = 0.0;

String str_GeoFenceMidPoint_Latitude = "";
String str_GeoFenceMidPoint_Longitude = "";
String str_GeoFenceRadius = "";
String str_DeviceID = "";
String str_DevicePW = "";


uint32_t isr_tmr_Counter = 0;
uint32_t GeoAlarm_AlarmInterrupt = 8000;
