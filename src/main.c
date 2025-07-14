/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define DARKGRAYALPHA   CLITERAL(Color){ 80, 80, 80, 180 }

#define and &&
#define or	||
#define not !

#define WIDTH 1280
#define HEIGHT 800

#define SHEETROW 3
#define SHEETCOLUMN 3

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1
#define MAX_PLACEABLE_TILES 10

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

typedef struct GameContext
{
	// player texture
	Texture man;

	// player movement
	f32 manx;
	f32 many;
	f32 accelaration;

	// animation data
	Rectangle frame_rec;
	s32 current_frame;
	s32 frame_counter;
	s32 frame_speed;
	
	// tile stuff
	Texture background_tile;
	Texture template_tile;

	Rectangle current_viewable_tile;
	u8 tile_row_count;
	u8 tile_col_count;
	u8 current_tile;
	Vector2 current_tile_position;

	// placeable tile rectangles
	Rectangle placeable_tiles[MAX_PLACEABLE_TILES];
	Vector2 placeable_tile_positions[MAX_PLACEABLE_TILES];
	u16 placed_tiles_count;

	// mouse location
	Vector2 mouse_pos;
} GameContext;

void update(GameContext* ctx)
{
	if (IsKeyDown(KEY_UP) && ctx->accelaration == 0)
	{
		ctx->accelaration = -4.f;
	}
	if (IsKeyDown(KEY_LEFT))
	{
		ctx->manx -= 2;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		ctx->manx += 2;
	}

	// falling
	//physics
	ctx->many += ctx->accelaration;

	if (ctx->many < HEIGHT - ctx->frame_rec.height)
	{
		ctx->accelaration += 0.1f;
	}

	else {
		ctx->accelaration = 0.f;
	}
	
	if (ctx->many > HEIGHT - ctx->frame_rec.height)
	{
		ctx->many = HEIGHT - ctx->frame_rec.height;
	}

	ctx->frame_counter++;

	if (ctx->frame_counter >= (60 / ctx->frame_speed))
	{
		ctx->frame_counter = 0;
		ctx->current_frame++;

		if (ctx->current_frame > SHEETROW*SHEETCOLUMN-1) ctx->current_frame = 0;


		ctx->frame_rec.x = (f32)(ctx->current_frame % SHEETCOLUMN) * (f32)ctx->man.width / SHEETCOLUMN;
		ctx->frame_rec.y = (f32)(ctx->current_frame / SHEETCOLUMN) * (f32)ctx->man.height / SHEETROW;
	}

	s32 mouse_wheel = (s32)GetMouseWheelMove();
	if (ctx->current_tile < (ctx->tile_col_count * ctx->tile_row_count) - 1 and mouse_wheel == -1)
	{
		ctx->current_tile += 1;
	}
	else if(ctx->current_tile > 0 and mouse_wheel == 1)
	{
		ctx->current_tile -= 1;
	}

	ctx->current_viewable_tile.x = (f32)(ctx->current_tile % ctx->tile_col_count) * (f32)ctx->template_tile.width / ctx->tile_col_count;
	ctx->current_viewable_tile.y = (f32)(ctx->current_tile / ctx->tile_col_count) * (f32)ctx->template_tile.height / ctx->tile_row_count;

	ctx->mouse_pos = GetMousePosition();

	ctx->current_tile_position = (Vector2){ (u32)(ctx->mouse_pos.x / 32) * 32, (u32)(ctx->mouse_pos.y / 32) * 32 };
	// Add a tile add the mouse position
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) and not (ctx->placed_tiles_count == MAX_PLACEABLE_TILES)) 
	{
		bool replaced = false;
		for (u16 i = 0; i < ctx->placed_tiles_count; i++)
		{
			// If there is already a placed tile here then just replace it.
			if (ctx->placeable_tile_positions[i].x == ctx->current_tile_position.x and
				ctx->placeable_tile_positions[i].y == ctx->current_tile_position.y)
			{
				ctx->placeable_tiles[i] = ctx->current_viewable_tile;
				replaced = true;
			}
		}

		if (not replaced)
		{
			ctx->placeable_tiles[ctx->placed_tiles_count] = ctx->current_viewable_tile;
			ctx->placeable_tile_positions[ctx->placed_tiles_count] = ctx->current_tile_position;
			ctx->placed_tiles_count++;
		}
	}
	// Remove a tile (if it's there) and update the list
	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) 
	{
		for (u16 i = 0; i < ctx->placed_tiles_count; i++)
		{
			// If there is already a placed tile here then just replace it.
			if (ctx->placeable_tile_positions[i].x == ctx->current_tile_position.x and
				ctx->placeable_tile_positions[i].y == ctx->current_tile_position.y)
			{
				for (u16 u = 0; i + u + 1 < ctx->placed_tiles_count; u++)
				{
					ctx->placeable_tiles[i + u] = ctx->placeable_tiles[i + u + 1];
					ctx->placeable_tile_positions[i + u] = ctx->placeable_tile_positions[i + u + 1];
				}

				ctx->placed_tiles_count--;
			}
		}
	}
}

