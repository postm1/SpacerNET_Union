#pragma once
#include <cmath>
#include <vector>

namespace SpacerSelection {
// Keep the initial five IDs stable: they are persisted in spacer_net.ini.
enum Filter { All, WithoutMovers, WithoutWaypoints, OnlyItems, OnlyVobs,
    Points, Waypoints, Freepoints, Mobs, DynamicCollision, InteractiveMobs,
    Lights, Sounds, WithoutPfx, Invisible, WithoutDecals, WithoutDecalsAndPfx,
    Decals, Particles, Triggers, Zones, MusicZones, FilterCount };
struct Kind {
    bool editable, item, mover, moverAncestor, waypoint, freepoint;
    bool mob, dynamicCollision, interactiveMob, light, sound;
    bool pfx, invisible, decal, trigger, zone, musicZone;
    // VS 2015 cannot aggregate-initialize a type with default member initializers.
    Kind(bool canEdit, bool isItem, bool isMover, bool hasMoverAncestor, bool isWaypoint, bool isFreepoint)
        : editable(canEdit), item(isItem), mover(isMover), moverAncestor(hasMoverAncestor),
          waypoint(isWaypoint), freepoint(isFreepoint), mob(false), dynamicCollision(false),
          interactiveMob(false), light(false), sound(false), pfx(false), invisible(false),
          decal(false), trigger(false), zone(false), musicZone(false) {}
};
inline bool Matches(int filter, const Kind& k) {
    if (!k.editable) return false;
    switch (filter) {
    case All: return true;
    case WithoutMovers: return !k.mover && !k.moverAncestor;
    case WithoutWaypoints: return !k.waypoint && !k.freepoint;
    case OnlyItems: return k.item;
    case OnlyVobs: return !k.item && !k.mover;
    case Points: return k.waypoint || k.freepoint;
    case Waypoints: return k.waypoint;
    case Freepoints: return k.freepoint;
    case Mobs: return k.mob;
    case DynamicCollision: return k.dynamicCollision;
    case InteractiveMobs: return k.interactiveMob;
    case Lights: return k.light;
    case Sounds: return k.sound;
    case WithoutPfx: return !k.pfx;
    case Invisible: return k.invisible;
    case WithoutDecals: return !k.decal;
    case WithoutDecalsAndPfx: return !k.decal && !k.pfx;
    case Decals: return k.decal;
    case Particles: return k.pfx;
    case Triggers: return k.trigger;
    case Zones: return k.zone;
    case MusicZones: return k.musicZone;
    default: return false;
    }
}
inline bool ValidOffset(float x, float y, float z) {
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z)
        && std::fabs(x)<=1000000 && std::fabs(y)<=1000000 && std::fabs(z)<=1000000;
}
inline bool ValidAngles(float x, float y, float z) {
    return ValidOffset(x,y,z) && std::fabs(x)<=3600 && std::fabs(y)<=3600 && std::fabs(z)<=3600;
}
// Double precision composition avoids Euler-angle drift in continuous rotations.
struct Rotation {
    double m[3][3]={{1,0,0},{0,1,0},{0,0,1}};
    Rotation operator*(const Rotation& other)const {
        Rotation result;
        for(int i=0;i<3;++i)for(int j=0;j<3;++j) {
            result.m[i][j]=0;
            for(int k=0;k<3;++k)result.m[i][j]+=m[i][k]*other.m[k][j];
        }
        return result;
    }
    bool IsIdentity()const {
        for(int i=0;i<3;++i)for(int j=0;j<3;++j)
            if(std::fabs(m[i][j]-(i==j?1.0:0.0))>1e-10)return false;
        return true;
    }
    template<class Vector> Vector Apply(const Vector& vector)const {
        Vector result=vector;
        for(int i=0;i<3;++i) {
            double value=0;
            for(int j=0;j<3;++j)value+=m[i][j]*vector[j];
            result[i]=(float)value;
        }
        return result;
    }
    static Rotation Degrees(float x,float y,float z) {
        const double radians=3.14159265358979323846/180.0;
        Rotation rx,ry,rz;
        double c=std::cos(x*radians),s=std::sin(x*radians);
        rx.m[1][1]=c;rx.m[1][2]=-s;rx.m[2][1]=s;rx.m[2][2]=c;
        c=std::cos(y*radians);s=std::sin(y*radians);
        ry.m[0][0]=c;ry.m[0][2]=s;ry.m[2][0]=-s;ry.m[2][2]=c;
        c=std::cos(z*radians);s=std::sin(z*radians);
        rz.m[0][0]=c;rz.m[0][1]=-s;rz.m[1][0]=s;rz.m[1][1]=c;
        return rz*ry*rx; // World X, then Y, then Z.
    }
};
template<class Snapshot,class Vector,class Apply>
void ApplyTransform(const std::vector<Snapshot>& before,const Vector& delta,
    const Rotation& rotation,const Vector& pivot,Apply apply) {
    // Parent-first snapshots include unselected descendants, which stay fixed in world space.
    for(const auto& entry:before) {
        Vector target=entry.position;
        if(entry.selected)target=pivot+rotation.Apply(entry.position-pivot)+delta;
        apply(entry,target);
    }
}
}
