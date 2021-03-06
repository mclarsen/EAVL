// Copyright 2010-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EAVL_SCATTER_OP_H
#define EAVL_SCATTER_OP_H

#include "eavlCUDA.h"
#include "eavlDataSet.h"
#include "eavlArray.h"
#include "eavlOpDispatch.h"
#include "eavlOperation.h"
#include "eavlException.h"
#include <time.h>
#ifdef HAVE_OPENMP
#include <omp.h>
#endif

#ifndef DOXYGEN

struct eavlScatterOp_CPU
{
    static inline eavlArray::Location location() { return eavlArray::HOST; }
    template <class F, class IN, class OUT, class INDEX>
    static void call(int nitems, int,
                     const IN inputs, OUT outputs,
                     INDEX indices, F&)
    {
        int *denseindices = get<0>(indices).array;

#pragma omp parallel for
        for (int sparseindex = 0; sparseindex < nitems; ++sparseindex)
        {
            int denseindex = denseindices[get<0>(indices).indexer.index(sparseindex)];
            // can't use operator= because it's ambiguous when only
            // one input and one output array (without a functor that
            // would force a cast to a known type situation).
            collect(denseindex, outputs).CopyFrom(collect(sparseindex, inputs));
        }
    }
};

#if defined __CUDACC__

template <class IN, class OUT, class INDEX>
__global__ void
eavlScatterOp_kernel(int nitems,
                    const IN inputs, OUT outputs,
                    INDEX indices)
{
    int *denseindices = get<0>(indices).array;

    const int numThreads = blockDim.x * gridDim.x;
    const int threadID   = blockIdx.x * blockDim.x + threadIdx.x;
    for (int sparseindex = threadID; sparseindex < nitems; sparseindex += numThreads)
    {
        int denseindex = denseindices[get<0>(indices).indexer.index(sparseindex)];
        // can't use operator= because it's ambiguous when only
        // one input and one output array (without a functor that
        // would force a cast to a known type situation).
        collect(denseindex, outputs).CopyFrom(collect(sparseindex, inputs));
    }
}


struct eavlScatterOp_GPU
{
    static inline eavlArray::Location location() { return eavlArray::DEVICE; }
    template <class F, class IN, class OUT, class INDEX>
    static void call(int nitems, int,
                     const IN inputs, OUT outputs,
                     INDEX indices, F&)
    {
        int numThreads = 256;
        dim3 threads(numThreads,   1, 1);
        dim3 blocks (32,           1, 1);
        eavlScatterOp_kernel<<< blocks, threads >>>(nitems,
                                                   inputs, outputs,
                                                   indices);
        CUDA_CHECK_ERROR();
    }
};


#endif

#endif

#include "eavlExplicitConnectivity.h"
// ****************************************************************************
// Class:  eavlScatterOp
//
// Purpose:
///   A simple scatter operation on a single input and output array; copies
///   the values specified by the indices array from the source array to
///   the destination array.
///   Example: 
///      output array before operation: [0 0 0 0 0]
///                           input   : [8 5 9]
///                         indexes   : [2 1 4]
//                          output    : [0 5 8 0 9]
//
// Programmer:  Jeremy Meredith
// Creation:    August  1, 2013
//
// Modifications:
//     Matt Larsen- February 5, 2014 (used eavlGatherOp as a template)
// ****************************************************************************
template <class I, class O, class INDEX>
class eavlScatterOp : public eavlOperation
{
  protected:
    DummyFunctor functor;
    I            inputs;
    O            outputs;
    INDEX        indices;
  public:
    eavlScatterOp(I i, O o, INDEX ind)
        : inputs(i), outputs(o), indices(ind)
    {
    }
    virtual void GoCPU()
    {
        int dummy;
        int n = inputs.first.length();
        cerr<<"numInputs "<<n<<endl;
        eavlOpDispatch<eavlScatterOp_CPU>(n, dummy, inputs, outputs, indices, functor);
    }
    virtual void GoGPU()
    {
#ifdef HAVE_CUDA
        int dummy;
        int n = inputs.first.length();
        eavlOpDispatch<eavlScatterOp_GPU>(n, dummy, inputs, outputs, indices, functor);
#else
        THROW(eavlException,"Executing GPU code without compiling under CUDA compiler.");
#endif
    }
};

// helper function for type deduction
template <class I, class O, class INDEX>
eavlScatterOp<I,O,INDEX> *new_eavlScatterOp(I i, O o, INDEX indices) 
{
    return new eavlScatterOp<I,O,INDEX>(i,o,indices);
}


#endif
