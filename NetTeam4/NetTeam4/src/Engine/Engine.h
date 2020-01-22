#pragma once
#include "SDL/SDL.h"
#include "Key.h"

void engineInit();

void engineUpdate();

void engClose();

void engClear();

bool engIsOpen();

//Rendering
void engDrawRect(int X, int Y, int Width, int Height);
void engDrawLine(int X, int Y, int X2, int Y2);

//Input
bool engGetKey(Key InKey);
bool engGetKeyDown(Key InKey);
