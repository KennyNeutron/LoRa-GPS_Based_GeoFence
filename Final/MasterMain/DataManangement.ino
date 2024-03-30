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

  while (ch_GeoSerialDATA[b] != ',') {
    str_GeoFenceRadius = str_GeoFenceRadius + ch_GeoSerialDATA[b];
    b++;
  }

  b++;

  while (ch_GeoSerialDATA[b] != ',') {
    str_DeviceID_Recieved = str_DeviceID_Recieved + ch_GeoSerialDATA[b];
    b++;
  }

  b++;

  while (1) {
    str_DevicePW_Recieved = str_DevicePW_Recieved + ch_GeoSerialDATA[b];
    if (ch_GeoSerialDATA[b] == 'B' || ch_GeoSerialDATA[b] == 'C') {
      Serial.print("SLAVE ASSIGNMENT:");
      Serial.println(ch_GeoSerialDATA[b - 1], DEC);
      MasterPayload.SlaveAssignment = ch_GeoSerialDATA[b - 1] - '0';
      currentSlaveAssigment = MasterPayload.SlaveAssignment;
      goto endSetup;
    }
    b++;
  }

endSetup:
  if (ch_GeoSerialDATA[b] == 'B') {
    PairCommand_fromApp = true;
    Serial.println("Pairing Command!");
    Serial.println("SLAVE #" + String(currentSlaveAssigment));
    EEPROM.write(100 + currentSlaveAssigment, PairCommand_fromApp);
  } else if (ch_GeoSerialDATA[b] == 'C') {
    PairCommand_fromApp = false;
    Serial.println("Terminate Command!");
    Serial.println("SLAVE #" + String(currentSlaveAssigment));
    EEPROM.write(100 + currentSlaveAssigment, PairCommand_fromApp);
  }

  Serial.print("STR Lat:");
  Serial.println(str_GeoFenceMidPoint_Latitude);
  Serial.print("STR Lon:");
  Serial.println(str_GeoFenceMidPoint_Longitude);
  Serial.print("STR Rad:");
  Serial.println(str_GeoFenceRadius);
  Serial.print("STR Device ID Recieved:");
  Serial.println(str_DeviceID_Recieved);
  Serial.print("STR Device PIN Code Recieved:");
  Serial.println(str_DevicePW_Recieved);

  GeoFenceMidPoint_Latitude = str_GeoFenceMidPoint_Latitude.toFloat();
  GeoFenceMidPoint_Longitude = str_GeoFenceMidPoint_Longitude.toFloat();
  GeoFenceRadius = str_GeoFenceRadius.toInt();

  Serial.print("\nFloat Lat:");
  Serial.println(GeoFenceMidPoint_Latitude, 7);
  Serial.print("Float Lon:");
  Serial.println(GeoFenceMidPoint_Longitude, 7);
  Serial.print("Int Rad:");
  Serial.println(GeoFenceRadius);

  str_DeviceID_Recieved.toCharArray(SlaveID_fromMaster, 7);
  str_DevicePW_Recieved.toCharArray(SlavePW_fromMaster, 5);

  str_GeoFenceMidPoint_Latitude = "";
  str_GeoFenceMidPoint_Longitude = "";
  str_GeoFenceRadius = "";
  str_DeviceID_Recieved = "";
  str_DevicePW_Recieved = "";
}
