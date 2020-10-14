#ifndef __MD_H__
#define __MD_H__

class MD{
    public:

    //SPEED_MODE val : [rps]    ANGLE_MODE val : [deg] 
    double now_val;

    virtual void Move(double val) = 0;
    virtual void Brake();

    void Reverse(bool rev);  

    protected:   
    bool reverse = false;   
};

#endif