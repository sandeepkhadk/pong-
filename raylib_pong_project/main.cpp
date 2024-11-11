#include <iostream>
#include <string.h>
#include<raylib.h>
#include<cmath>
#include<vector>
#include<algorithm>
using namespace std;
// Color definitions
Color Background_Color = Color{37, 164, 137, 255};
Color Paddle_Color = Color{255, 255, 255, 255};  // Bright White
Color Ball_Color = Color{255, 223, 0, 255};      // Bright Yellow

// Game screen size
const int screen_width = 1280;
const int screen_height =800;
const char* player1_won = "PLAYER 1 WON";
const char* player2_won = "PLAYER 2 WON";
const char* roshan  = "Developed by Sandeep, Roshan, Rojin, Santosh";
// Game states
typedef enum gamestate {
    menu,
    multiplayer_menu,
    input_name,
    select_level,
    start_game,
    playing,
    paused,
    won
} gamestate;
gamestate state = menu;

// Multiplayer mode
typedef enum multiplayerMode {
    single_player,
    player_vs_player
} multiplayerMode;
multiplayerMode currentMode = single_player;

// Difficulty levels
typedef enum difficulty {
    easy,
    medium,
    hard
} difficulty;
difficulty currentDifficulty = easy;

// Scores
int player_score = 0;
int cpu_score = 0;
int winner = 0;

// Player names and input management
char player1_name[10] = "";
char player2_name[10] = "";
int letterCount1 = 0;
int letterCount2 = 0;
bool enterName1 = true;
bool enterName2 = false;

// Ball class
class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, Ball_Color);
    }

    void Update() {
        Sound loss = LoadSound("points2.mp3");
        Sound gameover = LoadSound("gameover.mp3");

        x += speed_x;
        y += speed_y;

        if (y + radius >= screen_height || y - radius <= 0) {
            speed_y *= -1;
        }
        if (x + radius >= screen_width) {
            cpu_score++;
            if (cpu_score < 10) {
                PlaySound(loss);
            }
            ResetBall();
        }

        if (x - radius <= 10) {
            player_score++;
            if (player_score < 10) {
                PlaySound(loss);
            }
            ResetBall();
        }

        if (cpu_score == 10) {
            PlaySound(gameover);
            state = won;
            winner = 2;
        }

        if (player_score == 10) {
            PlaySound(gameover);
            state = won;
            winner = 1;
        }
    }
    void ResetBall() {
    x = screen_width / 2;
    y = screen_height / 2;

    int speed_factor = (currentDifficulty == easy) ? 8 : (currentDifficulty == medium) ? 12 : 16;
    speed_x = speed_factor * ((GetRandomValue(0, 1) == 0) ? -1 : 1);
    speed_y = speed_factor * ((GetRandomValue(0, 1) == 0) ? -1 : 1);
}

    
};
// Paddle class
class Paddle {
public:
    float x, y;
    float width, height;
    int speed;
    Color color;

    void Draw() {
        DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.8f, 0, color);
    }

    void Update(int player, float deltaTime) {
        float velocity = speed * deltaTime*25;
        Color defaultColor = color;

        if (player == 1) {
            if (IsKeyDown(KEY_UP)) {
                y -= velocity;
                color = RED;
            } else if (IsKeyDown(KEY_DOWN)) {
                y += velocity;
                color = RED;
            } else {
                color = defaultColor;
            }
        } else {
            if (IsKeyDown(KEY_W)) {
                y -= velocity;
                color = BLUE;
            } else if (IsKeyDown(KEY_S)) {
                y += velocity;
                color = BLUE;
            } else {
                color = defaultColor;
            }
        }
        LimitMovement();
    }

    void ResetPaddle() {
        y = screen_height / 2 - height / 2;
        x = width / 2;
    }

    void ResetPaddle(int) {
        y = screen_height / 2 - height / 2;
        x = screen_width - width - 10;
    }

    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= screen_height) {
            y = screen_height - height;
        }
    }
};


// Initialize ball and paddles
Ball ball;
Paddle player;
Paddle cpu;

// Update CPU logic
void UpdateCPU(float deltaTime) {
    float velocity = cpu.speed * deltaTime*25 ;
    Color defaultColor = cpu.color;
    if (cpu.y + cpu.height / 2 < ball.y) {
        cpu.y += velocity;
        cpu.color = BLUE; // Change color when moving down
    } 
    else if (cpu.y + cpu.height / 2 > ball.y) {
        cpu.y -= velocity;
        cpu.color = BLUE; // Change color when moving up
    } 
    else {
        cpu.color = defaultColor;
    }
    
    cpu.LimitMovement();
}

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;
    Color color;
};

vector<Particle> particles;

