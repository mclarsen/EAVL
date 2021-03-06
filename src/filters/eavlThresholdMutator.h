// Copyright 2010-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EAVL_THRESHOLD_MUTATOR_H
#define EAVL_THRESHOLD_MUTATOR_H

#include "STL.h"
#include "eavlDataSet.h"
#include "eavlCellComponents.h"
#include "eavlFilter.h"

// ****************************************************************************
// Class:  eavlThresholdMutator
//
// Purpose:
///   Add a thresholded cell set to an existing data set
///   as well as stripped copies of the vars.
///   Note: this version creates an explicit standalone cell set.
//
// Programmer:  Jeremy Meredith, Dave Pugmire, Sean Ahern
// Creation:    April 13, 2012
//
// ****************************************************************************
class eavlThresholdMutator : public eavlMutator
{
  protected:
    double minval, maxval;
    string fieldname, cellsetname;
  public:
    eavlThresholdMutator();
    void SetRange(double vmin, double vmax)
    {
        minval = vmin;
        maxval = vmax;
    }
    void SetField(const string &name)
    {
        fieldname = name;
    }
    void SetCellSet(const string &name)
    {
        cellsetname = name;
    }
    
    virtual void Execute();
};

#endif

