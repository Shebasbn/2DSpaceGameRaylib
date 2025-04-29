#include "raylib.h"
#include "raymath.h"
#include <iostream>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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
	Vector2 ShipDrawPos = ShipPos;
	Vector2 ShipPrevPos = ShipPos;
	Vector2 ShipHeading{1, 0};
	float HeadingAngle = 0;
	float ShipTopSpeed = 100;
	float ShipSpeed = 0;
	float ShipAccel = 0;
	float TargetDistance = 0;
	float DistanceTraveled = 0;
	bool ShipSelected = false;



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
	float RotateSpeed = 50;

	while (!WindowShouldClose())
	{
	// Input
		// Mouse Input
		Vector2 MousePos = GetMousePosition();
		Vector2 WorldMousePos = GetScreenToWorld2D(MousePos, Camera);
		bool MOUSE_BUTTON_1_PRESSED = IsMouseButtonPressed(0);
		bool MOUSE_BUTTON_2_PRESSED = IsMouseButtonPressed(1);
		bool MOUSE_BUTTON_3_PRESSED = IsMouseButtonPressed(2);

		// Keyboard Input
		// Camera Input
		bool CAMERA_MOVE_UP = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
		bool CAMERA_MOVE_DOWN = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
		bool CAMERA_MOVE_LEFT = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
		bool CAMERA_MOVE_RIGHT = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
		bool CAMERA_ZOOM_IN = IsKeyDown(KEY_PAGE_UP);
		bool CAMERA_ZOOM_OUT = IsKeyDown(KEY_PAGE_DOWN);
		bool CAMERA_ROTATE_LEFT = IsKeyDown(KEY_Q);
		bool CAMERA_ROTATE_RIGHT = IsKeyDown(KEY_E);

		CameraDirection.x = (int(CAMERA_MOVE_LEFT) - int(CAMERA_MOVE_RIGHT));
		CameraDirection.y = (int(CAMERA_MOVE_UP) - int(CAMERA_MOVE_DOWN));
		CameraDirection = Vector2Normalize(CameraDirection);
		ZoomDirection = (int(CAMERA_ZOOM_IN) - int(CAMERA_ZOOM_OUT));
		RotateDirection = (int(CAMERA_ROTATE_RIGHT) - int(CAMERA_ROTATE_LEFT));
		
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

		Rectangle ShipRect;
		ShipRect.width = SpaceshipTexture.width;
		ShipRect.height = SpaceshipTexture.height;
		ShipRect.x = ShipPos.x;
		ShipRect.y = ShipPos.y;

		// Collisions
		
		if (CheckCollisionPointRec(WorldMousePos, ShipRect)) { MouseHover = MOUSE_HOVER_SHIP; }
		else if (CheckCollisionPointCircle(WorldMousePos, BackGroundPos, 50)) { MouseHover = MOUSE_HOVER_SUN; }
		else { MouseHover = NO_VALUE; }

	// Updates
		
		if (MOUSE_BUTTON_1_PRESSED)
		{
			PreviousTargetSelected = TargetSelected;
			TargetSelected = (TargetSelectedEnum)MouseHover;
		}

		if (TargetSelected == SELECT_SHIP && MOUSE_BUTTON_2_PRESSED)
		{
			TargetVector = WorldMousePos;
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
		Camera.zoom = MAX(0.2, MIN(5, Camera.zoom));
		Camera.target = CameraPosition;
		Camera.rotation += RotateDirection * RotateSpeed * dt;
		

		/*BackGroundPos.x += CameraMovement.x;
		BackGroundPos.y += CameraMovement.y;*/
		
		ShipPos.x += ShipHeading.x * ShipSpeed * dt/* * Camera.zoom*/ /** + CameraMovement.x*/;
		ShipPos.y += ShipHeading.y * ShipSpeed * dt/* * Camera.zoom*/ /*+ CameraMovement.y*/;

		std::cout << "Pos: (" << ShipPos.x << ", " << ShipPos.y << "), ";
		std::cout << "WMPos: (" << WorldMousePos.x << ", " << WorldMousePos.y << "), ";
		std::cout << "SMPos: (" << MousePos.x << ", " << MousePos.y << "), ";
		std::cout << "Spd: " << ShipSpeed << ", ";
		std::cout <<  "Acl: " << ShipAccel << ", ";
		std::cout <<  "TD: " << TargetDistance << " MouseHover: " << MouseHover << std::endl;

		// Drawing
		BeginDrawing();
		BeginMode2D(Camera);
		ClearBackground(BLACK);
		DrawCircleV(BackGroundPos, 50, ORANGE);
		//DrawCircleV(MousePos, 5, WHITE);
		DrawEllipseLines(BackGroundPos.x, BackGroundPos.y, 300, 300, YELLOW);

		
		/*Vector2 ShipDrawPos
		{ 
			ShipPos.x - SpaceshipTexture.width / 2,
			ShipPos.y - SpaceshipTexture.height / 2
		};*/

		ShipDrawPos = GetWorldToScreen2D(ShipPos, Camera);
		//Rectangle SpaceshipRectDraw{ShipDrawPos.x, ShipDrawPos.y, SpaceshipTexture.width * Camera.zoom, SpaceshipTexture.height * Camera.zoom };
		DrawTextureEx(SpaceshipTexture, ShipPos, HeadingAngle, 1, WHITE);
		
		if (DEBUG_MODE)
		{
			DrawRectangleRec(ShipRect, { 255,255,255,100 });
			DrawFPS(0, 0);
		}
		else if (TargetSelected == SELECT_SHIP)
		{
			DrawRectangleLinesEx(ShipRect, 1, YELLOW);
		}
		else if (TargetSelected == SELECT_SUN)
		{
			DrawCircleLinesV(BackGroundPos, 50, WHITE);
		}
		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}