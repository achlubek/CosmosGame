#pragma once 

class AbsInterpolatorTask {
public:
    AbsInterpolatorTask(double istart, double iduration, double ismoothPower) 
        : start(istart), duration(iduration), smoothPower(ismoothPower)
    {

    }
    ~AbsInterpolatorTask() {

    }

    double getInterpolationCoefficent(double time) {
        double tmp = time - start;
        tmp /= duration;
        if (tmp >= 1.0) {
            finished = true;
        }
        tmp = glm::clamp(tmp, 0.0, 1.0);
        return smoothstep2(tmp, smoothPower);
    }

    virtual void update(double time) = 0;

    bool hasFinished() {
        return finished;
    }

    double getStart() {
        return start;
    }

    void setStart(double s) {
        start = s;
    }


private:
    double start;
    double duration;
    double smoothPower;
    bool finished = false;

    double smoothstep2(double t, double f) {
        t *= 2.0;
        double c = 1.0;
        if (t < 1.0)
        {
            double power = pow(t, f);
            return c / 2.0 * power;
        }
        else {
            t = t - 1.0;
            double power = pow(1.0 - t, f);
            return 1.0 - c / 2.0 * power;
        }
    }
};

class DoubleInterpolatorTask : public AbsInterpolatorTask {
public:
    DoubleInterpolatorTask(double* ivaluePointer, double target, double istart, double iduration, double ismoothPower)
        : AbsInterpolatorTask(istart, iduration, ismoothPower),
        startingValue(*ivaluePointer),
        valuePointer(ivaluePointer),
        endingValue(target)
    {

    }
    ~DoubleInterpolatorTask()
    {

    }

    virtual void update(double time) override {
        double coeff = getInterpolationCoefficent(time);
        if (coeff <= 0.0) {
            startingValue = *valuePointer;
        }
        *valuePointer = glm::mix(startingValue, endingValue, coeff);
    }

private:
    double startingValue;
    double endingValue;
    double* valuePointer;
};

class Vec3InterpolatorTask : public AbsInterpolatorTask {
public:
    Vec3InterpolatorTask(glm::dvec3* ivaluePointer, glm::vec3 target, double istart, double iduration, double ismoothPower)
        : AbsInterpolatorTask(istart, iduration, ismoothPower),
        startingValue(*ivaluePointer),
        valuePointer(ivaluePointer),
        endingValue(target)
    {

    }
    ~Vec3InterpolatorTask()
    {

    }

    virtual void update(double time) override {
        double coeff = getInterpolationCoefficent(time);
        if (coeff <= 0.0) {
            startingValue = *valuePointer;
        }
        *valuePointer = glm::mix(startingValue, endingValue, coeff);
    }

private:
    glm::dvec3 startingValue;
    glm::dvec3 endingValue;
    glm::dvec3* valuePointer;
};

class QuatInterpolatorTask : public AbsInterpolatorTask {
public:
    QuatInterpolatorTask(glm::dquat* ivaluePointer, glm::dquat target, double istart, double iduration, double ismoothPower)
        : AbsInterpolatorTask(istart, iduration, ismoothPower),
        startingValue(*ivaluePointer),
        valuePointer(ivaluePointer),
        endingValue(target)
    {

    }
    ~QuatInterpolatorTask()
    {

    }

    virtual void update(double time) override {
        double coeff = getInterpolationCoefficent(time);
        if (coeff <= 0.0) {
            startingValue = *valuePointer;
        }
        *valuePointer = glm::slerp(startingValue, endingValue, coeff);
    }

private:
    glm::dquat startingValue;
    glm::dquat endingValue;
    glm::dquat* valuePointer;
};

class Interpolator
{
public:
    Interpolator();
    ~Interpolator();

    void addDoubleInterpolator(double* valuePointer, double target, double start, double duration, double smoothPower);
    void addVec3Interpolator(glm::dvec3* valuePointer, glm::dvec3 target, double start, double duration, double smoothPower);
    void addQuatInterpolator(glm::dquat* valuePointer, glm::dquat target, double start, double duration, double smoothPower);
    void addPremadeInterpolator(AbsInterpolatorTask* task);

    void update(double time);

private:
    std::vector<AbsInterpolatorTask*> tasks{};
    void removeFinishedInterpolators();
};

