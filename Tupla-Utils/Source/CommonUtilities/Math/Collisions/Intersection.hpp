#pragma once

#include "../Geometry/AABB3D.hpp"
#include "../Geometry/Ray.hpp"
#include "../Geometry/Plane.hpp"
#include "../Geometry/Sphere.hpp"
#include "CommonUtilities/Math/Math.hpp"

#define CLAMP(a, b, c) ((a) < (b) ? (b) : (c) < (a) ? (c) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace CommonUtilities
{
    template <typename T>
    struct IntersectionData
    {
        T TimeToEnter = std::numeric_limits<T>::infinity();
        T TimeToExit = 0;
        bool Weak = false;
        Vector3<T> EntryPoint;
        Vector3<T> ExitPoint;
        Vector3<T> IntersectionNormal;
    };
    
    // If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
    // the ray is in the plane, true is returned, if not, false is returned. If the ray
    // isn't parallel to the plane and hits, the intersection point is stored in
    // aOutIntersectionPoint and true returned otherwise false is returned and
    // outIntersectionPoint is unchanged.
    template <typename T>
    bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, IntersectionData<T>* aOutData = nullptr)
    {
        Vector3<T> planeNormal = aPlane.GetNormal();
        Vector3<T> planePosition = aPlane.GetPoint();
        
        Vector3<T> rayDirection = aRay.GetDirection();
        Vector3<T> rayOrigin = aRay.GetOrigin();

        T denominator = planeNormal.Dot(rayDirection);

        if(abs(denominator) > 0.0001f)
        {
            T t = (planePosition - rayOrigin).Dot(planeNormal) / denominator;

            if (t >= 0) {
                aOutData.EntryPoint = rayOrigin + rayDirection * t;
                aOutData.TimeToEnter = t;
                return true;
            }
            return t >= 0;
        }

        if ((planePosition - rayOrigin).Dot(planeNormal) == 0) { 
            aOutData.EntryPoint = rayOrigin;
            aOutData.TimeToEnter = 0.0f;
            return true;
        };
        
        return false;
    }

    // If no collision, aOutIntersectionPoint remains unchanged.
    // If The sphere overlaps the AABB true is returned, if not, false is returned.
    // The point on the AABB closest to the sphere centre is saved in
    // aOutIntersectionPoint.
    // A sphere touching the aabb is considered overlapping.
    template <typename T>
    bool IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D, Vector3<T>& outIntersectionPoint)
    {
        const Vector3<T>& sphereCenter = aSphere.GetCenter();
        const Vector3<T>& aabbMin = aAABB3D.GetMin();
        const Vector3<T>& aabbMax = aAABB3D.GetMax();
        
        T x = CLAMP(sphereCenter.x, aabbMin.x, aabbMax.x);
        T y = CLAMP(sphereCenter.y, aabbMin.y, aabbMax.y);
        T z = CLAMP(sphereCenter.z, aabbMin.z, aabbMax.z);

        Vector3<T> closestPoint = Vector3<T>(x, y, z);

        if(aSphere.IsInside(closestPoint))
        {
            outIntersectionPoint = closestPoint;
            return true;
        }

        return false;
    }

    // If the ray intersects the AABB, true is returned, if not, false is returned.
    // A ray in one of the AABB's sides is counted as intersecting it.
    template <typename T>
    bool IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay, IntersectionData<T>* aData = nullptr)
    {
        T tmin = 0;
        T tmax = std::numeric_limits<T>::infinity();

        Vector3<T> aabbMin = aAABB.GetMin();
        Vector3<T> aabbMax = aAABB.GetMax();

        Vector3<T> t1 = (aabbMin - aRay.GetOrigin()) / aRay.GetDirection();
        Vector3<T> t2 = (aabbMax - aRay.GetOrigin()) / aRay.GetDirection();

        tmin = MIN(MAX(t1.x, tmin), MAX(t2.x, tmin));
        tmin = MIN(MAX(t1.y, tmin), MAX(t2.y, tmin));
        tmin = MIN(MAX(t1.z, tmin), MAX(t2.z, tmin));
        
        tmax = MAX(MIN(t1.x, tmax), MIN(t2.x, tmax));
        tmax = MAX(MIN(t1.y, tmax), MIN(t2.y, tmax));
        tmax = MAX(MIN(t1.z, tmax), MIN(t2.z, tmax));

        if(aData)
        {
            aData->TimeToEnter = tmin;
            aData->TimeToExit = tmax;
            aData->EntryPoint = aRay.GetOrigin() + aRay.GetDirection() * tmin;
            aData->ExitPoint = aRay.GetOrigin() + aRay.GetDirection() * tmax;
            
            const auto delta = (aData->EntryPoint - aAABB.GetPosition()) / aAABB.GetHalf();
            const T aX = Math::Abs(delta.x);
            const T aY = Math::Abs(delta.y);
            const T aZ = Math::Abs(delta.z);

            if(aX > MAX(aY, aZ))
            {
                aData->IntersectionNormal = { Math::Sign(delta.x), 0.0f, 0.0f };
            }
            else if(aY > MAX(aX, aZ))
            {
                aData->IntersectionNormal = { 0.0f, Math::Sign(delta.y), 0.0f };
            }
            else if(aZ > MAX(aX, aY))
            {
                aData->IntersectionNormal = { 0.0f, 0.0f, Math::Sign(delta.z) };
            }
            else
            {
                T maxPoint = MAX(MAX(aX, aY), aZ);

                aData->IntersectionNormal = {
                    aX == maxPoint ? Math::Sign(delta.x) : T(0),
                    aY == maxPoint ? Math::Sign(delta.y) : T(0),
                    aZ == maxPoint ? Math::Sign(delta.z) : T(0)
                };
            
                aData->IntersectionNormal.Normalize();
                aData->Weak = true;
            }
        }
        
        
        return tmin <= tmax;
    }

    // If the ray intersects the sphere, true is returned, if not, false is returned.
    // A ray intersecting the surface of the sphere is considered as intersecting it.
    template <typename T>
    bool IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, IntersectionData<T>* aData = nullptr)
    {
        Vector3<T> spherePosDelta = aSphere.GetCenter() - aRay.GetOrigin();
        const T dMiddle = aRay.GetDirection().GetNormalized().Dot(spherePosDelta);
        const T radius = aSphere.GetRadius();
        const T discriminant = radius * radius - spherePosDelta.LengthSqr() + dMiddle * dMiddle;

        if(aData)
        {
            const T timeEnter = dMiddle - sqrt(discriminant);
            const T timeExit = dMiddle + sqrt(discriminant);
            //const T tDelta = sqrt(radius * radius - discriminant); Self cooked
            aData->TimeToEnter = timeEnter;
            aData->TimeToExit = timeEnter;
            aData->EntryPoint = aRay.GetDirection() * timeEnter + aRay.GetOrigin();
            aData->ExitPoint = aRay.GetDirection() * timeExit + aRay.GetOrigin();
            aData->IntersectionNormal = (aRay.GetDirection() * timeEnter - spherePosDelta).GetNormalized();
        }
        
        return discriminant >= 0 && dMiddle >= sqrt(discriminant);
    }
}