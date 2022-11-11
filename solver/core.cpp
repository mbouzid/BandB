#include "core.h"


double core::heuristic::ratioA(int16_t C, uint16_t p, uint16_t w)
{
    return (double)C / (double)w; ;
}

double core::heuristic::ratioB(int16_t C, uint16_t p, uint16_t w)
{
    return ((double)C - (double)p) / (double)w;
}

double core::heuristic::ratioC(int16_t C, uint16_t p, uint16_t w)
{
    return ratioA(C,p,w) + ratioB(C,p,w);
}