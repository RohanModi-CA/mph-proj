#include "raylib.h"

// Types and Structures Definition
typedef enum GameScreen { LOGO = 0, TITLE, SIM1, SIM2, ABOUT_US, ENDING } GameScreen;

// Function to check if a button is clicked
bool IsButtonClicked(Rectangle button) {
    return CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - interactive buttons");

    GameScreen currentScreen = LOGO;
    int framesCounter = 0; // Frame counter

    // Load image
    Texture2D relativityImage = LoadTexture("resources/picture.png");

    SetTargetFPS(60); // Set desired framerate

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        switch (currentScreen)
        {
            case LOGO:
                framesCounter++;
                if (framesCounter > 120) {
                    currentScreen = TITLE;
                }
                break;

            case TITLE:
            {
                Rectangle sim1Button = {screenWidth / 2 - 120, screenHeight / 2 + 20, 240, 50};
                Rectangle sim2Button = {screenWidth / 2 - 120, screenHeight / 2 + 80, 240, 50};
                Rectangle aboutUsButton = {screenWidth - 110, screenHeight - 40, 100, 30};

                if (IsButtonClicked(sim1Button)) {
                    currentScreen = SIM1;
                }
                if (IsButtonClicked(sim2Button)) {
                    currentScreen = SIM2;
                }
                if (IsButtonClicked(aboutUsButton)) {
                    currentScreen = ABOUT_US;
                }
            } break;

            case SIM1:
            case SIM2:
            case ABOUT_US:
            {
                Rectangle backButton = {screenWidth / 2 - 50, screenHeight - 60, 100, 40};
                if (IsButtonClicked(backButton)) {
                    currentScreen = TITLE;
                }
            } break;

            case ENDING:
                // Other screens
                break;

            default
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen)
        {
            case LOGO:
                DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                DrawText("WAIT for 2 SECONDS...", 290, 220, 20, GRAY);
                break;

            case TITLE:
            {
                // Draw background image
                Rectangle sourceRec = { 0.0f, 0.0f, (float)relativityImage.width, (float)relativityImage.height };
                Rectangle destRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };
                Vector2 origin = { 0.0f, 0.0f };

                DrawTexturePro(relativityImage, sourceRec, destRec, origin, 0.0f, WHITE);

                // Draw title and buttons
                DrawText("SPECIAL RELATIVITY", 20, 20, 40, WHITE);
                DrawText("Pick One!", screenWidth / 2 - 60, screenHeight / 2 - 40, 20, WHITE);

                Rectangle sim1Button = {screenWidth / 2 - 120, screenHeight / 2 + 20, 240, 50};
                Rectangle sim2Button = {screenWidth / 2 - 120, screenHeight / 2 + 80, 240, 50};
                Rectangle aboutUsButton = {screenWidth - 110, screenHeight - 40, 100, 30};

                DrawRectangleRounded(sim1Button, 0.3f, 10, WHITE);
                DrawRectangleRounded(sim2Button, 0.3f, 10, WHITE);
                DrawRectangleRounded(aboutUsButton, 0.5f, 10, WHITE);

                DrawText("SIMULATION 1", sim1Button.x + 45, sim1Button.y + 15, 20, BLACK);
                DrawText("SIMULATION 2", sim2Button.x + 45, sim2Button.y + 15, 20, BLACK);
                DrawText("ABOUT US!", aboutUsButton.x + 10, aboutUsButton.y + 5, 15, BLACK);
            } break;

            case SIM1:
                DrawText("SIMULATION 1 SCREEN", 20, 100, 40, DARKGREEN);
                break;

            case SIM2:
                DrawText("SIMULATION 2 SCREEN", 20, 100, 40, DARKBLUE);
                break;

            case ABOUT_US:
                DrawText("ABOUT THE PROJECT", 20, 50, 30, DARKGRAY);
                DrawText("This project is a simple interactive demonstration of", 20, 100, 20, GRAY);
                DrawText("special relativity concepts using raylib.", 20, 130, 20, GRAY);
                DrawText("Created by Rohan Modi and Olivia Choi.", 20, 180, 20, GRAY);
                break;

            default:
                break;
        }

        // Draw back button for screens with it
        if (currentScreen == SIM1 || currentScreen == SIM2 || currentScreen == ABOUT_US) {
            Rectangle backButton = {screenWidth / 2 - 50, screenHeight - 60, 100, 40};
            DrawRectangleRounded(backButton, 0.3f, 10, WHITE);
            DrawText("BACK", backButton.x + 20, backButton.y + 10, 20, BLACK);
        }

        EndDrawing();
    }

    // Unload image
    UnloadTexture(relativityImage);

    CloseWindow(); // Close window and OpenGL context
    return 0;
}
