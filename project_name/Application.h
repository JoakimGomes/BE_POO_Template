#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "UltraSonicSensor.hpp"

class Application
{
  private:
    UltrasonicSensor* SensorSon;  // <-- membre de classe
    int buzzerPin;

  public:
    Application();
    ~Application();

    void init(void);
    void run(void);
};

#endif