void gps_loop() {
  // This sketch displays information every time a new sentence is correctly encoded.
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      displayInfo();
    }
  }

  /*
    if (millis() > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while (true)
        ;
    }*/
}





void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    gps_lat = String(gps.location.lat(), 6);
    SlavePayload.slaveLat = gps.location.lat();
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    gps_lon = String(gps.location.lng(), 6);
    SlavePayload.slaveLon = gps.location.lng();
    valid_location = true;
  } else {
    Serial.println(F("INVALID"));
    valid_location = false;
  }

  SlavePayload.LocationValid = valid_location;
}
