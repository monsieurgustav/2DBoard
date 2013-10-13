//
//  Helpers.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 06/10/13.
//
//

#ifndef Labyrinth_Helpers_h
#define Labyrinth_Helpers_h


namespace cinder {
    template <class T> class Vec2;
    template <class T> class Vec3;
    template <class T> class Vec4;
}
namespace ci = cinder;


template <class T>
T reduce_max(ci::Vec2<T> v)
{
    return std::max(v.x, v.y);
}

template <class T>
T reduce_min(ci::Vec2<T> v)
{
    return std::min(v.x, v.y);
}


template <class T>
T reduce_max(ci::Vec3<T> v)
{
    return std::max(reduce_max(v.xy()), v.z);
}

template <class T>
T reduce_min(ci::Vec3<T> v)
{
    return std::min(reduce_min(v.xy()), v.z);
}


template <class T>
T reduce_max(ci::Vec4<T> v)
{
    return std::max(reduce_max(v.xyz()), v.z);
}

template <class T>
T reduce_min(ci::Vec4<T> v)
{
    return std::min(reduce_min(v.xyz()), v.z);
}

#endif
