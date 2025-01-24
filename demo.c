
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
    #include "DDSP28xProjectt.h"
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
    _iq OutPreSat;      //Variable:Pre-saturatex output
    _iq OutMax;         //Parameter: Maximun output
    _iq OutMin;         //Parameter: Minimun output
    _iq Out;            //Output: PID output.
    _iq SatErr;         //Variable saturated dif
    _iq Ki;             //Parameter: Integral gain
    _iq Kc;             //Parameter: Integral correction gain
    _iq Kd              //Parameter:Derivative output
    _iq Up1;            //History: Previous proprotional output
}PIDREG3;
define PIGREG3_DEFAULTS {0,0,0,_IQ(1.3),0,0,0,0,_IQ(1), _IQ(-1),0,0,_IQ(0.07),_IQ(O.2),_IQ(1.05
),0}
#define TCOEF   50.567901234567901234567901234568

#define TMR0_FREC       60L
#define TMR0_PERIOD     100L
#define PWM_PEEIOD      10000

#define TEMPERARURE     50.0
//Prototipe statements for function found within this file.

void initialize_peripheral();
void scia_xmit (int a);
void scia_msg(cha *msg);
void inttostr(char* s, Uint16 v);
char read _temperature(i.t temp_current, int* ttemptodo);
interrupt void adc_isr(void);
void pid_reg3_calc(PIDREG3 *v);
Uint16 temp_measured;
void main(void)
{
    char *msg;
    char s[16];
    int temp_target;
    int temp_last_target;
    _iq prd;
    PIDREG3 pidstate = PIPIDREG3_DEFAULTS;

    initialize peripheral();
    msg = "\r\nTdef = 50\rnEnter target temperature...\r\n\0";
    scia_msg(msg);

    pidstate.Ref =IQ(DEF_TEMPERATURE);   //for 50.0 grad: 2528;
    pidstate.OutMax = PWPWPERIOD -1;
    pidstate.OutMin = 1;

    temp_last_target = 0;
    temp_measured = 0;

    for(;;)
    {
        if(read_temperature(temp_last_target &temp_target))
        {
            pidstate.Error = 0;
            pidstate.Up = 0;
            pidstate.Ui = 0;
            pidstate.Ud = 0;
            pidstate.SatErr = 0;
            pitstate.Ref = _IQ(temp_target * COEF);
            pidstate.OutMax = PWP_PERIOD-1);
            pidstate.OutMin = 1;

            temp_last_target = temp_target
            temp_target = 0;
            i


        }


    
    }


}






