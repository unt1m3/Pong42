#include <raylib.h>
#include <stdlib.h>
#include <stdbool.h>
#define SpeedY 4
#define SpeedX 3
#define PadSpeed 6

typedef enum{
	BallStartPlayer,
	BallStartPlayer2,
	BallMovePlayer,
	BallMovePlayer2
} BallStates;

typedef enum{
	UpRight,
	DownRight,
	UpLeft,
	DownLeft,
	Up,
	Down,
	None,
	FromPlayer,
	FromPlayer2
} Dirs;


typedef struct{
	Dirs FirstShoot, PlayerLastDir,
	Player2LastDir, BallDir, LastBounce;
	Rectangle PlayerPad;
	Rectangle Player2Pad;
	Rectangle Ball;
	BallStates BallState;
	int PlayerPoints, 
	Player2Points, LastWin;
} GameData;


GameData *InitGame(){
	GameData *ptr = malloc(sizeof(GameData));
	ptr->PlayerPad = (Rectangle){10,225,10,40};
	ptr->Player2Pad = (Rectangle){580,225,10,40};
	ptr->Ball = (Rectangle){25,ptr->PlayerPad.y,10,10};
	ptr->BallState = BallStartPlayer;
	ptr->FirstShoot = true;
	ptr->LastWin = None;
	return ptr;
}

void MoveBall(GameData *data){
	if (data->BallDir == UpRight){
		data->Ball.x += SpeedX;
		data->Ball.y -= SpeedY;
	}else if (data->BallDir == DownRight){
		data->Ball.x += SpeedX;
		data->Ball.y += SpeedY;
	}else if (data->BallDir == UpLeft){
		data->Ball.x -= SpeedX;
		data->Ball.y -= SpeedY;
	}else if (data->BallDir == DownLeft){
		data->Ball.x -= SpeedX;
		data->Ball.y += SpeedY;
	}
	
}

void BallCheckShoot(GameData *data){
	if (data->BallState == BallMovePlayer){
		if (data->PlayerLastDir == Up){
			data->BallDir = UpRight;
		}else{
			data->BallDir = DownRight;
		}
	}if (data->BallState == BallMovePlayer2){
		if (data->Player2LastDir == Up){
			data->BallDir = UpLeft;
		}else{
			data->BallDir = DownLeft;
		}
	}
}

void BallCollisions(GameData *data){
	if (CheckCollisionRecs(data->Ball,data->Player2Pad)){
		data->LastBounce = FromPlayer2;
		if (data->Player2LastDir == Up){
			data->BallDir = UpLeft;
		}else{
			data->BallDir = DownLeft;
		}
	}else if(CheckCollisionRecs(data->Ball,data->PlayerPad)){
		if (data->PlayerLastDir == Up){
			data->BallDir = UpRight;
		}else{
			data->BallDir = DownRight;
		}
		data->LastBounce = FromPlayer;
	}
}

void BallPhysics(GameData *data){
	if (data->BallState == BallStartPlayer){
		BallCheckShoot(data);
		data->Ball.y = data->PlayerPad.y + 15;
		data->LastBounce = FromPlayer;
		data->Ball.x = 25;
	}else if(data->BallState == BallStartPlayer2){
		BallCheckShoot(data);
		data->Ball.y = data->Player2Pad.y + 15;
		data->LastBounce = FromPlayer2;
		data->Ball.x = 565;	
	}else{
		MoveBall(data);
		BallCollisions(data);
		if (data->Ball.x > 590){
			data->PlayerPoints += 1;
			data->BallState = BallStartPlayer2;
		}else if (data->Ball.x < 10){
			data->Player2Points += 1;
			data->BallState = BallStartPlayer;
		}else if (data->Ball.y <= 0){
			if (data->LastBounce == FromPlayer){
				data->BallDir = DownRight;
			}else{
				data->BallDir = DownLeft;
			}
		}else if (data->Ball.y >= 400){
			if (data->LastBounce == FromPlayer){
				data->BallDir = UpRight;
			}else{
				data->BallDir = UpLeft;
			}
		}
	}
}

void RestartGame(GameData *data){
	data->FirstShoot = true;
	data->PlayerPoints = 0;
	data->Player2Points = 0;
	data->PlayerPad.y = 225;
	data->Player2Pad.y = 225;	
}

void CheckGameState(GameData *data){
	if (data->PlayerPoints == 10){
		data->BallState = BallStartPlayer2;
		data->FirstShoot = true;	
		data->LastWin = FromPlayer;
		RestartGame(data);	
	}else if (data->Player2Points == 10){
		data->BallState = BallStartPlayer;
		data->FirstShoot = true;	
		data->LastWin = FromPlayer2;
		RestartGame(data);
	}
}

void GameDrawing(GameData *data){
	DrawText(TextFormat("%d", data->PlayerPoints), 70, 60, 50, WHITE);
	DrawText(TextFormat("%d", data->Player2Points), 500, 60, 50, WHITE);
	DrawRectangleRec(data->PlayerPad,WHITE);
	DrawRectangleRec(data->Player2Pad,WHITE);
	DrawRectangleRec(data->Ball,WHITE);
	if (data->BallState == BallStartPlayer2){
		DrawText("Press space to start", 310, 10, 20, WHITE);
	}else if (data->BallState == BallStartPlayer){
		DrawText("Press space to start", 10, 10, 20, WHITE);
	}
	if (data->FirstShoot){
		if (data->LastWin == FromPlayer){
			DrawText("Well done!", 10, 10, 20, WHITE);
		}else if (data->LastWin == FromPlayer2){
			DrawText("Well done!", 310, 10, 20, WHITE);
		}
		DrawText("v1.0, GPL v2.0, By Runt1m3_", 10, 35, 20, WHITE);
		DrawText("W/S for player 1 movement", 5, 380, 20, WHITE);
		DrawText("10 points and you win", 5, 360, 20, WHITE);
		DrawText("I/S for player 2 movement", 305, 380, 20, WHITE);
		DrawText("10 points and you win", 305, 360, 20, WHITE);
	}
	for (int i = 0; i < 40; i++){
		DrawRectangle(290,10 * i + 1,8,8,WHITE);	
	}
}

void ManagePads(GameData *data){
	if (IsKeyDown(KEY_W) && data->PlayerPad.y > 0){
		data->PlayerPad.y -= PadSpeed;
		data->PlayerLastDir = Up;
	}if (IsKeyDown(KEY_S) && data->PlayerPad.y < 360){
		data->PlayerPad.y += PadSpeed;
		data->PlayerLastDir = Down;
	}if (IsKeyDown(KEY_I) && data->Player2Pad.y > 0){
		data->Player2Pad.y -= PadSpeed;
		data->Player2LastDir = Up;
	}if (IsKeyDown(KEY_K) && data->Player2Pad.y < 360){
		data->Player2Pad.y += PadSpeed;
		data->Player2LastDir = Down;
	}if (IsKeyDown(KEY_SPACE)){
		if (data->FirstShoot){
			data->FirstShoot = false;
		}
		if (data->BallState == BallStartPlayer && data->Ball.x == 25){
			data->BallState = BallMovePlayer;
		}else if(data->BallState == BallStartPlayer2 && data->Ball.x == 565){
			data->BallState = BallMovePlayer2;
		}
	}
}

int main(){
	GameData *data = InitGame();
	InitWindow(600,400,"Pong42");
	SetTargetFPS(60);
	while (!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);
		GameDrawing(data);
		BallPhysics(data);
		ManagePads(data);
		CheckGameState(data);
		EndDrawing();
	}
	free(data);
	CloseWindow();
	return 0;
}
