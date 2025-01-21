
include "DDSP28xProjectt.h"
#define MATH TYPE      IQ_MATH
#define GLOBAL_Q       16
#include "IQmathhLib.n"

typedef struct
{
    _iq Ref;         //Imput: Reference input
        _iq Fdb;     //Input; Feetbak input
        _iq Err;            //variable error
    _iq Kp;                 //Parameter: Proportional gain
    _iq Up;                 //Variable: Proportional gain
    _iq Ui;                 //Variable: Integral output
    _iq Ud:                 //Variable: Derivative output
    _iq OutPreSat;    //Variable:Pre-saturatex output
    _iq OutMax;       //Parameter: Maximun output
    _iq OutMin;       //Parameter: Minimun output
    _iq Out;          //Output: PID output
    

