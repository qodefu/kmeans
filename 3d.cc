#include "common.cc"
#include "raylib.h"
#include "raymath.h"
#include <cstdint>
#include <unordered_map>

#define CUBE_SIZE 2.0f
#define CUBE_COLOR RED
#define CAMERA_SPEED CUBE_SIZE

#define CLUSTER_RADIUS 1.0f

#define SAMPLE_RADIUS CLUSTER_RADIUS /120.0f
#define MEAN_RADIUS SAMPLE_RADIUS * 2
#define K 16

static float dt = 0;
///void cam_step(Camera &cam, bool backward) {
///    Vector3 dir = Vector3Subtract(cam.target, cam.position);
///    float move = backward ? -1.0f: 1.0f;
///    cam.position = Vector3Add(cam.position, Vector3Scale(dir, move*CAMERA_SPEED*dt));
///}

//static array<Cluster<Vector3>, K> clusters;
//static Samples3d set;



static uint32_t ColorToUint32(Color c) {
    uint32_t* i = reinterpret_cast<uint32_t*>(&c);
    return *i;
}

static Vector3 ColorToVector3(Color c) {
    return {
          .x = float(c.r)/255.0f*CLUSTER_RADIUS,
          .y = float(c.g)/255.0f*CLUSTER_RADIUS,
          .z = float(c.b)/255.0f*CLUSTER_RADIUS,
        };

}

static Color Vector3ToColor(Vector3 v) {
    return {
     .r = (unsigned char)(v.x/CLUSTER_RADIUS*255.0f),
     .g = (unsigned char)(v.y/CLUSTER_RADIUS*255.0f),
     .b = (unsigned char)(v.z/CLUSTER_RADIUS*255.0f),
     .a = 255,
    };

}

void generateSample(Samples3d &samples) {

    generate_cluster3d(Vector3Zero(), 200, CLUSTER_RADIUS, &samples);
    generate_cluster3d(Vector3{-CLUSTER_RADIUS, CLUSTER_RADIUS, 0}, 80, CLUSTER_RADIUS/2 , &samples);
    generate_cluster3d(Vector3{CLUSTER_RADIUS, CLUSTER_RADIUS, 0}, 80, CLUSTER_RADIUS/2 , &samples);
}

int cluster_of_color(Color c, vector<Cluster<Vector3>> &clusters) {
    Vector3 pixel = ColorToVector3(c);
    float s = FLT_MAX;
    int k = -1;
    for (int i = 0; i < clusters.size(); i++) {
        float d = Vector3Distance(clusters[i].means, pixel);
        if (d < s) {
            k = i;
            s = d;
        }
    }
    return k;
}

int main() {

#define IMAGE
#ifdef  IMAGE
    string lenaPath = "lena.png";
    Image image = LoadImage(lenaPath.c_str());
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);       // 24 bpp
    size_t colorCount = image.width * image.height;
    Color *color_points = static_cast<Color*>(image.data);

    std::unordered_map<uint32_t, Color> colorMap;
    for (size_t i=0; i < colorCount; i++) {
        if (colorMap.find(ColorToInt(color_points[i])) == colorMap.end()) {
            colorMap[ColorToInt(color_points[i])] = color_points[i];
        }
    }
    cout << "file: " << lenaPath << " with unique colors: " << colorMap.size() << endl;


#endif


    Camera3D camera = {
      .position=Vector3{0, 0, CUBE_SIZE},
      .target=Vector3{0,0,0},
      .up={0,1,0},
      .fovy = 90,
      .projection = CAMERA_PERSPECTIVE
    };

    float camera_mag = CUBE_SIZE;
    float camera_mag_vel = 0;
    float camera_theta = 0.0f;
    float camera_phi = 0.0f;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "3d kmeans");
    SetTargetFPS(60);
    cout << camera.position << "" << endl;
    Samples3d samples;
#ifndef IMAGE
    generateSample(samples);
#else

    for (auto i = colorMap.begin(); i != colorMap.end(); i++) {
//        cout << std::hex << i->first << endl;
        Vector3 item = ColorToVector3(i->second);
        samples.add(item);
    }
#endif

    samples.minmax();

    vector<Cluster<Vector3>> clusters;
    for (int i = 0; i < K ; i++) {
        clusters.push_back(Cluster<Vector3>());
    }
    kmeans<Vector3> (samples, clusters);

    while(!WindowShouldClose()) {
        dt = GetFrameTime();
        float wheel = GetMouseWheelMove();
        // cout << "mouse moved: " << wheel << endl;
        camera_mag += camera_mag_vel * dt;
        camera_mag = Clamp(camera_mag, CUBE_SIZE/5, CUBE_SIZE*5);
        camera_mag_vel += wheel * CUBE_SIZE;
        camera_mag_vel *= 0.9;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            camera_theta -= dt * delta.x;
            camera_phi -= dt * delta.y;

        }
        if (IsKeyPressed(KEY_S)) {
            //save image
            for (size_t i=0; i < colorCount; i++) {
                int k = cluster_of_color(color_points[i], clusters);
                Color meanColor = Vector3ToColor(clusters[k].means);
                color_points[i] = meanColor;
            }
            ExportImage(image, "output.png");
        }
        if (IsKeyPressed(KEY_R)) {
            samples.reset();
            for (Cluster<Vector3> &cluster: clusters) {
                cluster.reset(samples.minvec, samples.maxvec );
            }
            generate_cluster3d(Vector3Zero(), 200, CLUSTER_RADIUS, &samples);
            generate_cluster3d(Vector3{-CLUSTER_RADIUS, CLUSTER_RADIUS, 0}, 80, CLUSTER_RADIUS/2 , &samples);
            generate_cluster3d(Vector3{CLUSTER_RADIUS, CLUSTER_RADIUS, 0}, 80, CLUSTER_RADIUS/2 , &samples);
            samples.minmax();
            kmeans<Vector3> (samples, clusters);

        }
        if (IsKeyPressed(KEY_SPACE)) {
            kmeans<Vector3> (samples, clusters);
        }

        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        BeginMode3D(camera);
        camera.position = Vector3 {
           .x = sinf(camera_theta) * cosf(camera_phi)* camera_mag,
           .y = sinf(camera_theta) * sinf(camera_phi)* camera_mag,
           .z = cosf(camera_theta) * camera_mag,
        };


        for (int i =0; i < clusters.size(); i++) {
            Cluster<Vector3> &cluster = clusters[i];

            Color c = Vector3ToColor(cluster.means);
            for (auto &pt: cluster.points.items) {

                DrawCube(pt, SAMPLE_RADIUS, SAMPLE_RADIUS, SAMPLE_RADIUS, c);
            }
            DrawCube(cluster.means, MEAN_RADIUS, MEAN_RADIUS, MEAN_RADIUS, WHITE);


        }

        EndMode3D();
        EndDrawing();
    }
    cout << "done" << endl;
    CloseWindow();
    return 0;

}
