void DecodeGeofenceData() {
  str_GeoFenceMidPoint_Latitude = "";
  str_GeoFenceMidPoint_Longitude = "";
  str_GeoFenceRadius = "";
  for (int x = 1; x <= 6; x++) {
    SlaveStatus[x] = ch_GeoSerialDATA[x];
    Serial.println("STATUS Slave [" + String(x) + "]:" + String(SlaveStatus[x]));
    EEPROM.write(100 + x, SlaveStatus[x]);
  }

  int b = 7;
  while (ch_GeoSerialDATA[b] != ',') {
    str_GeoFenceMidPoint_Latitude = str_GeoFenceMidPoint_Latitude + ch_GeoSerialDATA[b];
    b++;
  }
  b++;
  while (ch_GeoSerialDATA[b] != ',') {
    str_GeoFenceMidPoint_Longitude = str_GeoFenceMidPoint_Longitude + ch_GeoSerialDATA[b];
    b++;
  }
  b++;
  while (ch_GeoSerialDATA[b] != ',') {
    str_GeoFenceRadius = str_GeoFenceRadius + ch_GeoSerialDATA[b];
    b++;
  }
  Serial.print("\n\n\nGeofence Lat:");
  Serial.println(str_GeoFenceMidPoint_Latitude);
  Serial.print("Geofence Lon:");
  Serial.println(str_GeoFenceMidPoint_Longitude);
  Serial.print("Geofence Rad:");
  Serial.println(str_GeoFenceRadius);

  GeoFenceMidPoint_Latitude = str_GeoFenceMidPoint_Latitude.toFloat();
  GeoFenceMidPoint_Longitude = str_GeoFenceMidPoint_Longitude.toFloat();
  GeoFenceRadius = str_GeoFenceRadius.toInt();
}