void UpdateParticles(float deltaTime) {
    for (auto &particle : particles) {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.life -= deltaTime;
    }
    particles.erase(remove_if(particles.begin(), particles.end(), [](Particle &p) { return p.life <= 0; }), particles.end());
}

void DrawParticles() {
    for (auto &particle : particles) {
        DrawCircleV(particle.position, 2, particle.color);
    }
}

// Example of spawning particles (e.g., on collision)
void SpawnParticles(Vector2 position, int count,Color) {
    for (int i = 0; i < count; i++) {
        particles.push_back({position, {GetRandomValue(-100, 100) / 10.0f, GetRandomValue(-100, 100) / 10.0f}, 1.0f,RED});
    }
}
void UpdatedMenu(){
    player_score = 0;
    cpu_score=0;
    // Reset name input variables
    letterCount1 = 0;
    letterCount2 = 0;
    enterName1 = true;  // To re-enable player name input
    enterName2 = false;
    player1_name[0] = '\0';
    player2_name[0] = '\0';
}      


int main() {

    cout << "READY TO PLAY" << endl;
    InitWindow(screen_width, screen_height, "PROJECT Ping-Pong");
    InitAudioDevice();

    Rectangle buttonPlayerVsPlayer = { screen_width / 2 - 100, screen_height / 2 - 50, 200, 50 };
    Rectangle buttonPlayerVsCpu = { screen_width / 2 - 100, screen_height / 2 + 25, 200, 50 };
    Rectangle buttonEasy = { screen_width / 2 - 100, screen_height / 2 - 50, 200, 50 };
    Rectangle buttonMedium = { screen_width / 2 - 100, screen_height / 2 + 50, 200, 50 };
    Rectangle buttonHard = { screen_width / 2 - 100, screen_height / 2 + 150, 200, 50 };
    Rectangle buttonStart = { screen_width / 2 - 100, screen_height / 2 -50, 200, 50 };

    Music backgroundMusic = LoadMusicStream("game.mp3");
    Sound ballhit = LoadSound("ballhit.mp3");
    Sound ballgnd = LoadSound("ballgnd.mp3");
    PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.05f);
    SetTargetFPS(90);

    ball.radius = 10;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 12;
    ball.speed_y = 12;

    player.width = 15;
    player.height = 100;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 12;  // Set initial speed for player paddle
    player.color = Paddle_Color;

    cpu.height = 100;
    cpu.width = 15;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 12;  // Set initial speed for CPU paddle
    cpu.color = Paddle_Color;

    while (!WindowShouldClose()) {
        UpdateMusicStream(backgroundMusic);
        float deltatime=GetFrameTime();
        UpdateParticles(deltatime); // Update particles
        if (state == menu) {
            BeginDrawing();
            ClearBackground(Background_Color);
            DrawText("Main Menu", screen_width / 2 - MeasureText("Main Menu", 40) / 2, 100, 40, Paddle_Color);

            DrawRectangleRec(buttonPlayerVsPlayer, Paddle_Color);
            DrawText("Multi_player",buttonPlayerVsPlayer.x + 30,buttonPlayerVsPlayer.y + 10, 20, Background_Color);

            DrawRectangleRec(buttonPlayerVsCpu, Paddle_Color);
            DrawText("Single_player", buttonPlayerVsCpu.x + 30, buttonPlayerVsCpu.y + 10, 20, Background_Color);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousepos = GetMousePosition();
                SpawnParticles(mousepos, 20, RED);
                if (CheckCollisionPointRec(mousepos, buttonPlayerVsCpu)) {
                    currentMode = single_player;
                    state = input_name;
                }
                else if (CheckCollisionPointRec(mousepos,buttonPlayerVsPlayer )) {
                    currentMode = player_vs_player;
                    state = input_name;
                }
            }
            DrawParticles();
            EndDrawing();
        }
        if (state == input_name) {
            BeginDrawing();
            ClearBackground(Background_Color);

            DrawText("Enter Player Names", screen_width / 2 - MeasureText("Enter Player Names", 40) / 2, 100, 40, Paddle_Color);
            if (enterName1) {
                DrawText("Player 1 Name: ", screen_width / 2 - 300, screen_height / 2 - 50, 20, Paddle_Color);
                DrawText(player1_name, screen_width / 2 - 50, screen_height / 2 - 50, 20, Paddle_Color);
            } else if (enterName2) {
                DrawText("Player 2 Name: ", screen_width / 2 - 300, screen_height / 2-50, 20, Paddle_Color);
                DrawText(player2_name, screen_width / 2 - 50, screen_height / 2-50, 20, Paddle_Color);
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (enterName1 && letterCount1 > 0) {
                    letterCount1--;
                    player1_name[letterCount1] = '\0';
                } else if (enterName2 && letterCount2 > 0) {
                    letterCount2--;
                    player2_name[letterCount2] = '\0';
                }
            }

            int key = GetKeyPressed();
            if (key >= 32 && key <= 125) {
                if (enterName1 && letterCount1 < 10) {
                    player1_name[letterCount1] = (char)key;
                    player1_name[++letterCount1] = '\0';
                } else if (enterName2 && letterCount2 < 10) {
                    player2_name[letterCount2] = (char)key;
                    player2_name[++letterCount2] = '\0';
                }
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (enterName1) {
                    enterName1 = false;
                    if (currentMode == player_vs_player) {
                        enterName2 = true;
                    } else {
                        state = select_level;
                    }
                } else if (enterName2) {
                    state = select_level;
                }
            }
            DrawParticles();
            EndDrawing();
        }
        // Add the rest of the states (playing, paused, won, etc.) as per your original code...
        if (state == select_level) {
            BeginDrawing();
            ClearBackground(Background_Color);
            DrawText("Select Difficulty Level", screen_width / 2 - MeasureText("Select Difficulty Level", 40) / 2, 100, 40, Paddle_Color);

            DrawRectangleRec(buttonEasy, Paddle_Color);
            DrawText("Easy", buttonEasy.x + 75, buttonEasy.y + 10, 20, Background_Color);

            DrawRectangleRec(buttonMedium, Paddle_Color);
            DrawText("Medium", buttonMedium.x + 75, buttonMedium.y + 10, 20, Background_Color);

            DrawRectangleRec(buttonHard, Paddle_Color);
            DrawText("Hard", buttonHard.x + 75, buttonHard.y + 10, 20, Background_Color);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousepos = GetMousePosition();
                if (CheckCollisionPointRec(mousepos, buttonEasy)) {
                    currentDifficulty = easy;
                    state = start_game;
                }
                else if (CheckCollisionPointRec(mousepos, buttonMedium)) {
                    currentDifficulty = medium;
                    state = start_game;
                }
                else if (CheckCollisionPointRec(mousepos, buttonHard)) {
                    currentDifficulty = hard;
                    state = start_game;
                }
            }
            DrawParticles();
            EndDrawing();
        }

        if (state == start_game) {
            BeginDrawing();
            ClearBackground(Background_Color);
            DrawText("Ready to Start!", screen_width / 2 - MeasureText("Ready to Start!", 40) / 2, 250, 40, Paddle_Color);
            DrawRectangleRec(buttonStart, Paddle_Color);
            DrawText("Start Game", buttonStart.x + 30, buttonStart.y + 10, 20, Background_Color);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousepos = GetMousePosition();
                if (CheckCollisionPointRec(mousepos, buttonStart)) {
                    state = playing;
                }
            }
            DrawParticles();
            EndDrawing();
        }
    if (state == playing) {
        BeginDrawing();
        ClearBackground(Background_Color);
        DrawLine(screen_width / 2,0,screen_width / 2,screen_height,WHITE);
        //Ensure CPU name is displayed in single-player mode
        if (currentMode == single_player) {
            strcpy(player2_name, "CPU");
            DrawText(TextFormat("%-10s: %i", player1_name, player_score), screen_width / 2+50 , 20, 40, BLACK);
            DrawText(TextFormat("%-10s: %i", player2_name,cpu_score), screen_width / 2-350, 20, 40, BLACK);
        }
        else{
            // Adjust spacing for names and scores
            DrawText(TextFormat("%-10s: %i", player1_name,cpu_score), screen_width / 2 -350, 20, 40, BLACK);
            
            DrawText(TextFormat("%-10s: %i", player2_name,player_score), screen_width / 2 + 50, 20, 40, BLACK);
           
        }
        
         // Game pause and restart control
        if (IsKeyPressed(KEY_R)) {
            state = paused;
        if (IsKeyPressed(KEY_A)) {
            player_score = 0;
            cpu_score = 0;
            ball.ResetBall();
            state = playing;
            cpu.ResetPaddle();
            player.ResetPaddle(1);
        }}
    
    // Draw ball and paddles
    ball.Draw();
    player.Draw();
    cpu.Draw();

    // Paddle updates based on mode
    if (currentMode == player_vs_player) {
        player.Update(1,deltatime);  // Player 1 controlled by arrow keys
        cpu.Update(2,deltatime);     // Player 2 controlled by 'W' and 'S' keys
    } 
    else{
        player.Update(1,deltatime);  // Player controlled by arrow keys
        UpdateCPU(deltatime);       // CPU controls the other paddle
    }
UpdateMusicStream(backgroundMusic);
    // Ball update
    ball.Update();
 if (ball.y + ball.radius >= screen_height || ball.y - ball.radius <= 0)
            {
                PlaySound(ballgnd);
                SpawnParticles(Vector2{ ball.x, ball.y }, 10, GREEN);
            }
    // Collision handling for paddles and ball
   if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ player.x, player.y, player.width, player.height }) && ball.speed_x > 0) {
    ball.speed_x *= -1;
    PlaySound(ballhit);
    SpawnParticles(Vector2{ player.x, player.y }, 10, BLUE);
}

