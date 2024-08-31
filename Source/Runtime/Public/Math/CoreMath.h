#pragma once

#ifndef EE_CORE_MATH
#define EE_CORE_MATH

#include <math.h>
#include <float.h>

#include "Math/MathForward.h"
#include "Math/MathUtils.h"

#include "Math/Fixmath/Fixmath.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/IntVector2.h"
#include "Math/IntVector3.h"
#include "Math/IntVector4.h"

#include "Math/Matrix3x3.h"
#include "Math/Matrix4x4.h"

#include "Math/Box2.h"
#include "Math/IntBox2.h"
#include "Math/Box3.h"
#include "Math/IntBox3.h"

#include "Math/Plane.h"

#endif // EE_CORE_MATH

#ifdef EE_MAKE_HASHABLE
EE_MAKE_HASHABLE( EE::Vector2f, t.x, t.y )
EE_MAKE_HASHABLE( EE::Vector3f, t.x, t.y, t.z )
EE_MAKE_HASHABLE( EE::Vector4f, t.x, t.y, t.z, t.w )
#endif // EE_CORE_MATH