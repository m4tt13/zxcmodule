#ifndef GARRYSMOD_LUA_SOURCECOMPAT_H
#define GARRYSMOD_LUA_SOURCECOMPAT_H

#ifdef GMOD_USE_SOURCESDK

#include "math.h"
#include "angle.h"
#include "vector.h"

using QAngle = Angle;

#else
    struct Vector
    {
        Vector()
            : x( 0.f )
            , y( 0.f )
            , z( 0.f )
        {}

        Vector( const Vector& src )
            : x( src.x )
            , y( src.y )
            , z( src.z )
        {}

        Vector& operator=( const Vector& src )
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }

        float x, y, z;
    };

    using QAngle = Vector;
#endif

#endif
