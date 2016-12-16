/******************************************************************************/
/** @file		shaper.cpp
    @date		2016-10-14
    @version    0.1
    @author		Anton Schmied[2016-10-14]
    @brief		Shaper Class member and method definitions
*******************************************************************************/

#include "shaper.h"

/******************************************************************************/
/** Shaper Default Constructor
 * @brief    initialization of the modules local variabels with default values
 *           Drive:                 0.18f
 *           Fold:                  0.5
 *           Asym:                  0.f
*******************************************************************************/

Shaper::Shaper()
    : mDrive(0.18f)
    , mFold(0.5f)
    , mAsym(0.f)
{
}



/******************************************************************************/
/** Shaper Parameterized Constructor
 * @brief    initialization of the modules local variabels with predefined values
*******************************************************************************/

Shaper::Shaper(float _drive, float _fold, float _asym)
    : mDrive(_drive)
    , mFold(_fold)
    , mAsym(_asym)
{
}



/******************************************************************************/
/** Main Shaper Function
 *  @param raw sample
 *  @return shape processed sample
*******************************************************************************/

float Shaper::applyShaper(float _sample)
{
    _sample *= mDrive;

    float ctrl = _sample;

    _sample = NlToolbox::Math::sinP3(_sample);
    _sample = NlToolbox::Others::threeRanges(_sample, ctrl, mFold);

    float sample_square = _sample * _sample + (-0.5f);

    _sample = NlToolbox::Others::parAsym(_sample, sample_square, mAsym);

    return _sample;
}



/******************************************************************************/
/** setter function for the shaper drive
 * @param   drive amount
*******************************************************************************/

void Shaper::setDrive(float _drive)
{
    mDrive = _drive;
}



/******************************************************************************/
/** setter function for the shaper fold
 * @param   fold amount
*******************************************************************************/

void Shaper::setFold(float _fold)
{
    mFold = _fold;
}



/******************************************************************************/
/** setter function for the shaper asymmetrie
 * @param   asymmetry amount
*******************************************************************************/

void Shaper::setAsym(float _asym)
{
    mAsym = _asym;
}
