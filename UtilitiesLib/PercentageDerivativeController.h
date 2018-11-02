#pragma once
class PercentageDerivativeController
{
public:
    PercentageDerivativeController() {};
    PercentageDerivativeController(double kp, double kd, double ki);
    ~PercentageDerivativeController();
    double calculate(double target, double actual, double elapsed);
private:
    double lastTime = 0.0;
    double errorPrior = 0.0;
    double integral = 0.0;
    double KP = 0.5;
    double KD = 0.5;
    double KI = 0.5;
};

