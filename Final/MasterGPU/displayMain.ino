bool displayMain_initialize = false;

void display_Main() {
  if (!displayMain_initialize) {
    displayMain_init();
  }
  show_string(String(GeoFenceMidPoint_Latitude, 6), 150, 320, 2, YELLOW, BLACK, 0);
  show_string(String(GeoFenceMidPoint_Longitude, 6), 150, 340, 2, YELLOW, BLACK, 0);
  show_string(String(GeoFenceRadius) + "              ", 150, 360, 2, YELLOW, BLACK, 0);

  uint16_t coordY = 0;
  for (int y = 1; y <= 6; y++) {
    if (SlaveStatus[y]) {
      show_string("PAIRED   ", 230, coordY, 5, GREEN, BLACK, 0);
      if (SlaveMeddle[y]) {
        show_string("MEDDLED", 500, coordY, 3, PINK, BLACK, 0);
        if (AlarmType[y]) {
          show_string("OUTSIDE!      ", 650, coordY, 3, RED, BLACK, 0);
        } else {
          show_string("INSIDE!       ", 650, coordY, 3, ORANGE, BLACK, 0);
        }
      } else {
        show_string("                         ", 500, coordY, 3, PINK, BLACK, 0);
      }

    } else {
      show_string("UNPAIRED                                        ", 230, coordY, 5, RED, BLACK, 0);
    }


    coordY = coordY + 40;
  }
}

void displayMain_init() {
  displayMain_initialize = true;
  tft.Fill_Screen(BLACK);
  show_string("SLAVE 1: ", 10, 0, 4, WHITE, WHITE, 0);
  show_string("SLAVE 2: ", 10, 40, 4, WHITE, WHITE, 0);
  show_string("SLAVE 3: ", 10, 80, 4, WHITE, WHITE, 0);
  show_string("SLAVE 4: ", 10, 120, 4, WHITE, WHITE, 0);
  show_string("SLAVE 5: ", 10, 160, 4, WHITE, WHITE, 0);
  show_string("SLAVE 6: ", 10, 200, 4, WHITE, WHITE, 0);

  show_string("GEOFENCE MIDPOINT", 10, 300, 2, CYAN, BLACK, 0);
  show_string("Latitude:", 10, 320, 2, WHITE, WHITE, 0);
  show_string("Longitude:", 10, 340, 2, WHITE, WHITE, 0);
  show_string("Radius:", 10, 360, 2, WHITE, WHITE, 0);
}