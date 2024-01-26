#include<bits/stdc++.h>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <math.h>
#include <ctime>
#include <cfloat>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <iostream>
#include "raylib.h"
#include "raymath.h"

using std::vector;
using std::array;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::string;
using std::ostream;

float rand_float() {
    return (float) rand() / (float)RAND_MAX;
}

template<typename V>
struct Samples {
    vector<V> items;
    V minvec;
    V maxvec;
    public:
        size_t count() {
            return items.size();
        }

        void add(V item) {
            items.push_back(item);
        }
        void reset() {
            items.clear();
        }
        V at(size_t i) {
            return items[i];
        }
        void minmax() {
            VectorMinInit(minvec);
            VectorMaxInit(maxvec);

            for (auto pt: items) {
                VectorMinMax(pt, minvec, maxvec);
            }
        }

};

typedef Samples<Vector2> Samples2d;
typedef Samples<Vector3> Samples3d;


float VectorDistanceSqr(Vector2 v1, Vector2 v2) {
    return Vector2DistanceSqr(v1, v2);
}

float VectorDistanceSqr(Vector3 v1, Vector3 v2) {
    return Vector3DistanceSqr(v1, v2);
}

Vector2 VectorAdd(Vector2 v1, Vector2 v2) {
    return Vector2Add(v1, v2);
}

Vector3 VectorAdd(Vector3 v1, Vector3 v2) {
    return Vector3Add(v1, v2);
}

Vector2 VectorScale(Vector2 v1, float s) {
    return Vector2Scale(v1, s);
}

Vector3 VectorScale(Vector3 v1, float s) {
    return Vector3Scale(v1, s);
}


void VectorMinMax(Vector2 val, Vector2 &minvec, Vector2 &maxvec) {
    if (val.x < minvec.x) {
        minvec.x = val.x;
    }
    if (val.x >  maxvec.x) {
        maxvec.x = val.x;
    }
    if (val.y < minvec.y) {
        minvec.y = val.y;
    }
    if (val.y >  maxvec.y) {
        maxvec.y = val.y;
    }

}


void VectorMinMax(Vector3 val, Vector3 &minvec, Vector3 &maxvec) {
    if (val.x < minvec.x) {
        minvec.x = val.x;
    }
    if (val.x >  maxvec.x) {
        maxvec.x = val.x;
    }
    if (val.y < minvec.y) {
        minvec.y = val.y;
    }
    if (val.y >  maxvec.y) {
        maxvec.y = val.y;
    }
    if (val.z < minvec.z) {
        minvec.z = val.z;
    }
    if (val.z >  maxvec.z) {
        maxvec.z = val.z;
    }

}

void VectorMinInit(Vector2 &minvec) {
    minvec = Vector2 {
    .x = FLT_MAX,
    .y = FLT_MAX,
};

}

void VectorMaxInit(Vector2 &maxvec) {
    maxvec = Vector2 {
    .x = FLT_MIN,
    .y = FLT_MIN,
};
}

void VectorMinInit(Vector3 &minvec) {
    minvec = Vector3 {
    .x = FLT_MAX,
    .y = FLT_MAX,
    .z = FLT_MAX,
};

}

void VectorMaxInit(Vector3 &maxvec) {
    maxvec = Vector3 {
    .x = FLT_MIN,
    .y = FLT_MIN,
    .z = FLT_MIN,
};
}


Vector2 VectorLerp(Vector2 minvec, Vector2 maxvec) {
    return Vector2{
    .x = Lerp(minvec.x, maxvec.x, rand_float()),
    .y = Lerp(minvec.y, maxvec.y, rand_float())
    };
}

Vector3 VectorLerp(Vector3 minvec, Vector3 maxvec) {
    return Vector3{
    .x = Lerp(minvec.x, maxvec.x, rand_float()),
    .y = Lerp(minvec.y, maxvec.y, rand_float()),
    .z = Lerp(minvec.z, maxvec.z, rand_float()),
    };
}

void VectorZero(Vector2 &v) {
    v = Vector2Zero();
}

void  VectorZero(Vector3 &v) {
    v = Vector3Zero();
}

template<typename VEC>
struct Cluster {
    Samples<VEC> points;
    VEC means;

    void updateMeans(VEC minvec, VEC maxvec) {
        VEC sum;
        VectorZero(sum);
        cout << "initial sum: " << sum << " -> ";
        for (auto pt: points.items) {
            sum = VectorAdd(sum, pt);
        }

        if (points.count() > 0) {
            means = VectorScale(sum, 1.0f/points.count());

        } else {
            means = VectorLerp(minvec, maxvec);
        }
        cout << " items: " << points.count() << " inverse: " << 1.0f/points.count() << " sums: " << sum << " means: "<<means << endl;
    }

    void reset(VEC minvec, VEC maxvec) {
        points.reset();
        means=VectorLerp(minvec, maxvec);

    }
};

static array<Color, 7> colors{
RED,
BLUE,
YELLOW,
GREEN,
PURPLE,
BEIGE,
BROWN,
};


void generate_cluster2d(Vector2 center, size_t count,float radius, Samples2d *samples) {
    for (size_t i = 0; i < count; i++) {
        float angle = rand_float() * 2 * PI;
        float mag = rand_float() * radius;
        Vector2 samplePt = (Vector2){
        .x = center.x + cosf(angle) * mag,
        .y = center.y + sinf(angle) * mag,
        };
        samples->add(samplePt);

    }

}

void generate_cluster3d(Vector3 center, size_t count,float radius, Samples3d *samples) {
    for (size_t i = 0; i < count; i++) {
        float theta = rand_float() * 2 * PI;
        float phi = rand_float() * 2 * PI;
        float mag = rand_float() * radius;
        Vector3 samplePt = (Vector3){

           .x = sinf(theta) * cosf(phi)* mag,
           .y = sinf(theta) * sinf(phi)* mag,
           .z = cosf(theta) * mag,
        };
        samples->add(Vector3Add(center, samplePt));

    }

}


template <typename VEC>
void minmax(Samples<VEC> set) {

}

template <typename VEC>
static void kmeans(Samples<VEC> set, vector<Cluster<VEC>> &clusters) {
    for (Cluster<VEC> &cluster: clusters) {
        cluster.points.reset();
    }
    for (size_t i = 0; i < set.count(); i++) {
        VEC pt = set.at(i);
        float d = FLT_MAX;
        int k = -1;
        for (size_t j = 0; j < clusters.size(); j++) {
            Cluster<VEC> &cluster= clusters[j];

            VEC mj = cluster.means;
            float dj = VectorDistanceSqr(pt, mj);
            if (dj < d) {
                d = dj;
                k = j;
            }
        }

        Cluster<VEC> &cluster= clusters[k];
        //cout << "address: " << &cluster << "size: " << cluster.points.count() << endl;
        cluster.points.add(pt);
    }

    //update means
    for (Cluster<VEC> &cluster: clusters) {
        cluster.updateMeans(set.minvec, set.maxvec);
        //cout << "address: " << &cluster << "cluster points: " << cluster.points.count() << endl;
    }
    //cout << minvec << " | "<< maxvec <<endl;
}



ostream& operator<<(ostream& os,
                    const Vector3& pt)
{
    os << "{" << pt.x << "," << pt.y << "," << pt.z << "}";
    return os;
}

ostream& operator<<(ostream& os,
                    const Vector2& pt)
{
    os << "{" << pt.x << "," << pt.y << "}";
    return os;
}

template <typename S>
ostream& operator<<(ostream& os,
                    const vector<S>& vector)
{
    for (auto s: vector) {
        os << s << " ";
    }
    os << endl;
    return os;
}
