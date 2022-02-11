#include "Static_Refrence.h"

std::vector<unsigned short> s_GetMotionstatus;
long Static_Refrence::s_GetSpeedcounter;
double Static_Refrence::s_GetCommandCounter ;

Static_Refrence::Static_Refrence(unsigned short AxisNumber)
{
    s_GetMotionstatus;
    s_GetSpeedcounter = 0;
    s_GetCommandCounter = 0;
}
