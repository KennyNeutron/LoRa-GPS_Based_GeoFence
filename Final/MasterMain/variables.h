//Store All Variables Here
uint32_t ihap = 0;
uint8_t GeoSerialDATA = 0xFF;

uint16_t NumberOfBytesToRead = 0;

char ch_GeoSerialDATA[100] = "";
uint8_t IICdata[100] = "";

float GeoFenceMidPoint_Latitude = 0.00;
float GeoFenceMidPoint_Longitude = 0.00;
uint16_t GeoFenceRadius = 0;
float DistanceFrom_GeoFenceMidpoint = 0.0;

String str_GeoFenceMidPoint_Latitude = "";
String str_GeoFenceMidPoint_Longitude = "";
String str_GeoFenceRadius = "";
String str_DeviceID_Recieved = "";
String str_DevicePW_Recieved = "";


char SlaveID_fromMaster[6] = "xxxxxx";
char SlavePW_fromMaster[5] = "0000";
bool PairCommand_fromApp = false;

uint8_t currentSlaveAssigment = 0;

bool SlaveStatus[7] = { false, false, false, false, false, false };

bool GeoAlarm = false;

uint32_t isr_tmr_Counter = 0;
uint32_t GeoAlarm_AlarmInterrupt = 8000;
bool BuzzerToggle = false;


bool AlarmTypeMaster = false;

uint32_t alarm_last_millis = 0;
