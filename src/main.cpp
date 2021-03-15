#include <raylib.h>

int main() {
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(800, 600, "sim");

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("simulation", 50, 50, 20, YELLOW);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}