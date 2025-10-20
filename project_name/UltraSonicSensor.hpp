#ifndef ULTRASONIC_SENSOR_HPP
#define ULTRASONIC_SENSOR_HPP


class UltrasonicSensor {
  private:
    int signalPin;

  public:
    UltrasonicSensor(int pin);
    float measureDistance();  // retourne la distance en cm
};

#endif