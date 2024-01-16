#include <bits/stdc++.h>
#include <raylib.h>
#include <raymath.h>

using namespace std;
#define CUBE_SIZE 2.0f
#define CUBE_COLOR RED
#define CAMERA_SPEED CUBE_SIZE

static float dt = 0;
///void cam_step(Camera &cam, bool backward) {
///    Vector3 dir = Vector3Subtract(cam.target, cam.position);
///    float move = backward ? -1.0f: 1.0f;
///    cam.position = Vector3Add(cam.position, Vector3Scale(dir, move*CAMERA_SPEED*dt));
///}


ostream& operator<<(ostream& os,
                    const Vector3& pt)
{
    os << "{" << pt.x << "," << pt.y << "," << pt.z << "}";
    return os;
}


int main() {
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

    while(!WindowShouldClose()) {
        dt = GetFrameTime();
        float wheel = GetMouseWheelMove();
        // cout << "mouse moved: " << wheel << endl;
        camera_mag += camera_mag_vel * dt;
        camera_mag = Clamp(camera_mag, CUBE_SIZE, CUBE_SIZE*5);
        camera_mag_vel += wheel * CUBE_SIZE;
        camera_mag_vel *= 0.9;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 delta = GetMouseDelta();
            camera_theta -= dt * delta.x;

        }
        if (IsKeyPressed(KEY_W)) {
        }
        if (IsKeyPressed(KEY_S)) {
        }
        BeginDrawing();
        ClearBackground(GetColor(0x181818AA));
        BeginMode3D(camera);
        camera.position = Vector3 {
           .x = sinf(camera_theta) * cosf(camera_phi)* camera_mag,
           .y = sinf(camera_theta) * sinf(camera_phi)* camera_mag,
           .z = cosf(camera_theta) * camera_mag,
        };


        DrawCube(Vector3Zero(), CUBE_SIZE,CUBE_SIZE,CUBE_SIZE,RED);

        EndMode3D();
        EndDrawing();
    }
    cout << "done" << endl;
    CloseWindow();
    return 0;

}
