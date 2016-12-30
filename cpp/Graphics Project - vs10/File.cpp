#include <math.h>
#include "CMUgraphicsLib\CMUgraphics.h"	
#include "CMUgraphicsLib\auxil.h"

using namespace std;

void drawBackground(window &win);
void drawGround(window &win);
void drawRoad(window &win);
void drawSun(window &win, int posX, int posY, int radius);
void drawClouds(window &win);

int main(void)
{
	window w(1000, 700);
	drawBackground(w);
	drawGround(w);
	drawRoad(w);
	drawSun(w, 55, 40, 10);
	drawSun(w, 122, 75, 15);
	drawSun(w, 55, 113, 50);
	drawClouds(w);
	return 0;
}

void drawBackground(window &win)
{
	win.SetBrush(BLUE);
	win.DrawRectangle(0, 0, 1000, 700);
}

void drawGround(window &win)
{
	win.SetBrush(GREEN);
	win.DrawRectangle(0, 550, 1000, 700);
}

void drawRoad(window &win)
{
	win.SetBrush(GRAY);
	win.DrawRectangle(0, 550, 1000, 450);

	win.SetBrush(YELLOW);
	for ( int i = 1000 ; i != -1 ; i-- )
	{
		if (!(i % 99))
		{
			win.DrawRectangle(i, 505, (i + 50), 495);
		}
	}
}

void drawSun(window &win, int posX, int posY, int radius)
{
	win.SetBrush(YELLOW);
	win.DrawCircle(posX, posY, radius);
}

void drawClouds(window &win)
{
	win.SetBrush(WHITE);
	int posx[] = {300, 500, 700, 200, 450, 800, 900};
	int posy[] = {100, 300, 200, 250, 150, 100, 300};
	int rad = 20;

	for ( int i = 0 ; i != 7 ; i++ )
	{
		win.DrawCircle(posx[i], posy[i], rad);
		win.DrawCircle((posx[i] + rad), (posy[i] + rad), rad);
		win.DrawCircle((posx[i] + rad), (posy[i] - rad), rad);
		win.DrawCircle((posx[i] - rad), (posy[i] + rad), rad);
		win.DrawCircle((posx[i] - rad), (posy[i] - rad), rad);
		win.DrawCircle((posx[i] + rad), (posy[i]), rad);
		win.DrawCircle((posx[i]), (posy[i] + rad), rad);
		win.DrawCircle((posx[i]), (posy[i] - rad), rad);
		win.DrawCircle((posx[i] - rad), (posy[i]), rad);
	}
}