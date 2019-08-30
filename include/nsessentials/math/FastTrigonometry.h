/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#include <cmath>

namespace nse
{
	namespace math
	{

		//Approximates the arccos function with an absolute error of about 1°.
		template <typename T>
		T FastAcos(T x)
		{
			//formula from https://stackoverflow.com/a/36387954/1210053
			
			T xSq = (T)(x * x);
		#ifdef FP_FAST_FMA
			T num = x * std::fma((T)0.9217841528914573, xSq, (T)-0.939115566365855);
			T denom = 1 + xSq * std::fma((T)0.295624144969963174, xSq, (T)-1.2845906244690837);
		#else
			T num = x * ((T)-0.939115566365855 + (T)0.9217841528914573 * xSq);
			T denom = 1 + xSq * ((T)-1.2845906244690837 + (T)0.295624144969963174 * xSq);
		#endif	
			return (T)M_PI_2 + num / denom;
		}

		//Approximates the arcsin function with an absolute error of about 1°.
		template <typename T>
		T FastAsin(T x)
		{
			return (T)M_PI_2 - FastAcos(x);
		}
	}
}