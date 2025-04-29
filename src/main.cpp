#include "raylib.h"
#include "raymath.h"
#include <iostream>

#define WINDOW_WIDTH 1920/2
#define WINDOW_HEIGHT 1080/2

struct Ship
{
};

enum MouseHoverEnum
{
	NO_VALUE,
	MOUSE_HOVER_SUN,
	MOUSE_HOVER_SHIP
};

enum TargetSelectedEnum
{
	NO_TARGET,
	SELECT_SUN,
	SELECT_SHIP
};

int main()
{
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Base");
	SetExitKey(KEY_ESCAPE);

	bool DEBUG_MODE = false;

	MouseHoverEnum MouseHover = NO_VALUE;
	TargetSelectedEnum TargetSelected = NO_TARGET;
	TargetSelectedEnum PreviousTargetSelected = NO_TARGET;

	//float ZoomLevel = 0.5;

	Texture2D SpaceshipTexture = LoadTexture("assets/images/spaceship.png");
	Vector2 ShipPos{100, 100};
	Vector2 ShipPrevPos = ShipPos;
	Vector2 ShipHeading{1, 0};
	float HeadingAngle = 0;
	float ShipTopSpeed = 100;
	float ShipSpeed = 0;
	float ShipAccel = 0;
	float TargetDistance = 0;
	float DistanceTraveled = 0;
	bool ShipSelected = false;
	Rectangle SpacshipRect{ ShipPos.x, ShipPos.y, SpaceshipTexture.width, SpaceshipTexture.height};


	Vector2 BackGroundPos{ 300, 300 };
	Vector2 TargetVector = ShipPos;

	// Camera
	Vector2 CameraPosition{ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
	Vector2 CameraOffset{ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };
	Camera2D Camera = Camera2D();
	Camera.zoom = 1;
	Camera.target = CameraPosition;
	Camera.offset = CameraOffset;
	Vector2 CameraDirection{};
	float CameraSpeed = 100;
	int ZoomDirection = 0;
	float ZoomSpeed = 1;
	int RotateDirection = 0;
	float RotateSpeed = 1;

	while (!WindowShouldClose())
	{
	// Input
		// Mouse Input
		Vector2 MousePos = GetMousePosition();
		bool MOUSE_BUTTON_1_PRESSED = IsMouseButtonPressed(0);
		bool MOUSE_BUTTON_2_PRESSED = IsMouseButtonPressed(1);
		bool MOUSE_BUTTON_3_PRESSED = IsMouseButtonPressed(2);

		// Keyboard Input
		// Camera Input
		bool CAMERA_MOVE_UP = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
		bool CAMERA_MOVE_DOWN = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
		bool CAMERA_MOVE_LEFT = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
		bool CAMERA_MOVE_RIGHT = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
		bool CAMERA_ZOOM_IN = IsKeyDown(KEY_Z);
		bool CAMERA_ZOOM_OUT = IsKeyDown(KEY_X);

		CameraDirection.x = (int(CAMERA_MOVE_LEFT) - int(CAMERA_MOVE_RIGHT));
		CameraDirection.y = (int(CAMERA_MOVE_UP) - int(CAMERA_MOVE_DOWN));
		CameraDirection = Vector2Normalize(CameraDirection);
		ZoomDirection = (int(CAMERA_ZOOM_IN) - int(CAMERA_ZOOM_OUT));
		
		// Debug Input
		if (!DEBUG_MODE)
		{
			DEBUG_MODE = IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_SHIFT);
		}
		else
		{
			DEBUG_MODE = !(IsKeyPressed(KEY_D) && IsKeyDown(KEY_LEFT_SHIFT));
		}

	// Check Timers
		float dt = GetFrameTime();

	// Ai

	// Calculate Positions
		SpacshipRect.width = SpaceshipTexture.width /** ZoomLevel*/;
		SpacshipRect.height = SpaceshipTexture.height /** ZoomLevel*/;
		SpacshipRect.x = ShipPos.x - SpacshipRect.width / 2;
		SpacshipRect.y = ShipPos.y - SpacshipRect.height / 2;

		// Collisions
		
		if (CheckCollisionPointRec(MousePos, SpacshipRect)) { MouseHover = MOUSE_HOVER_SHIP; }
		else if (CheckCollisionPointCircle(MousePos, BackGroundPos, 50 /** ZoomLevel*/)) { MouseHover = MOUSE_HOVER_SUN; }
		else { MouseHover = NO_VALUE; }

	// Updates
		
		if (MOUSE_BUTTON_1_PRESSED)
		{
			PreviousTargetSelected = TargetSelected;
			TargetSelected = (TargetSelectedEnum)MouseHover;
		}

		if (TargetSelected == SELECT_SHIP && MOUSE_BUTTON_2_PRESSED)
		{
			TargetVector = MousePos;
			ShipAccel = ShipTopSpeed / 10;
		}
		
		Vector2 ShipToTargetV = Vector2Subtract(TargetVector, ShipPos);
		ShipHeading = Vector2Normalize(ShipToTargetV);
		TargetDistance = Vector2Length(ShipToTargetV);
		HeadingAngle = Vector2Angle(ShipPos, TargetVector);

		if (ShipSpeed < ShipTopSpeed)
			ShipSpeed += ShipAccel;

		if (TargetDistance <= 5)
		{
			ShipSpeed = 0;
			ShipAccel = 0;
		}

		// Camera Update
		CameraPosition.x -= CameraDirection.x * CameraSpeed * dt;
		CameraPosition.y -= CameraDirection.y * CameraSpeed * dt;
		Camera.zoom += ZoomDirection * ZoomSpeed * dt;
		Camera.target = CameraPosition;
		

		/*BackGroundPos.x += CameraMovement.x;
		BackGroundPos.y += CameraMovement.y;*/
		
		ShipPos.x += ShipHeading.x * ShipSpeed * dt /** ZoomLevel*//* + CameraMovement.x*/;
		ShipPos.y += ShipHeading.y * ShipSpeed * dt /** ZoomLevel*/ /*+ CameraMovement.y*/;

		std::cout << "Pos: (" << ShipPos.x << ", " << ShipPos.y << "), ";
		std::cout << "MPos: (" << MousePos.x << ", " << MousePos.y << "), ";
		std::cout << "Spd: " << ShipSpeed << ", ";
		std::cout <<  "Acl: " << ShipAccel << ", ";
		std::cout <<  "TD: " << TargetDistance << " MouseHover: " << MouseHover << std::endl;

		// Drawing
		BeginDrawing();
		BeginMode2D(Camera);
		ClearBackground(BLACK);
		DrawCircleV(BackGroundPos, 50/**ZoomLevel*/, ORANGE);
		//DrawCircleV(MousePos, 5, WHITE);
		DrawEllipseLines(BackGroundPos.x, BackGroundPos.y, 300/**ZoomLevel*/, 300/** ZoomLevel*/, YELLOW);

		
		Vector2 ShipDrawPos
		{ 
			ShipPos.x - SpaceshipTexture.width / 2 /** ZoomLevel*/,
			ShipPos.y - SpaceshipTexture.height / 2 /** ZoomLevel*/
		};
		DrawTextureEx(SpaceshipTexture, ShipDrawPos, HeadingAngle, 1 /** ZoomLevel*/, WHITE);
		
		if (DEBUG_MODE)
		{
			DrawRectangleRec(SpacshipRect, { 255,255,255,100 });
			DrawFPS(0, 0);
		}
		else if (TargetSelected == SELECT_SHIP)
		{
			DrawRectangleLinesEx(SpacshipRect, 1, YELLOW);
		}
		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}