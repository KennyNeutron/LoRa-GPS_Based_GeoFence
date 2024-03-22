void DecodeToFloat() {
  int b = 1;
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

  while (ch_GeoSerialDATA[b] != 'B') {
    str_GeoFenceRadius = str_GeoFenceRadius + ch_GeoSerialDATA[b];
    b++;
  }

  Serial.print("STR Lat:");
  Serial.println(str_GeoFenceMidPoint_Latitude);
  Serial.print("STR Lon:");
  Serial.println(str_GeoFenceMidPoint_Longitude);
  Serial.print("STR Rad:");
  Serial.println(str_GeoFenceRadius);

  GeoFenceMidPoint_Latitude = str_GeoFenceMidPoint_Latitude.toFloat();
  GeoFenceMidPoint_Longitude = str_GeoFenceMidPoint_Longitude.toFloat();
  GeoFenceRadius = str_GeoFenceRadius.toInt();

  Serial.print("\nFloat Lat:");
  Serial.println(GeoFenceMidPoint_Latitude, 7);
  Serial.print("Float Lon:");
  Serial.println(GeoFenceMidPoint_Longitude, 7);
  Serial.print("Int Rad:");
  Serial.println(GeoFenceRadius);

  str_GeoFenceMidPoint_Latitude = "";
  str_GeoFenceMidPoint_Longitude = "";
  str_GeoFenceRadius = "";
}
