void GeoFenceInterrupt() {
  cli();  // disable interrupts

  // set timer2 interrupt at 8kHz
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 249;
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS21);
  TIMSK2 |= (1 << OCIE2A);

  sei();  // enable interrupts
}

ISR(TIMER2_COMPA_vect) {
  // timer2 interrupt 8kHz
  cli();
  isr_tmr_Counter++;
  if (isr_tmr_Counter >= GeoAlarm_AlarmInterrupt) {
    isr_tmr_Counter = 0;
    BuzzerToggle = !BuzzerToggle;
  }

  if (AlarmTypeMaster > 0) {
    if (!BuzzerToggle) {
      analogWrite(buzzerPin, 100);
    } else {
      analogWrite(buzzerPin, 255);
    }
  } else {
    analogWrite(buzzerPin, 255);
  }



  sei();
}
