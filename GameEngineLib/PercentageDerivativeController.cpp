#include "stdafx.h"
#include "PercentageDerivativeController.h"


PercentageDerivativeController::PercentageDerivativeController(double kp, double kd, double ki)
    : KP(kp), KD(kd), KI(ki)
{
}


PercentageDerivativeController::~PercentageDerivativeController()
{
}

double PercentageDerivativeController::calculate(double target, double actual, double elapsed)
{
    double error = target - actual;
    double derivative = (error - errorPrior) / elapsed;
    integral += error * elapsed;
    double output = KP * error + KD * derivative + KI * integral;
    errorPrior = error;
    return output;
}
