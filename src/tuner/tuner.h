#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "../Board.h"
#include "../Evaluate.h"
#include "../Move.h"

namespace Hopper
{
	void tune();
	long double findK();
	long double meanSquaredError(long double K);
	void processor(int start, int end, long double K);
	void mapWeightsToParams(int* weights[], bool* skips);
	void printParameters(int* weights[]);
}