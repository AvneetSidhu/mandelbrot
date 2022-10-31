#include <SDL.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

int WIDTH = 2000; //increase for better resolutions
int HEIGHT = 2000;//increase for better resolutions

long double min = -2.0;
long double max = 2.0;
long double factor = 1;

bool SAVE = false;

int ITERATIONS = 200;

bool ZOOM = true;

long double map(long double value, long double input_min, long double input_max, long double output_min, long double output_max) { // maps value from one range to another range
	return (value - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;

	//std::ifstream settingsFile("settings.txt");

	//std::string myText;

	if (SAVE) {
		SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer); //changes dimensions if the save feature is on to capture frames in true dimensions
	}
	else {
		SDL_CreateWindowAndRenderer(1920, 1080, 0, &window, &renderer);
	}

	SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

	int count = 0;

	while (1) {

		if (ZOOM) { //if zoom feature is enabled then zoom into image
			max -= 0.1 * factor;
			min += 0.15 * factor;
			factor *= 0.9349;
			ITERATIONS += 5;
		}

		SDL_RenderPresent(renderer);

		for (int x = 0; x < WIDTH; x++) { //loops through x axis values

			if (SDL_PollEvent(&event) && event.type == SDL_QUIT) { //exit program
				return 0;
			}

			if (GetKeyState('Q') && 0x8000) { //exit program
				return 0;
			}

			for (int y = 0; y < HEIGHT; y++) { //loops through y axis (imaginary) values

				long double a = map(x, 0, WIDTH, min, max);  // maps value to pixel on screen
				long double b = map(y, 0, HEIGHT, min, max);

				long double ai = a; //saves initial values 
				long double bi = b;

				int n = 0; // will track the number of iterations required to determine if in set

				for (int i = 0; i < ITERATIONS; i++) { //loops to a maximum number of iterations set manually to determine if point is in the set

					long double a1 = a * a - b * b; // equation that results from squaring mandelbrot function to be applicable to real numbers 
					long double b1 = 2 * a * b;

					a = a1 + ai;
					b = b1 + bi;

					if ((a + b) > 2) { //if moving toward infinity 
						break;
					}

					n++; //n tracks the number of iterations needed to determine if the value is in the set
				}

				int brightness = map(n, 0, ITERATIONS, 0, 255); //map number of iterations found from range 0 to ITERATIONS to range 0-255 to get color

				if (n == ITERATIONS || brightness < 20) { //setting color to black in case of very light color or max numbers of iterations reached
					brightness = 0;
				}


				int red = map(brightness * brightness, 0, 255 * 255, 0, 255);
				int blue = map(sqrt(brightness), 0, sqrt(255), 0, 255);
				int green = brightness;

				SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}

		if (SAVE) { // checks if the save feature is enabled, will save each frame of the rendering
			SDL_Surface* sshot = SDL_GetWindowSurface(window); //gets surface of the window on which the rendering is happening
			SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch); //puts window in pixel format
			std::string file = std::to_string(count) + ".bmp"; // creates a file with image name corresponding to the count of the frame being rendered
			SDL_SaveBMP(sshot, file.c_str()); // saves frame to file 
			SDL_FreeSurface(sshot); //frees surface
		}

		count++;
	}

	return 0;
}