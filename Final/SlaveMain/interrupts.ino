void GeoFenceInterrupt() {
  cli();  // disable interrupts

  // set timer0 interrupt at 2kHz
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0 = 0;
  OCR0A = 124;
  TCCR0A |= (1 << WGM01);
  TCCR0B |= (1 << CS01) | (1 << CS00);
  TIMSK0 |= (1 << OCIE0A);

  sei();  // enable interrupts
}

// Interrupt for Push Buttons
ISR(TIMER0_COMPA_vect) {
  cli();
  isr_tmr_Counter++;
  if (isr_tmr_Counter >= GeoAlarm_AlarmInterrupt) {
    isr_tmr_Counter = 0;
    if (DevicePaired) {
      BuzzerToggle = !BuzzerToggle;
    }
  }

  if (DevicePaired) {
    if (!BuzzerToggle) {
      analogWrite(BuzzerPin, 100);
    } else {
      analogWrite(BuzzerPin, 255);
    }
  } else {
    analogWrite(BuzzerPin, 255);
  }

  sei();
}
