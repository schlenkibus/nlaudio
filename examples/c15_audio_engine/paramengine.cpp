/******************************************************************************/
/** @file		flanger.cpp
    @date		2018-01-23
    @version            0.1
    @author		Matthias Seeber & Anton Schmied[2018-01-23]
    @brief		Paramengine Class member and method definitions
*******************************************************************************/

#include "paramengine.h"

/******************************************************************************/
/** Paramengine Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           SampleRate:            48 kHz
*******************************************************************************/

Paramengine::Paramengine()
{

}



/******************************************************************************/
/** Paramengine Destructor
 * @brief
*******************************************************************************/

Paramengine::~Paramengine()
{

}



/*****************************************************************************/
/** @brief    interface method which converts and scales the incoming midi
 *            values and passes these to the respective methods
 *  @param    midi control value [0 ... 127]
 *  @param    midi control ID -> enum CtrlID
******************************************************************************/

void Paramengine::setParams(unsigned char _ctrlID, float _ctrlVal)
{
    ParamSignalData::instance().setSignalValue(0, 10, _ctrlVal);
    printf("Signal Data - Voice 0, Siganl ID 10: %f\n", ParamSignalData::instance().getSignalValue(0, 10));

//    PARAMSIGNALDATA[0][4] = _ctrlVal;
//    printf("Signal Data - Voice 0, Siganl ID 4: %f\n", PARAMSIGNALDATA[0][4]);
//    printf("Signal Data - Voice 0, Siganl ID 2: %f\n", PARAMSIGNALDATA[0][2]);
}
