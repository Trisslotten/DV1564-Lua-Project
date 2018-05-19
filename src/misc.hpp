#pragma once

#include <irrlicht.h>

inline void setVec(irr::core::vector3df& vec, int i, float value)
{
	switch (i)
	{
	case 0:
		vec.X = value;
		break;
	case 1:
		vec.Y = value;
		break;
	case 2:
		vec.Z = value;
		break;
	}
}