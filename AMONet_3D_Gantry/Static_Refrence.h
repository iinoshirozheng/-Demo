#ifndef STATIC_REFRENCE_H
#define STATIC_REFRENCE_H

#include <vector>

class Static_Refrence
{
public:
    Static_Refrence(unsigned short AxisNumber);
    static std::vector<unsigned short> s_GetMotionstatus;
    static long s_GetSpeedcounter;
    static double s_GetCommandCounter ;
};

#endif // STATIC_REFRENCE_H
