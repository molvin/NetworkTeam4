#include "Engine.h"
#include <chrono>

static SDL_Window* Window;
static SDL_Renderer* Renderer;
static bool IsOpen;
static int CurrentFrame;

static std::chrono::high_resolution_clock::time_point LastFrameTime;
static float DeltaTime = 0.0f;

struct InputState{
	bool Pressed;
	int FrameNum;
};
static InputState KeyStates[(unsigned int)Key::MAX];


void engineInit()
{
	SDL_Init(SDL_INIT_VIDEO);
	Window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	IsOpen = true;
	CurrentFrame = 0;
	LastFrameTime = std::chrono::high_resolution_clock::now();
}

void engClear(){
	// Clear for next frame
	SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(Renderer);
}
void engClose(){
	// 'Closes' the window (sets the flag to close, actual closing happens in Destroy
	IsOpen = false;
}
bool engIsOpen()
{
	return IsOpen;
}
void engineUpdate()
{
	CurrentFrame++;

	// Poll window events
	SDL_Event e;
	while (SDL_PollEvent(&e)){
		if (e.type == SDL_QUIT)
			engClose();

		if (e.type == SDL_KEYDOWN){
			// We dont care about repeats
			if (e.key.repeat == 0){
				InputState& state = KeyStates[e.key.keysym.scancode];
				state.Pressed = true;
				state.FrameNum = CurrentFrame;
			}
		}

		if (e.type == SDL_KEYUP){
			InputState& state = KeyStates[e.key.keysym.scancode];
			state.Pressed = false;
			state.FrameNum = CurrentFrame;
		}
	}

	// Calculate next frame delta
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - LastFrameTime).count() * 1e-6f;  // 10^6 microseconds in a second
	LastFrameTime = now;

	// Present SDL renderer
	SDL_RenderPresent(Renderer);
	engClear();

	// Do a small delay so we dont fry the CPU
	SDL_Delay(1);
}

float engDeltaTime()
{
	return DeltaTime;
}

void engDrawRect(int X, int Y, int Width, int Height){
	SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

	SDL_Rect rect = { X, Y, Width, Height };
	SDL_RenderFillRect(Renderer, &rect);
}
void engDrawLine(int X, int Y, int X2, int Y2)
{
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
	SDL_RenderDrawLine(Renderer, X, Y, X2, Y2);
}

// Input
bool engGetKey(Key InKey)
{
	return KeyStates[(int)InKey].Pressed;
}
bool engGetKeyDown(Key InKey)
{
	// Is pressed and changed this frame
	InputState& State = KeyStates[(int)InKey];
	return State.Pressed && State.FrameNum == CurrentFrame;
}