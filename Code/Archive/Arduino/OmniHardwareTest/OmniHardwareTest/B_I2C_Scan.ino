void i2c_scan(void) {
  // Scan through each of the multiplexer ports
  for (uint8_t t=0; t<8; t++) {   
    tcaselect(t);                                   // Switch multiplexer port
    Serial.print("TCA Port #"); Serial.print(t);  // Print port to Serial Monitor

    if (t==1) {
      Serial.println("-Position Port");
    }
    else if (t==2) {
      Serial.println("-Temperature Port");
    }
    else {
      Serial.println("-Unused");
    }
    
    // Scan through each possible I2C address
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;    // Ignore the multiplexer I2C Address

      Wire.beginTransmission(addr);   // Begin transmission at each address
      if(!Wire.endTransmission()){    // If successful in closing the address (module is detected)
        Serial.print("  Found I2C 0x"); Serial.println(addr, HEX);  // Print to Serial Monitor
      }
    }
  }
}
