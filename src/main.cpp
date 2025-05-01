#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <math.h>


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define SCREEN_CENTER_X WINDOW_WIDTH/2
#define SCREEN_CENTER_Y WINDOW_HEIGHT/2

#define PIXEL_PERCENT_OF_SCREEN(n) WINDOW_WIDTH * n/100.0f
#define INITIAL_VALUE 500.0f / (PIXEL_PERCENT_OF_SCREEN(10.0f))
#define GROWTH_FACTOR 1024.0f

#define NUM_OF_BODIES 9



enum MouseHoverState
{
	NO_VALUE = -1,
	MOUSE_HOVER_SHIP = NUM_OF_BODIES
};

//MouseHoverState CheckMouseHoverState(Body bodies[NUM_OF_BODIES])
//{
//	MouseHoverState result = NO_VALUE;
//	for (int i = 0; i < NUM_OF_BODIES; i++)
//	{
//		result = CheckCollisionPointRec(WorldMousePos, ShipRect) ? (MouseHoverState)i : result;
//	}
//}

enum TargetSelectedState
{
	NO_TARGET = -1,
	//SELECT_SUN,
	SELECT_SHIP = NUM_OF_BODIES
};


struct Timer
{
	float duration;
	float startTime;
	uint32_t CyclesAchieved;
	bool active;
	bool repeat;
};

void StartTimer(Timer& timer)
{
	timer.active = true;
	timer.startTime = GetTime();
}
uint32_t EndTimer(Timer& timer)
{
	timer.CyclesAchieved++;
	timer.active = false;
	timer.startTime = 0;
	if (timer.repeat)
	{
		StartTimer(timer);
	}

	return timer.CyclesAchieved;
}

void UpdateTimer(Timer& timer)
{
	if (timer.active)
	{
		if (GetTime() - timer.startTime >= timer.duration)
		{
			EndTimer(timer);
		}
	}
}

Timer CreateTimer(float duration, bool repeat = false, bool autostart = false)
{
	Timer timer{};
	timer.duration = duration;
	timer.startTime = 0;
	timer.repeat = 0;
	timer.CyclesAchieved = 0;

	if (autostart)
	{
		StartTimer(timer);
	}

	return timer;
}

void PrintSolarScale(float scale)
{
	float output = scale;

	if (scale < 1000 && scale > -1000)
		std::cout << scale;
	else if (scale < 1000000 && scale > -10000000)
		std::cout << scale / 1000 << "kkm";
	else if (scale < 1'000'000'000 && scale > -1'000'000'000)
		std::cout << scale / 1'000'000 << "mkm";
	else if (scale < 1'000'000'000'000 && scale > -1'000'000'000'000)
		std::cout << scale / 1'000'000'000 << "bkm";
	else if (scale < 9460730467457.2792969 && scale > -9460730467457.2792969)
		std::cout << scale / 1'000'000'000'000 << "tkm";
	else
		std::cout << scale / 9460730467457.2792969 << "ly";
}

struct Body
{
	std::string Name;
	Vector2 Position;
	uint32_t ParentBodyID;
	float DistanceFromParentBody;
	float RadiusInkm;
	float SizeRelativeToStar;
	float DeltaAngle;
	Color Color;
};

//void CalculateBodyPosition(Body bodies[NUM_OF_BODIES], float scale, float dt)
//{
//	//body.Angle = RAD_PER_DEG * 1 * dt / scale;
//	for (int i = 0; i < NUM_OF_BODIES; i++)
//	{
//		bodies[i].Position = Vector2Normalize(Vector2Rotate(bodies[i].Position, bodies[i].DeltaAngle * dt * 100000));
//		bodies[i].Position *= bodies[i].DistanceFromParentBody/scale/Camera.zoom;
//	}
//}

