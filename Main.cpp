#include <iostream>
#include <string.h>
#include <raylib.h>
#include <string>
#include <random>

using namespace Raylib;

#define VICTORYSCORE 3

Vector2& operator-(Vector2& vector)
{
	vector.x = -vector.x;
	vector.y = -vector.y;

	return vector;
}

template<typename T>
T ClampValue(T Value, T Min, T Max)
{
	return Value > Max ? Max : Value < Min ? Min : Value;
}

template<typename T>
T RandRange(T Min, T Max)
{
	//Modern C++ way of generating a rand value in range
	std::random_device rd;  // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(Min, Max); // define the range

	return distr(gen);
}

struct FPlayerData
{
	Vector2 InitialPosition;
	std::string PlayerName = "";
	int Score = 0;
	float Velocity = 600.f;

};

struct FBallData
{
	Vector2 InitialPosition;
	Vector2 CurrentPosition;
	Vector2 Velocity;

	Color BallColor;
	float Size;
};

enum EScreen
{
	MENU,
	GAME,
	WINSCREEN,
	PAUSE
};

bool CheckCollisionBall(const Rectangle& Rect, const Vector2& BallPosition, const float& BallSize)
{
	return Rect.x + Rect.width > BallPosition.x - BallSize / 2.f;
}

void LimitPlayerBoundary(Rectangle& Player, const int WindowHeight)
{
	if (Player.y < 0)
	{
		Player.y = 0.f;		
	}
	if (Player.y + Player.height > WindowHeight)
	{
		Player.y = WindowHeight - Player.height;
	}
}

void BallMovement(FBallData& BallData, int WindowWidth, int WindowHeight, bool& bGameHasStarted, FPlayerData& P01Data, FPlayerData& P02Data)
{
	if(!bGameHasStarted) return;

	BallData.CurrentPosition.x += BallData.Velocity.x * GetFrameTime();
	BallData.CurrentPosition.y += BallData.Velocity.y * GetFrameTime();

	if (BallData.CurrentPosition.y - BallData.Size < 0)
	{
		BallData.Velocity.y = -BallData.Velocity.y;
	}
	else if (BallData.CurrentPosition.y + BallData.Size > WindowHeight)
	{
		BallData.Velocity.y = -BallData.Velocity.y;
	}

	//Reset ball
	//Give player 01 score
	if (BallData.CurrentPosition.x > WindowWidth)
	{
		P01Data.Score++;
		BallData.CurrentPosition = BallData.InitialPosition;
		bGameHasStarted = false;		
	}
	//Give player 02 score
	else if (BallData.CurrentPosition.x + BallData.Size < 0)
	{
		P02Data.Score++;
		BallData.CurrentPosition = BallData.InitialPosition;
		bGameHasStarted = false;
	}

}

void PlayersMovementInput(Rectangle& P01, float &P01Velocity, Rectangle& P02, float& P02Velocity)
{
	//Player 1
	if (IsKeyDown(KEY_W))
	{
		P01.y -= P01Velocity * GetFrameTime();
	}

	if (IsKeyDown(KEY_S))
	{
		P01.y += P01Velocity * GetFrameTime();
	}

	//Player 2
	if (IsKeyDown(KEY_UP))
	{
		P02.y -= P02Velocity * GetFrameTime();
	}

	if (IsKeyDown(KEY_DOWN))
	{
		P02.y += P02Velocity * GetFrameTime();
	}
}


void RestartGame(FPlayerData* P01Data, FPlayerData* P02Data, Rectangle* Player01Rect, Rectangle* Player02Rect, FBallData* BallData, bool& bGameHasStarted)
{
	if (P01Data && P02Data)
	{
		P01Data->Score = 0;
		P02Data->Score = 0;
	}

	if (Player01Rect && Player02Rect)
	{
		Player01Rect->x = P01Data->InitialPosition.x;
		Player01Rect->y = P01Data->InitialPosition.y;

		Player02Rect->x = P02Data->InitialPosition.x;
		Player02Rect->y = P02Data->InitialPosition.y;
	}

	if (BallData)
	{
		BallData->CurrentPosition = BallData->InitialPosition;
	}
	bGameHasStarted = false;
}

