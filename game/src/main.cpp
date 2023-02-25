/*
 *
 *Floppy Bird
 *By RadsammyT
 *segfault code: *(int*) 0x0 = 0;
 *
 */

//#define DEBUG // R to reset birb

#include <raylib-cpp.hpp>
#include <vector>
#include <cfloat>
#include <array>
#include <ctime>
#include <math.h>

int main() {
    // Initialization
	int screenWidth = 800*1.5;
	int screenHeight = 450*1.5;
    raylib::Window w(screenWidth, screenHeight, "Gaming");
	
	struct pipe { //
		raylib::Rectangle top;
		raylib::Rectangle bottom;
	};

	std::array<pipe, 2> pipes = {
		{
			{raylib::Rectangle(0,-999,50,50), raylib::Rectangle(0,-999,50,50)},
			{raylib::Rectangle(screenWidth/2,-999,50,50), raylib::Rectangle(screenWidth/2,-999,50,50)}
		}
	};
	raylib::Rectangle player (screenWidth/2-25,screenHeight/2,50,50);
	const float terminal  = 100;
	const float gravJump = -16.67;
	const int   gapMax = 300;
	const int   gapMin = 150;
	int gap = 0;
	float grav = 0;
	float gravRate = 100;
	float speed = 5;
	bool over = false;

	raylib::Image floppy = LoadImage("resources/floppy2.png");
	ImageFormat(&floppy, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
	ImageResize(&floppy, 50, 50);
	raylib::Texture ftex = LoadTextureFromImage(floppy);
	
	raylib::Image background = LoadImage("resources/background.png");
	raylib::Texture btex = LoadTextureFromImage(background);

	raylib::Image pipe = LoadImage("resources/pipe.png");
	raylib::Texture tpipe = LoadTextureFromImage(pipe);
	enum state { // theres no STATE_GAMEOVER because segfault
		STATE_TITLE,
		STATE_GAMEPLAY
	};
	
	state cur = STATE_TITLE;
	float score = 0;
	SetTargetFPS(60);

	SetRandomSeed(std::time(nullptr));
    // Main game loop
    while (!w.ShouldClose()) // Detect window close button or ESC key
    {
        // Update
		if(cur == STATE_TITLE) {
			if(IsKeyPressed(KEY_SPACE)) {
				cur = STATE_GAMEPLAY;
				for(auto p: pipes) {
					gap = GetRandomValue(gapMin, screenHeight-gapMax);
					p.top.SetHeight(gap-9999);	
					p.bottom.SetY(p.top.GetHeight() + gap-9999);
					p.bottom.SetHeight(screenHeight - p.bottom.GetY());
				}
			}
		} else {	
			score += GetFrameTime();
			grav += gravRate * GetFrameTime();
			if(grav>terminal) {
				grav = terminal;
			}

			if(IsKeyPressed(KEY_SPACE)) {
				grav = gravJump;
			}

			player.SetY(player.GetY() + grav);
			
			if(IsKeyPressed(KEY_R)) {
				player.SetY(screenHeight/2);
				grav = 0;
			}

			for(int i = 0; i < 2; i++) {
				pipes[i].top.SetX(pipes[i].top.GetX() - speed);
				pipes[i].bottom.SetX(pipes[i].bottom.GetX() - speed);
				if(pipes[i].top.GetX() - pipes[i].top.GetWidth() < -100
					&& pipes[i].bottom.GetX() - pipes[i].bottom.GetWidth() < -100) {
					pipes[i].top.SetX(screenWidth + pipes[i].top.GetWidth());
					pipes[i].top.SetY(0);
					pipes[i].bottom.SetX(screenWidth + pipes[i].bottom.GetWidth());
					
					gap = GetRandomValue(gapMin, screenHeight-gapMax);
					pipes[i].top.SetHeight(gap);	
					pipes[i].bottom.SetY(pipes[i].top.GetHeight() + gap);
					pipes[i].bottom.SetHeight(screenHeight - pipes[i].bottom.GetY());
				}

				if(pipes[i].top.CheckCollision(player) || pipes[i].bottom.CheckCollision(player) 
					|| player.GetY() < -10 || player.GetY() > screenHeight + 25) {
					printf("Death. Score=%d\n", (int)score);
					//*(int*) 0x0 = 0; //SEGFAULT NOW
					over = true;
				}
			}		

		}
		BeginDrawing();
			ClearBackground(BLACK);
			btex.Draw(0,0,WHITE);
			if(cur == STATE_TITLE) {
				DrawText("Floppy Bird", (screenWidth/2)-2.75*60, 0, 60, WHITE);
				DrawText("Space to Begin/\"Flap\"", (screenWidth/2)-5*60, 60, 60, WHITE);
				DrawText("If you die, game segfaults", (screenWidth/2)-6.5*60, 120, 60, WHITE);

				ftex.Draw((double)screenWidth/2-25, (double)sin(GetTime()/GetFrameTime() / 10) * 10 + screenHeight/2, WHITE);

				DrawFPS(0,30);
			} else {
				player.Draw(WHITE);
				ftex.Draw(player.GetX(), player.GetY(), WHITE);
				for(short i = 0; i < pipes.size(); i++) {
					pipes[i].top.Draw(GREEN);
					pipes[i].bottom.Draw(RED);

					DrawTextureRec(tpipe, raylib::Rectangle(0,0,pipes[i].top.GetWidth(),pipes[i].top.GetHeight()), raylib::Vector2(pipes[i].top.GetX(), pipes[i].top.GetY()), WHITE);

					DrawTextureRec(tpipe, raylib::Rectangle(0,0,pipes[i].bottom.GetWidth(),pipes[i].bottom.GetHeight()), raylib::Vector2(pipes[i].bottom.GetX(), pipes[i].bottom.GetY()), WHITE);
				}

				DrawFPS(0,30);
				DrawText(std::to_string((int)score).c_str(), 0, 0, 30, GRAY);
			}

		EndDrawing();

		if(over) {
			*(int*) 0x0 = 0;
			CloseWindow();
		}
    }

    return 0;
}
