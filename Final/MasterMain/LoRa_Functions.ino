void LoRa_SendDataToSlave() {
  Serial.println("Sending LoRa Packet");

  MasterPayload.PairCommand = PairCommand_fromApp;
  strcpy(MasterPayload.SlaveID, SlaveID_fromMaster);
  strcpy(MasterPayload.SlavePW, SlavePW_fromMaster);

  MasterPayload.SlaveAssignment = 1;

  MasterPayload.GeofenceMidpoint_Lat = GeoFenceMidPoint_Latitude;
  MasterPayload.GeofenceMidpoint_Lon = GeoFenceMidPoint_Longitude;
  MasterPayload.GeofenceRad = GeoFenceRadius;

  LoRa.beginPacket();
  LoRa.write((uint8_t *)&MasterPayload, sizeof(MasterPayload));
  LoRa.endPacket();
}
