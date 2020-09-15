#ifndef BITSTOSHIFT_H
#define BITSTOSHIFT_H

class BitsToShift {
public:
  BitsToShift() {
    bits = 0;
  }
  void setBit(uint8_t reg_bit, bool set) {
    if (set)
      bits = bits | reg_bit;
    else
      bits = bits & ~reg_bit;
    setRegister();
  }
  bool getBit(uint8_t reg_bit) {
    return (bits & reg_bit) > 0;
  }

private:
  uint8_t bits;
  
  void setRegister() {
    digitalWrite(LATCH_PIN, LOW); // начинаем передачу данных
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, bits ^ 0b11111111);
    digitalWrite(LATCH_PIN, HIGH); // прекращаем передачу данных
  }
};


#endif
