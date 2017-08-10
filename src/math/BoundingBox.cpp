/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#ifdef HAVE_EIGEN
#include "nsessentials/math/BoundingBox.h"

namespace nse 
{
	namespace math
	{
		template struct BoundingBox<float, 3>;
	}
}
#endif