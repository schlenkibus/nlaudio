/******************************************************************************/
/** @file		flanger.h
    @date		2017-06-08
    @version            0.1
    @author		Matthias Seeber & Anton Schmied[2018-01-23]
    @brief		An implementation of the Parameter Renderer
                        and TDC INterpreter

    @todo
*******************************************************************************/

#pragma once

#include "nltoolbox.h"
#include "nlglobaldefines.h"


class Paramengine
{
public:

    Paramengine();              // Default Constructor

    ~Paramengine();             // Destructor

    void setParams(unsigned char _ctrlID, float _ctrlVal);

private:
};

