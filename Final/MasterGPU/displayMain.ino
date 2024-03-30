bool displayMain_initialize = false;

void display_Main() {
  if (!displayMain_initialize) {
    displayMain_init();
  }
}

void displayMain_init() {
  displayMain_initialize = true;
  tft.Fill_Screen(BLACK);
  show_string("SLAVE 1: ", 10, 0, 5, WHITE, WHITE, 0);
  show_string("SLAVE 2: ", 10, 50, 5, WHITE, WHITE, 0);
  show_string("SLAVE 3: ", 10, 100, 5, WHITE, WHITE, 0);
  show_string("SLAVE 4: ", 10, 150, 5, WHITE, WHITE, 0);
  show_string("SLAVE 5: ", 10, 200, 5, WHITE, WHITE, 0);
  show_string("SLAVE 6: ", 10, 250, 5, WHITE, WHITE, 0);

  show_string("PAIRED", 250, 0, 5, GREEN, BLACK, 0);
  show_string("PAIRED", 250, 50, 5, GREEN, BLACK, 0);
  show_string("UNPAIRED", 250, 100, 5, RED, BLACK, 0);
  show_string("UNPAIRED", 250, 150, 5, RED, BLACK, 0);
  show_string("UNPAIRED", 250, 200, 5, RED, BLACK, 0);
  show_string("UNPAIRED", 250, 250, 5, RED, BLACK, 0);
}