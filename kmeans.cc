#include "common.cc"
#define K 5
#define SAMPLE_RADIUS 2.50f
#define MEAN_RADIUS SAMPLE_RADIUS * 5
//#define MIN_X (float)-20.0
//#define MAX_X (float) 20.0
//#define MIN_Y (float) -20.0
//#define MAX_Y (float) 20.0



//static Vector2 MIN_VEC, MAX_VEC;
static Samples2d set;
static vector<Cluster<Vector2>> clusters;

void reset_scale() {
    set.minmax();
    for (Cluster<Vector2> &cluster: clusters ) {
        cluster.reset(set.minvec, set.maxvec);
    }
}
static Vector2 project_sample_to_screen(Vector2 sample) {

    float w = GetScreenWidth();
    float h = GetScreenHeight();
    return CLITERAL(Vector2) {
    .x = (sample.x - set.minvec.x)/(set.maxvec.x - set.minvec.x)*w,
    .y = h - (sample.y - set.minvec.y)/(set.maxvec.y - set.minvec.y)*h,
    };

}


void generate_samples() {
    set.reset();
    generate_cluster2d((Vector2){0,0}, 200, 10, &set);
    generate_cluster2d((Vector2){20*0.5f, 20*0.5f}, 200, 5, &set);
    generate_cluster2d((Vector2){-20*0.5f,20*0.5f}, 200, 5, &set);

}

//static void kmeans() {
//    Vector2 sums[K] = {{0,0}};
//    for (size_t i =0; i < K; i++) {
//        clusters[i].points.reset();
//    }
//    for (size_t i = 0; i < set.count(); i++) {
//        Vector2 pt = set.at(i);
//        float d = FLT_MAX;
//        int k = -1;
//        for (size_t j = 0; j < clusters.size(); j++) {
//            Vector2 mj = clusters[j].means;
//            float dj = Vector2DistanceSqr(pt, mj);
//            if (dj < d) {
//                d = dj;
//                k = j;
////                means[j] = Vector2Add(Vector2Scale(means[j], j), pt);
//            }
//        }
//        clusters[k].points.add(pt);
//        sums[k] = Vector2Add(sums[k], pt);
//    }
//    for (size_t i = 0; i < K; i++) {
//        if (clusters[i].points.count() > 0) {
//            clusters[i].means.x = sums[i].x / clusters[i].points.count();
//            clusters[i].means.y = sums[i].y / clusters[i].points.count();
//        } else {
//
//            clusters[i].means.x = Lerp(MIN_X, MAX_X, rand_float());
//            clusters[i].means.y = Lerp(MIN_Y, MAX_Y, rand_float());
//        }
//        cout << "("<< clusters[i].means.x << ", "<< clusters[i].means.y << ")"<< endl;
//
//    }
//}



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

    for (int i = 0; i < K; i++) {
        clusters.push_back(Cluster<Vector2>());
    }

    kmeans<Vector2>(set, clusters);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        if(IsKeyPressed(KEY_R)) {
            generate_samples();
            reset_scale();
            kmeans(set, clusters);
        } else if (IsKeyPressed(KEY_SPACE)) {
            kmeans(set, clusters);

        }
        //Vector2 sample = {1, 1};
        //project_sample_to_screen(sample);
        //DrawCircleV(project_sample_to_screen(sample), 10, RED);

        for (size_t i = 0; i < K; i++) {
            Cluster<Vector2> &cluster = clusters[i];

            Samples2d &group = cluster.points;
            Color color = colors[i % colors.size()];
            for (size_t j = 0; j < group.count(); j++) {
                Vector2 it = project_sample_to_screen(group.at(j));
                DrawCircleV(it, SAMPLE_RADIUS, color);
            }
            Vector2 it = project_sample_to_screen(cluster.means);
            DrawCircleV(it, MEAN_RADIUS, WHITE);
        }

        //DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2,10, RED);
        EndDrawing();
    }




    //printf("hello world");
    return 0;
}
