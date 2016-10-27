/******************************************************************************/
/** @file		shaper.h
    @date		2016-10-14
    @version	0.1
    @author		Anton Schmied[2016-10-14]
    @brief		An implementation of an Shaper Class
                as used in the C15 and implemented in Reaktor
*******************************************************************************/

#pragma once
#include "nltoolbox.h"

class Shaper
{
public:
    Shaper();                   // Default Constructor

    Shaper(float _drive,        // Parameterized Constructor
           float _fold,
           float _asym);

    float applyShaper(float _sample);

    void setDrive(float _drive);
    void setFold(float _fold);
    void setAsym(float _asym);

private:
    float mDrive;               // Drive Amount
    float mFold;                // Fold Amount
    float mAsym;                // Drive Amount


};