void draw(GameContext* ctx)
{
	// This stretches the 32x32 tile to the entire width and height of the screen
	DrawTextureRec(ctx->background_tile, (Rectangle) { 0, 0, WIDTH, HEIGHT }, (Vector2) { 0, 0 }, WHITE);

	// Draw grid
	for (u32 i = 0; i < HEIGHT; i += 32)
	{
		DrawLine(0, i, WIDTH, i, RED);
	}
	for (u32 i = 0; i < WIDTH; i += 32)
	{
		DrawLine(i, 0, i, HEIGHT, RED);
	}

	// Draw the tiles that have been placed
	for (u16 i = 0; i < ctx->placed_tiles_count; i++)
	{
		DrawTextureRec(ctx->template_tile, ctx->placeable_tiles[i], ctx->placeable_tile_positions[i], WHITE);
	}

	// Draw tile under mouse
	DrawTextureRec(ctx->template_tile, ctx->current_viewable_tile, ctx->current_tile_position, WHITE);


	// draw our character to the screen
	DrawTextureRec(ctx->man, ctx->frame_rec, (Vector2) { ctx->manx, ctx->many }, WHITE);

	// Draw overlay stuff here
	DrawRectangle(WIDTH - 200, 0, 200, HEIGHT, DARKGRAYALPHA);
	DrawLine(WIDTH - 200, 0, WIDTH - 200, HEIGHT, RAYWHITE);
	
	Vector2 position = (Vector2){ WIDTH - 180, 20 };
	DrawTexturePro(ctx->template_tile, ctx->current_viewable_tile, 
		(Rectangle){ position.x, position.y, fabsf(ctx->current_viewable_tile.width*1.5f), fabsf(ctx->current_viewable_tile.height*1.5f) },
		(Vector2){0.0f, 0.0f}, 0.0f, WHITE);

	DrawFPS(0, 0);

	DrawText(TextFormat("Mouse x: %f, y: %f", ctx->mouse_pos.x, ctx->mouse_pos.y), 0, 20, 20, DARKBLUE);
	DrawText(TextFormat("Current_tile: %d", ctx->current_tile), 0, 40, 20, DARKBLUE);
	DrawText(TextFormat("Placed tiles: %d", ctx->placed_tiles_count), 0, 60, 20, DARKBLUE);
}

int main ()
{
	// Initialize game context
	GameContext* ctx = (GameContext*)malloc(sizeof(GameContext));

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	SetTargetFPS(60);

	// Load a texture from the resources directory
	ctx->man = LoadTexture("sprites/Anim_Robot_Idle1_v1.1_spritesheet.png");
	ctx->manx = 200.f; ctx->many = 400.f; ctx->accelaration = 0.f;

	ctx->current_frame = 0;
	ctx->frame_counter = 0;
	ctx->frame_speed = 10;

	ctx->frame_rec = (Rectangle){ 0.0f, 0.0f, (f32)ctx->man.width / SHEETCOLUMN, (f32)ctx->man.height / SHEETROW };

	ctx->background_tile = LoadTexture("tiles/3_Far_Background_Tile.png");
	ctx->template_tile = LoadTexture("tiles/0_Template_Tileset.png");

	ctx->current_viewable_tile = (Rectangle){ 0.0f, 0.0f, 32.f, 32.f };
	ctx->tile_row_count = (ctx->template_tile.height / 32);
	ctx->tile_col_count = (ctx->template_tile.width / 32);
	ctx->current_tile = 0;

	ctx->placed_tiles_count = 0;
	// game loop
	while (not WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(DARKBLUE);

		//f32 frametime = GetFrameTime();
		update(ctx);
		draw(ctx);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(ctx->man);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}