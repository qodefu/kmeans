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

#define K 5
#define SAMPLE_RADIUS 2.50f
#define MEAN_RADIUS SAMPLE_RADIUS * 5
//#define MIN_X (float)-20.0
//#define MAX_X (float) 20.0
//#define MIN_Y (float) -20.0
//#define MAX_Y (float) 20.0

using std::vector;
using std::array;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::string;
using std::ostream;


static float MIN_X;
static float MIN_Y;
static float MAX_X;
static float MAX_Y;

struct Samples {
    vector<Vector2> items;
    public:
        size_t count() {
            return items.size();
        }

        void add(Vector2 item) {
            items.push_back(item);
        }
        void reset() {
            items.clear();
        }
        Vector2 at(size_t i) {
            return items[i];
        }
};

static Samples set;
static array<Samples,K> clusters;
static array<Vector2,K> means;
static array<Color, 7> colors{
RED,
BLUE,
YELLOW,
GREEN,
PURPLE,
BEIGE,
BROWN,
};

float rand_float();
void reset_scale() {
    MIN_X = FLT_MAX;
    MAX_X = FLT_MIN;
    MIN_Y = FLT_MAX;
    MAX_Y = FLT_MIN;

    for (auto samplePt: set.items) {

        if (samplePt.x < MIN_X) {
            MIN_X = samplePt.x;
        }
        if (samplePt.x > MAX_X) {
            MAX_X = samplePt.x;
        }
        if (samplePt.y < MIN_Y) {
            MIN_Y = samplePt.y;
        }
        if (samplePt.y > MAX_Y) {
            MAX_Y = samplePt.y;
        }
    }

    for (size_t i =0; i < K; i++) {
        means[i].x = Lerp(MIN_X, MAX_X, rand_float());
        means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
        clusters[i].reset();
    }
}
static Vector2 project_sample_to_screen(Vector2 sample) {

    float w = GetScreenWidth();
    float h = GetScreenHeight();
    return CLITERAL(Vector2) {
    .x = (sample.x - MIN_X)/(MAX_X - MIN_X)*w,
    .y = h - (sample.y - MIN_Y)/(MAX_Y - MIN_Y)*h,
    };

}


float rand_float() {
    return (float) rand() / (float)RAND_MAX;
}


static void generate_cluster(Vector2 center, size_t count,float radius, Samples *samples) {
    for (size_t i = 0; i < count; i++) {
        float angle = rand_float() * 2 * PI;
        float mag = rand_float();
        Vector2 samplePt = (Vector2){
        .x = center.x + cosf(angle) * mag * radius,
        .y = center.y + sinf(angle) * mag * radius,
        };
        samples->add(samplePt);


    }

}


void generate_samples() {
    set.reset();
    generate_cluster((Vector2){0,0}, 200, 10, &set);
    generate_cluster((Vector2){20*0.5f, 20*0.5f}, 200, 5, &set);
    generate_cluster((Vector2){-20*0.5f,20*0.5f}, 200, 5, &set);

}

static void kmeans() {
    Vector2 sums[K] = {{0,0}};
    for (size_t i =0; i < K; i++) {
        clusters[i].reset();
    }
    for (size_t i = 0; i < set.count(); i++) {
        Vector2 pt = set.at(i);
        float d = FLT_MAX;
        int k = -1;
        for (size_t j = 0; j < means.size(); j++) {
            Vector2 mj = means[j];
            float dj = Vector2DistanceSqr(pt, mj);
            if (dj < d) {
                d = dj;
                k = j;
//                means[j] = Vector2Add(Vector2Scale(means[j], j), pt);
            }
        }
        clusters[k].add(pt);
        sums[k] = Vector2Add(sums[k], pt);
    }
    for (size_t i = 0; i < K; i++) {
        if (clusters[i].count() > 0) {
            means[i].x = sums[i].x / clusters[i].count();
            means[i].y = sums[i].y / clusters[i].count();
        } else {

            means[i].x = Lerp(MIN_X, MAX_X, rand_float());
            means[i].y = Lerp(MIN_Y, MAX_Y, rand_float());
        }
        cout << "("<< means[i].x << ", "<< means[i].y << ")"<< endl;
    }
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

vector<vector<string>> readcsv(string fname) {
    fstream file(fname, ios::in);
    string line, word;
    vector<vector<string>> content;
    vector<string> row;
    if (file.is_open()) {
        while(std::getline(file, line)) {
            row.clear();
            for (auto c: line) {
                if (c == ',') {
                    row.push_back(word);
                    word = "";

                } else {
                    word += c;
                }
            }
            row.push_back(word);
            word="";
            content.push_back(row);
        }
    }


    return content;
}


enum Leaf_Attrs {
LEAF_CLASS=0,
LEAF_SPECIMEN_NUMBER,
LEAF_ECCENTRICITY,
LEAF_ASPECT_RATIO,
LEAF_ELONGATION,
LEAF_SOLIDITY,
LEAF_STOCHASTIC_CONVEXITY,
LEAF_ISOPERIMETRIC_FACTOR,
LEAF_MAXIMAL_INDENTATION_DEPTH,
LEAF_LOBEDNESS,
LEAF_AVERAGE_INTENSITY,
LEAF_AVERAGE_CONTRAST,
LEAF_SMOOTHNESS,
LEAF_THIRD_MOMENT,
LEAF_UNIFORMITY,
LEAF_ENTROPY,
};

void leaf_samples() {
    vector<vector<string>> content = readcsv("./leaf.csv");
    set.reset();

    for (auto r: content) {
        float x,y;
        for (size_t i = 0; i < r.size(); i++) {
            //cout << "i: " << i << "-> " << r[i] << endl;
            switch(i) {
                case LEAF_ASPECT_RATIO:
                    x = std::stof(r[i]);
                    break;
                case LEAF_ENTROPY:
                    y = std::stof(r[i]);
                    break;
            }
        }
        set.add(Vector2{x,y});

    }
    cout << set.items << endl;
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800 ,600, "k-means");
    SetTargetFPS(60);



    leaf_samples();
    reset_scale();
//    generate_samples();
    kmeans();
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        if(IsKeyPressed(KEY_R)) {
            generate_samples();
            reset_scale();
            kmeans();
        } else if (IsKeyPressed(KEY_SPACE)) {
            kmeans();

        }
        //Vector2 sample = {1, 1};
        //project_sample_to_screen(sample);
        //DrawCircleV(project_sample_to_screen(sample), 10, RED);

        for (size_t i = 0; i < K; i++) {

            Samples group = clusters[i];
            Color color = colors[i % colors.size()];
            for (size_t j = 0; j < group.count(); j++) {
                Vector2 it = project_sample_to_screen(group.at(j));
                DrawCircleV(it, SAMPLE_RADIUS, color);
            }
            Vector2 it = project_sample_to_screen(means[i]);
            DrawCircleV(it, MEAN_RADIUS, WHITE);
        }

        //DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2,10, RED);
        EndDrawing();
    }




    //printf("hello world");
    return 0;
}
