#ifndef __FILTERED_DOUBLE_H__
#define __FILTERED_DOUBLE_H__

class Filtered_double{
    private:
    double slope;
    double now_val;
    double target_val;
    public:
    Filtered_double();
    Filtered_double(double Slope);
    Filtered_double &operator= (double val);
    void SetSlope(double Slope);
    Filtered_double &operator<<= (double Slope);
    double GetNowVal();
    operator double();
};

#endif