bool GameEnds(FPlayerData* P01Data, FPlayerData* P02Data, int WindowWidth, int WindowHeight)
{
	const int FontSize = 60; 

	if (P01Data)
	{
		if (P01Data->Score >= VICTORYSCORE)
		{
			DrawText("Player 1 Wins!", WindowWidth / 2 - 150, 350, FontSize, BLUE);
			DrawText("Press enter to play again!", WindowWidth / 2 - 210, 450, 40, WHITE);
			return true;
		}
	}

	if (P02Data)
	{
		if (P02Data->Score >= VICTORYSCORE)
		{
			DrawText("Player 2 Wins!", WindowWidth / 2 - 150, 350, FontSize, ORANGE);
			DrawText("Press enter to play again!", WindowWidth / 2 - 210, 450, 40, WHITE);
			return true;
		}
	}
	return false;
}
int main()
{
	EScreen CurrentScreen = EScreen::MENU;

	bool bRequestToCloseGame = false;

	//Window
	const int WindowWidth = 1200;
	const int WindowHeight = 800;

	const char* WindowTitle = "Pong by Rafael Zagolin";

	InitWindow(WindowWidth, WindowHeight, WindowTitle);
	
	//Main screen
	const char* GameNameText = "PONG";
	const char* EnterTheGameText = "---Press ENTER to PLAY---";
	const int GameNameTextSize = 30;
	const int GameNameTextPositionX = WindowWidth / 2.f - GameNameTextSize;
	const int GameNameTextPositionY = WindowHeight / 2.f - GameNameTextSize;

	//Game
	bool bGameHasStarted = false;


	//Players shared information
	Vector2 PlayersSize;
	PlayersSize.x = 15.f;
	PlayersSize.y = 100.f;	
	
	//Player 01 settings
	Rectangle Player01;
	FPlayerData P01Data;
	std::string P01Text;
	Player01.width = PlayersSize.x;
	Player01.height = PlayersSize.y;

	P01Data.InitialPosition.x = 0;
	P01Data.InitialPosition.y = WindowHeight / 2 - PlayersSize.y / 2;
	Player01.x = P01Data.InitialPosition.x;
	Player01.y = P01Data.InitialPosition.y;

	//Player 02 settings
	Rectangle Player02;
	FPlayerData P02Data;
	std::string P02Text;

	Player02.width = PlayersSize.x;
	Player02.height = PlayersSize.y;

	P02Data.InitialPosition.x = WindowWidth - PlayersSize.x;
	P02Data.InitialPosition.y = WindowHeight / 2 - PlayersSize.y / 2;
	Player02.x = P02Data.InitialPosition.x;
	Player02.y = P02Data.InitialPosition.y;

	//Ball settings
	FBallData BallData;
	BallData.Size = 10.f;
	BallData.InitialPosition.x = WindowWidth / 2.f - (BallData.Size / 2.f);
	BallData.InitialPosition.y = WindowHeight / 2.f - (BallData.Size / 2.f);
	BallData.CurrentPosition = BallData.InitialPosition;
	BallData.BallColor = RED;
	BallData.Velocity.x = RandRange(300.f, 500.f);
	BallData.Velocity.y = RandRange(-100.f, 100.f);

	//Middle screen line division
	Color LineColor;
	LineColor.r = 255.f;
	LineColor.g = 255.f;
	LineColor.b = 255.f;
	LineColor.a = 100.f;

	while (!bRequestToCloseGame)
	{
		BeginDrawing();

		switch (CurrentScreen)
		{
			case EScreen::MENU:
			{
				DrawText(GameNameText, GameNameTextPositionX, GameNameTextPositionY, GameNameTextSize, WHITE);
				DrawText(EnterTheGameText, GameNameTextPositionX - 180.f, GameNameTextPositionY + 40.f, GameNameTextSize, WHITE);


				//Entering the game...
				if (IsKeyPressed(KEY_ENTER))
				{
					CurrentScreen = EScreen::GAME;
				}
				if (IsKeyPressed(KEY_ESCAPE))
				{
					bRequestToCloseGame = true;
					CloseWindow();
				}
			}
				break;

			case EScreen::GAME:
			{
				//Check end game
				if (GameEnds(&P01Data, &P02Data, WindowWidth, WindowHeight))
				{
					CurrentScreen = EScreen::WINSCREEN;
				}

				if (IsKeyPressed(KEY_ESCAPE))
				{
					CurrentScreen = EScreen::MENU;
				}

				//Entering the game...
				if (IsKeyPressed(KEY_R))
				{					
					RestartGame(&P01Data, &P02Data, &Player01, &Player02, &BallData, bGameHasStarted);
				}
				//Pausing game...
				if (IsKeyPressed(KEY_P))
				{
					CurrentScreen = EScreen::PAUSE;
				}

				if (!bGameHasStarted)
				{
					if(IsKeyPressed(KEY_SPACE))
						bGameHasStarted = !bGameHasStarted;
				}

				//Input handling
				PlayersMovementInput(Player01, P01Data.Velocity, Player02, P02Data.Velocity);

				//Players score HUD
				P01Text = std::to_string(P01Data.Score);
				P02Text = std::to_string(P02Data.Score);
			
				//Player 01
				DrawRectangleRec(Player01, BLUE);
				DrawText(P01Text.c_str(), WindowWidth / 2.f - 60.f, 10, 50, WHITE); //Player 01 score
				LimitPlayerBoundary(Player01, WindowHeight);

				//Player 02
				DrawRectangleRec(Player02, ORANGE);	
				DrawText(P02Text.c_str(), WindowWidth / 2.f + 30.f, 10, 50, WHITE); //Player 02 score
				LimitPlayerBoundary(Player02, WindowHeight);				

				/**********BALL COLLISION**********/
				//With player 01
				if (CheckCollisionCircleRec(BallData.CurrentPosition, BallData.Size, Player01))
				{
					BallData.Velocity.x = -RandRange(300.f, 500.f);
					BallData.Velocity.y = RandRange(-100.f, 100.f);
					BallData.Velocity = -BallData.Velocity;

				}

				//With player 02
				if (CheckCollisionCircleRec(BallData.CurrentPosition, BallData.Size, Player02))
				{
					BallData.Velocity.x = RandRange(300.f, 500.f);
					BallData.Velocity.y = RandRange(-100.f, 100.f);
					BallData.Velocity = -BallData.Velocity;
				}

				//Ball
				DrawCircle(BallData.CurrentPosition.x, BallData.CurrentPosition.y, BallData.Size, RED);
				BallMovement(BallData, WindowWidth, WindowHeight, bGameHasStarted, P01Data, P02Data);

				//Middle screen line division
				DrawLine(WindowWidth / 2.f, 0.f, WindowWidth / 2.f, WindowHeight, LineColor);
			}
				break;
			case EScreen::WINSCREEN:
			{
				GameEnds(&P01Data, &P02Data, WindowWidth, WindowHeight);
				if (IsKeyPressed(KEY_ENTER))
				{
					RestartGame(&P01Data, &P02Data, &Player01, &Player02, &BallData, bGameHasStarted);
					CurrentScreen = EScreen::GAME;
				}
			}
				break;
			case EScreen::PAUSE:
			{
				DrawLine(WindowWidth / 2.f, 0.f, WindowWidth / 2.f, WindowHeight, LineColor);
				DrawCircle(BallData.CurrentPosition.x, BallData.CurrentPosition.y, BallData.Size, RED);
				DrawRectangleRec(Player02, ORANGE);
				DrawText(P02Text.c_str(), WindowWidth / 2.f + 30.f, 10, 50, WHITE); //Player 02 score
				DrawRectangleRec(Player01, BLUE);
				DrawText(P01Text.c_str(), WindowWidth / 2.f - 60.f, 10, 50, WHITE); //Player 01 score
				DrawText("PAUSED", GameNameTextPositionX - 150, 250, 100, YELLOW);

				//Unpause game
				if (IsKeyPressed(KEY_P))
				{
					CurrentScreen = EScreen::GAME;
				}

			}
				break;
		}

		ClearBackground(BLACK);
		EndDrawing();
	}

	CloseWindow();

	system("PAUSE > 0");
	return 0;
}