if (CheckCollisionCircleRec(Vector2{ ball.x, ball.y }, ball.radius, Rectangle{ cpu.x, cpu.y, cpu.width, cpu.height }) && ball.speed_x < 0) {
    ball.speed_x *= -1;
    PlaySound(ballhit);
    SpawnParticles(Vector2{ cpu.x, cpu.y }, 10, BLUE);
}
            DrawParticles();   
            EndDrawing();
        }

        if (state == paused) {
            BeginDrawing();
            ClearBackground(Background_Color);
            DrawText("Game Paused", screen_width / 2 - MeasureText("Game Paused", 40) / 2, screen_height / 2 - 50, 40, Paddle_Color);
            DrawText("Press R to Resume", screen_width / 2 - MeasureText("Press R to Resume", 20) / 2, screen_height / 2, 20, Paddle_Color);
            DrawText("Press A to Restart", screen_width / 2 - MeasureText("Press A to Restart", 20) / 2, screen_height / 2 + 50, 20, Paddle_Color);
            DrawText("Press M to Menu", screen_width / 2 - MeasureText("Press M to Restart", 20) / 2, screen_height / 2 + 100, 20, Paddle_Color);
            if (IsKeyPressed(KEY_R)) {
                state = playing;
            }
            if (IsKeyPressed(KEY_A)) {
                player_score = 0;
                cpu_score = 0;
                ball.ResetBall();
                state = playing;
            }
            if (IsKeyPressed(KEY_M)) {
                UpdatedMenu();
                state = menu;
            }
            DrawParticles();
            EndDrawing();
        }
        if (state == won) {
            BeginDrawing();
            ClearBackground(Background_Color);
            if(strcmpi(player2_name,"CPU")==0)
            {
                if (winner ==1) {
                DrawText(player1_name, screen_width / 2 - MeasureText(player1_name, 30) / 2-50, screen_height / 2 - 50, 40, Paddle_Color);
                DrawText("WON", screen_width / 2 - MeasureText(player1_won, 30) / 2+strlen(player1_name)*28+140/strlen(player1_name), screen_height / 2 - 50, 40, Paddle_Color);
                }  
                else if (winner == 2) {
                        DrawText(player2_name, screen_width / 2 - MeasureText(player2_name, 30) / 2-50, screen_height / 2 - 50, 40, Paddle_Color);
                        DrawText("WON", screen_width / 2 - MeasureText(player2_won, 30) / 2+strlen(player2_name)*28+140/strlen(player2_name), screen_height / 2 - 50, 40, Paddle_Color);
                }  
            }
            else{
                if (winner ==2) {
                DrawText(player1_name, screen_width / 2 - MeasureText(player1_name, 30) / 2-50, screen_height / 2 - 50, 40, Paddle_Color);
                DrawText("WON", screen_width / 2 - MeasureText(player1_won, 30) / 2+strlen(player1_name)*26+140/strlen(player1_name), screen_height / 2 - 50, 40, Paddle_Color);
                }  
                else if (winner == 1) {
                        DrawText(player2_name, screen_width / 2 - MeasureText(player2_name, 30) / 2-50, screen_height / 2 - 50, 40, Paddle_Color);
                        DrawText("WON", screen_width / 2 - MeasureText(player2_won, 30) / 2+strlen(player2_name)*26+140/strlen(player2_name), screen_height / 2 - 50, 40, Paddle_Color);
                }  
            }
            
        
        
            DrawText("Press A to Restart", screen_width / 2 - MeasureText("Press A to Restart", 20) / 2+25, screen_height / 2 + 50, 20, Paddle_Color);

            if (IsKeyPressed(KEY_A)) {
                player_score = 0;
                cpu_score = 0;
                ball.ResetBall();
                state = playing;
            }
             DrawText("Press M to Menu", screen_width / 2 - MeasureText("Press A to Restart", 20) / 2+25, screen_height / 2 + 100, 20, Paddle_Color);

            if (IsKeyPressed(KEY_M)) {
                UpdatedMenu();
                state = menu;
            }
            DrawParticles();
            EndDrawing();
        }
         
        
    }
    
    CloseWindow();
    return 0;

}
    