int main()
{


	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Base");
	//ToggleFullscreen();
	SetExitKey(KEY_ESCAPE);

	bool DEBUG_MODE = false;

	MouseHoverState MouseHover = NO_VALUE;
	TargetSelectedState TargetSelected = NO_TARGET;
	TargetSelectedState PreviousTargetSelected = NO_TARGET;

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

	Body Bodies[NUM_OF_BODIES]{};


	//Body Sun{};
	Bodies[0].Name = "Sun";
	Bodies[0].RadiusInkm = MIN(696342.0f, 100);
	Bodies[0].SizeRelativeToStar = 1.0f;
	Bodies[0].Color = { 252, 150, 1, 255};

	Bodies[1].Name = "Mercury";
	Bodies[1].DistanceFromParentBody = 57.9f * 1000000.0f;
	Bodies[1].RadiusInkm = 2440.0f;
	Bodies[1].SizeRelativeToStar = 0.003504f;
	Bodies[1].Position = { Bodies[Bodies[1].ParentBodyID].Position.x + Bodies[1].DistanceFromParentBody, Bodies[Bodies[1].ParentBodyID].Position.y};
	Bodies[1].DeltaAngle = 8.26387E-07;
	Bodies[1].Color = { 151, 151, 159, 255 };

	Bodies[2].Name = "Venus";
	Bodies[2].DistanceFromParentBody = 108.2f * 1000000.0f;
	Bodies[2].RadiusInkm = 6052.0f;
	Bodies[2].SizeRelativeToStar = 0.008691f;
	Bodies[2].Position = { Bodies[Bodies[2].ParentBodyID].Position.x + Bodies[2].DistanceFromParentBody, Bodies[Bodies[2].ParentBodyID].Position.y };
	Bodies[2].DeltaAngle = 0/*3.23641E-07*/;
	Bodies[2].Color = { 193,143,23, 255 };

	Bodies[3].Name = "Earth";
	Bodies[3].DistanceFromParentBody = 149.6f * 1000000.0f;
	Bodies[3].RadiusInkm = 6371.0f;
	Bodies[3].SizeRelativeToStar = 0.009149f;
	Bodies[3].Position = { Bodies[Bodies[3].ParentBodyID].Position.x + Bodies[3].DistanceFromParentBody, Bodies[Bodies[3].ParentBodyID].Position.y };
	Bodies[3].DeltaAngle = 1.99129E-07;
	Bodies[3].Color = { 159,193,100, 255 };

	Bodies[4].Name = "Mars";
	Bodies[4].DistanceFromParentBody = 228.0f * 1000000.0f;
	Bodies[4].RadiusInkm = 3390.0f;
	Bodies[4].SizeRelativeToStar = 0.004868f;
	Bodies[4].Position = { Bodies[Bodies[4].ParentBodyID].Position.x + Bodies[4].DistanceFromParentBody, Bodies[Bodies[4].ParentBodyID].Position.y };
	Bodies[4].DeltaAngle = 1.05855E-07;
	Bodies[4].Color = { 226, 123, 88, 255 };

	Bodies[5].Name = "Jupiter";
	Bodies[5].DistanceFromParentBody = 778.5f * 1000000.0f;
	Bodies[5].RadiusInkm = 69911.0f;
	Bodies[5].SizeRelativeToStar = 0.100398f;
	Bodies[5].Position = { Bodies[Bodies[5].ParentBodyID].Position.x + Bodies[5].DistanceFromParentBody, Bodies[Bodies[5].ParentBodyID].Position.y };
	Bodies[5].DeltaAngle = 1.67911E-08;
	Bodies[5].Color = { 144, 97, 77, 255 };

	Bodies[6].Name = "Saturn";
	Bodies[6].DistanceFromParentBody = 1432.0f * 1000000.0f;
	Bodies[6].RadiusInkm = 58232.0f;
	Bodies[6].SizeRelativeToStar = 0.083626f;
	Bodies[6].Position = { Bodies[Bodies[6].ParentBodyID].Position.x + Bodies[6].DistanceFromParentBody, Bodies[Bodies[6].ParentBodyID].Position.y };
	Bodies[6].DeltaAngle = 6.76673E-09;
	Bodies[6].Color = { 164, 155, 114, 255 };

	Bodies[7].Name = "Uranus";
	Bodies[7].DistanceFromParentBody = 2867.0f * 1000000.0f;
	Bodies[7].RadiusInkm = 69911.0f;
	Bodies[7].SizeRelativeToStar = 0.036422f;
	Bodies[7].Position = { Bodies[Bodies[7].ParentBodyID].Position.x + Bodies[7].DistanceFromParentBody, Bodies[Bodies[7].ParentBodyID].Position.y };
	Bodies[7].DeltaAngle = 2.37739E-09;
	Bodies[7].Color = { 187, 225, 228, 255 };

	Bodies[8].Name = "Neptune";
	Bodies[8].DistanceFromParentBody = 4515.0f * 1000000.0f;
	Bodies[8].RadiusInkm = 24622.0f;
	Bodies[8].SizeRelativeToStar = 0.035359f;
	Bodies[8].Position = { Bodies[Bodies[8].ParentBodyID].Position.x + Bodies[8].DistanceFromParentBody, Bodies[Bodies[8].ParentBodyID].Position.y };
	Bodies[8].DeltaAngle = 1.21609E-09;
	Bodies[8].Color = { 96, 129, 255, 255 };


	//Vector2 BackGroundPos{ 300, 300 };
	Vector2 TargetVector = ShipPos;

	// Camera
	Vector2 CameraPosition{ 0, 0 };
	Vector2 CameraOffset{ SCREEN_CENTER_X, SCREEN_CENTER_Y };
	Camera2D Camera = Camera2D();
	Camera.zoom = 8;
	Camera.target = CameraPosition;
	Camera.offset = CameraOffset;
	Vector2 CameraDirection{};
	float CameraSpeed = 100;
	int ZoomDirection = 0;
	float ZoomSpeed = 0.1;
	int RotateDirection = 0;
	float RotateSpeed = 50;
	float MaxZoom = 10;

	float Scale;

	float GameTime = GetTime();
	while (!WindowShouldClose())
	{
	// Input
		// Mouse Input
		Vector2 WorldMousePos = GetMousePosition() - Camera.offset + Camera.target / Scale / Camera.zoom;
		WorldMousePos = Vector2Add(WorldMousePos, Camera.target * Camera.zoom);
		WorldMousePos.y += 20 ;
		Vector2 MousePos = GetMousePosition();
		Vector2 TMousePos = GetScreenToWorld2D(MousePos, Camera);
		bool MOUSE_BUTTON_1_PRESSED = IsMouseButtonPressed(0);
		bool MOUSE_BUTTON_2_PRESSED = IsMouseButtonPressed(1);
		bool MOUSE_BUTTON_3_PRESSED = IsMouseButtonPressed(2);

		// Keyboard Input
		// Camera Input
		bool CAMERA_MOVE_UP = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
		bool CAMERA_MOVE_DOWN = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
		bool CAMERA_MOVE_LEFT = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
		bool CAMERA_MOVE_RIGHT = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
		bool CAMERA_ZOOM_IN = IsKeyPressed(KEY_PAGE_UP);
		bool CAMERA_ZOOM_OUT = IsKeyPressed(KEY_PAGE_DOWN);
		//bool CAMERA_ROTATE_LEFT = IsKeyDown(KEY_Q);
		//bool CAMERA_ROTATE_RIGHT = IsKeyDown(KEY_E);

		CameraDirection.x = (int(CAMERA_MOVE_LEFT) - int(CAMERA_MOVE_RIGHT));
		CameraDirection.y = (int(CAMERA_MOVE_UP) - int(CAMERA_MOVE_DOWN));
		CameraDirection = Vector2Normalize(CameraDirection);
		ZoomDirection = (int(CAMERA_ZOOM_IN) - int(CAMERA_ZOOM_OUT));
		//RotateDirection = (int(CAMERA_ROTATE_RIGHT) - int(CAMERA_ROTATE_LEFT));
		
		// Debug Input
		if (!DEBUG_MODE)
		{
			DEBUG_MODE = IsKeyPressed(KEY_F5);
		}
		else
		{
			DEBUG_MODE = !(IsKeyPressed(KEY_F5));
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


		// Camera Poisition
		CameraPosition.x -= CameraDirection.x * CameraSpeed * dt;
		CameraPosition.y -= CameraDirection.y * CameraSpeed * dt;
		Camera.zoom += ZoomDirection * ZoomSpeed;
		Camera.zoom = Clamp(Camera.zoom, 0.1f, 9.9f);
		Scale = INITIAL_VALUE * powf(GROWTH_FACTOR, MaxZoom - Camera.zoom);
		Camera.target = CameraPosition;
		Camera.rotation += RotateDirection * RotateSpeed * dt;

		// Calculate Body Positions
		//CalculateBodyPosition(Bodies, Scale, dt);
		for (int i = 0; i < NUM_OF_BODIES; i++)
		{
			Bodies[i].Position = Vector2Normalize(Vector2Rotate(Bodies[i].Position, Bodies[i].DeltaAngle * dt * 100000));
			Bodies[i].Position *= Bodies[i].DistanceFromParentBody / Scale;
		}

		std::cout << "Mercury Position: ";
		PrintSolarScale(Vector2Length(Bodies[1].Position) * Scale);
		std::cout << ", Camera Position: " << Camera.target.x << ", " << Camera.target.y << "\n";

		// Calc Ship Pos
		ShipPos.x += ShipHeading.x * ShipSpeed * dt;
		ShipPos.y += ShipHeading.y * ShipSpeed * dt;

		// Collisions
		if (CheckCollisionPointRec(WorldMousePos, ShipRect)) { MouseHover = MOUSE_HOVER_SHIP; }
		else if (MouseHover == NO_VALUE || !CheckCollisionPointCircle(WorldMousePos, Bodies[MouseHover].Position, 10/*Sun.RadiusInkm*/))
		{
			for (int i = 0; i < NUM_OF_BODIES; i++)
			{
				if (CheckCollisionPointCircle(WorldMousePos, Bodies[i].Position, 10/*Sun.RadiusInkm*/)) 
				{ 
					MouseHover = (MouseHoverState)i; 
					std::cout << "MouseHoverID: " << MouseHover << std::endl;
					break;
				}
				MouseHover = NO_VALUE;
			}
		}

	// Updates
		
		if (MOUSE_BUTTON_1_PRESSED)
		{
			PreviousTargetSelected = TargetSelected;
			TargetSelected = (TargetSelectedState)MouseHover;
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

		std::cout << "Venus: (" << Bodies[2].Position.x << ", " << Bodies[2].Position.y << "), ";
		std::cout << "WMPos: (" << WorldMousePos.x << ", " << WorldMousePos.y << "), ";
		std::cout << "TWMPos: (" << TMousePos.x << ", " << TMousePos.y << "), ";
		std::cout << "SMPos: (" << MousePos.x << ", " << MousePos.y << "), ";
		std::cout << "Spd: " << ShipSpeed << ", ";
		std::cout <<  "Acl: " << ShipAccel << ", ";
		std::cout <<  "TD: " << TargetDistance << " MouseHover: " << MouseHover << std::endl;
		std::cout << "dt: " << dt * 1000.0 << "ms, GameTime: " << GameTime << "s, ZoomLevel: " << MaxZoom - Camera.zoom << ", Scale: ";
		PrintSolarScale(Scale * PIXEL_PERCENT_OF_SCREEN(10.0f));
		std::cout << std::endl;
		//std::cout << "Scale to line of " << Vector2Length({ PIXEL_PERCENT_OF_SCREEN(10), 0.0f }) << "px" << std::endl;

		// Drawing
		BeginDrawing();
		BeginMode2D(Camera);
		ClearBackground(BLACK);

		for (int i = NUM_OF_BODIES - 1; i >= 0; i--)
		{
			DrawEllipseLines(
				Bodies[0].Position.x, Bodies[0].Position.y, 
				Bodies[i].DistanceFromParentBody/Scale/Camera.zoom, 
				Bodies[i].DistanceFromParentBody/Scale/Camera.zoom, 
				YELLOW);
			DrawCircleV(Bodies[i].Position/Camera.zoom, 10/Camera.zoom, Bodies[i].Color);
		}

		/*DrawEllipseLines(Sun.Position.x, Sun.Position.y, Mercury.DistanceFromParentBody / Scale / Camera.zoom, Mercury.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Venus.DistanceFromParentBody / Scale / Camera.zoom, Venus.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Earth.DistanceFromParentBody / Scale / Camera.zoom, Earth.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Mars.DistanceFromParentBody / Scale / Camera.zoom, Mars.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Jupiter.DistanceFromParentBody / Scale / Camera.zoom, Jupiter.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Saturn.DistanceFromParentBody / Scale / Camera.zoom, Saturn.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Uranus.DistanceFromParentBody / Scale / Camera.zoom, Uranus.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);
		DrawEllipseLines(Sun.Position.x, Sun.Position.y, Neptune.DistanceFromParentBody / Scale / Camera.zoom, Neptune.DistanceFromParentBody / Scale / Camera.zoom, YELLOW);*/


		////DrawBodies(Bodies, Camera);
		//DrawCircleV(Mercury.Position / Camera.zoom, 10 / Camera.zoom, GRAY);
		//DrawCircleV(Venus.Position / Camera.zoom, 10 / Camera.zoom, BROWN);
	 // 	DrawCircleV(Earth.Position / Camera.zoom, 10 / Camera.zoom, BLUE);
		//DrawCircleV(Mars.Position / Camera.zoom, 10 / Camera.zoom, RED);
		//DrawCircleV(Jupiter.Position / Camera.zoom, 10 / Camera.zoom, DARKBROWN);
		//DrawCircleV(Saturn.Position / Camera.zoom, 10 / Camera.zoom, GREEN);
		//DrawCircleV(Uranus.Position / Camera.zoom, 10 / Camera.zoom, BLUE);
		//DrawCircleV(Neptune.Position / Camera.zoom, 10 / Camera.zoom, DARKBLUE);
		//DrawCircleV(Sun.Position / Camera.zoom, 10/Camera.zoom, ORANGE);
		//DrawCircleV(MousePos, 5, WHITE);
		DrawLineV({ 0.0f, 0.0f }, { PIXEL_PERCENT_OF_SCREEN(10) / Camera.zoom, 0.0f }, YELLOW);
		//DrawLineV({ 0.0f, 0.0f }, Vector2Rotate({ PIXEL_PERCENT_OF_SCREEN(10) / Camera.zoom, 0.0f }, 1.5708), YELLOW);

		ShipDrawPos = GetWorldToScreen2D(ShipPos, Camera);
		//Rectangle SpaceshipRectDraw{ShipDrawPos.x, ShipDrawPos.y, SpaceshipTexture.width * Camera.zoom, SpaceshipTexture.height * Camera.zoom };
		DrawTextureEx(SpaceshipTexture, ShipPos, HeadingAngle, 1, WHITE);
		
		if (DEBUG_MODE)
		{
			DrawRectangleRec(ShipRect, { 255,255,255,100 });
			DrawFPS(0, 0);
		}

		switch (TargetSelected)
		{
		case NO_TARGET:
			break;
		case SELECT_SHIP:
			DrawRectangleLinesEx(ShipRect, 1, YELLOW);
			break;
		default:
			for (int i = 0; i < NUM_OF_BODIES; i++)
			{
				if (TargetSelected == (TargetSelectedState)i)
				{
					DrawCircleLinesV(Bodies[i].Position / Camera.zoom, 10 / Camera.zoom/*Sun.RadiusInkm*/, WHITE);
					break;
				}
			}
			break;
		}

		//else if (TargetSelected == SELECT_SHIP)
		//{
		//	DrawRectangleLinesEx(ShipRect, 1, YELLOW);
		//}
		//else if (TargetSelected == SELECT_SUN)
		//{
		//	DrawCircleLinesV(Bodies[0].Position, 10/Camera.zoom/*Sun.RadiusInkm*/, WHITE);
		//}
		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
}