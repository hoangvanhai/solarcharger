#ifndef MPPT_PNO_F_H_
#define MPPT_PNO_F_H_

#include <arm_cm0.h>
#include <uart.h>
#include <MKE06Z4.h>


typedef float           	float32;

//*********** Structure Definition ********//
#if 1
typedef struct {
	float32  Ipv;
	float32  Vpv;
	float32  DeltaPmin;
	float32  MaxVolt;
	float32  MinVolt;
	float32  Stepsize;
	float32  VmppOut;
	float32  DeltaP;
	float32  PanelPower;
	float32  PanelPower_Prev;
	int16 mppt_enable;
	int16 mppt_first;
} MPPT_PNO_F;

#endif 


#if 0
typedef struct {
	float   Ipv;
	float Vpv;
	float DeltaPmin;
	float MaxOut;
	float MinOut;
	float MpptOut;
	float MpptOut_1;
	float Vpv_open;

	// Internal variables
	float DeltaP;
	float Power;
	float Power_Prev;
	float Vpv_Prev;
	float DeltaVpv;
	float MPPT_Enable;

	float MaxDeltaV;
	float Maxstep;
	float Minstep;
} MPPT_PNO_F;
*/
typedef MPPT_PNO_F *mppt_pno_handle;

//*********** Function Declarations *******//



#define MPPT_PNO_INIT_MACRO(v)						\
	v.Ipv = 0.0;									\
	v.Vpv = 0.0;									\
	v.DeltaPmin = 0;					   	        \
	v.MaxOut = MPPT_VOLT_OUT_MAX;					\
	v.MinOut = MPPT_VOLT_OUT_MIN;				    \
	v.MpptOut = 0.0;								\
	v.Vpv_open = 21770;							    \
													\
	/* Internal variables */						\
	v.DeltaP = 0.0;									\
	v.Power = 0.0;								    \
	v.Power_Prev = 0.0;						        \
	v.Vpv_Prev = 0.0;								\
	v.DeltaVpv = 0.0;						        \
	v.MPPT_Enable = 1;								\
													\
	v.MaxDeltaV = 10;							    \
	v.Maxstep = 50;							    \
	v.Minstep = 30;

#define MPPT_PNO_F_MACRO(v)												\
if (v.MPPT_Enable == 1){												\
		v.Power = (v.Vpv * v.Ipv);						    	    \
		v.DeltaP = v.Power - v.Power_Prev;					            \
		v.DeltaVpv = v.Vpv - v.Vpv_Prev;					            \
		/*if(v.Vpv > v.Vpv_open - 2.92)v.MpptOut += 0.1;*/	            \
        if(v.Vpv > v.Vpv_open - 4.0)v.MpptOut += 0.01;                \
		if (v.DeltaP >= v.DeltaPmin){									\
			if(v.DeltaVpv < -v.MaxDeltaV)    {v.MpptOut += v.Minstep; } \
			else if(v.DeltaVpv < 0)          {v.MpptOut += v.Maxstep; } \
			else if(v.DeltaVpv > v.MaxDeltaV){v.MpptOut -= v.Maxstep; } \
			else if(v.DeltaVpv > 0)          {v.MpptOut -= v.Minstep; } \
		}																\
		if (v.DeltaP < -v.DeltaPmin){									\
			if(v.DeltaVpv < -v.MaxDeltaV)    {v.MpptOut -= v.Minstep; } \
			else if(v.DeltaVpv < 0)          {v.MpptOut -= v.Maxstep; } \
			else if(v.DeltaVpv > v.MaxDeltaV){v.MpptOut += v.Maxstep; } \
			else if(v.DeltaVpv > 0)          {v.MpptOut += v.Minstep; } \
		}																\
		v.Power_Prev = v.Power;								            \
		v.Vpv_Prev = v.Vpv;  										    \
																		\
	v.MpptOut = (v.MpptOut < v.MinOut) ? v.MinOut : v.MpptOut;		    \
	v.MpptOut = (v.MpptOut > v.MaxOut) ? v.MaxOut : v.MpptOut;		    \
}


#endif

#define MPPT_PNO_F_MACRO(v)												\
if (v.mppt_enable==1)													\
{																		\
	if (v.mppt_first == 1)												\
	{																	\
		v.VmppOut= v.Vpv - (50);										\
		v.mppt_first=0;													\
		v.PanelPower_Prev=v.PanelPower;									\
	}																	\
	else																\
	{																	\
		v.PanelPower = (v.Vpv*v.Ipv);									\
		v.DeltaP = v.PanelPower - v.PanelPower_Prev;					\
		if (v.DeltaP > v.DeltaPmin)										\
		{																\
			v.VmppOut += v.Stepsize;									\
		}																\
		else															\
		{																\
			if (v.DeltaP < -v.DeltaPmin)								\
			{															\
				v.VmppOut -= v.Stepsize;								\
			}															\
		}																\
		v.PanelPower_Prev = v.PanelPower;								\
	}																	\
	if(v.VmppOut< v.MinVolt) v.VmppOut = v.MinVolt;						\
	if(v.VmppOut> v.MaxVolt) v.VmppOut= v.MaxVolt;						\
}	

void MPPT_PNO_F_init(MPPT_PNO_F *v);
void MPPT_PNO_F_FUNC(MPPT_PNO_F *v);

#endif /*MPPT_PNO_F_H_*/
