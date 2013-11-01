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
ci::Vec2<T> abs(ci::Vec2<T> v)
{
    return ci::Vec2<T>(std::abs(v.x), std::abs(v.y));
}

template <class T>
ci::Vec2<T> min(ci::Vec2<T> left, ci::Vec2<T> right)
{
    return ci::Vec2<T>(std::min(left.x, right.x),
                       std::min(left.y, right.y));
}

template <class T>
ci::Vec2<T> max(ci::Vec2<T> left, ci::Vec2<T> right)
{
    return ci::Vec2<T>(std::max(left.x, right.x),
                       std::max(left.y, right.y));
}

//------------------------------------------------------------------------------

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
ci::Vec3<T> abs(ci::Vec3<T> v)
{
    return ci::Vec3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template <class T>
ci::Vec3<T> min(ci::Vec3<T> left, ci::Vec3<T> right)
{
    return ci::Vec3<T>(std::min(left.x, right.x),
                       std::min(left.y, right.y),
                       std::min(left.z, right.z));
}

template <class T>
ci::Vec3<T> max(ci::Vec3<T> left, ci::Vec3<T> right)
{
    return ci::Vec3<T>(std::max(left.x, right.x),
                       std::max(left.y, right.y),
                       std::max(left.z, right.z));
}

//------------------------------------------------------------------------------

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

template <class T>
ci::Vec4<T> abs(ci::Vec4<T> v)
{
    return ci::Vec4<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
}

template <class T>
ci::Vec4<T> min(ci::Vec4<T> left, ci::Vec4<T> right)
{
    return ci::Vec4<T>(std::min(left.x, right.x),
                       std::min(left.y, right.y),
                       std::min(left.z, right.z),
                       std::min(left.w, right.w));
}

template <class T>
ci::Vec4<T> max(ci::Vec4<T> left, ci::Vec4<T> right)
{
    return ci::Vec4<T>(std::max(left.x, right.x),
                       std::max(left.y, right.y),
                       std::max(left.z, right.z),
                       std::max(left.w, right.w));
}

#endif
