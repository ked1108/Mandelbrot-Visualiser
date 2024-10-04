#include "raylib.h"
#include <complex.h>
#include <omp.h>
#include <stdlib.h>

Color Mandelbrot(double real, double imag, int maxIterations) {
	double complex c = real + imag * I;  // Create the complex number
	double complex z = 0;
	int iterations = 0;

	while (iterations < maxIterations && cabs(z) <= 2.0) {
		z = z * z + c;  // Mandelbrot iteration: z = z^2 + c
		iterations++;
	}

	if (iterations == maxIterations) return BLACK;  // Inside the set, color black

	float hue = (float)iterations / maxIterations * 360.0f;
	float saturation = 1.0f;  // Full saturation
	float value = iterations < maxIterations ? 1.0f : 0.0f;  // Adjust brightness based on iterations

	return ColorFromHSV(hue, saturation, value);
}

int main(void)
{
	const int screenWidth = 1080;
	const int screenHeight = 720;

	double minReal = -2.5, maxReal = 1.5;
	double minImag = -1.5, maxImag = 1.5;
	int maxIterations = 500;

	Color *colorBuffer = (Color *)malloc(screenWidth * screenHeight * sizeof(Color));
	double zoomFactor = 1.0;
	double offsetX = 0.0, offsetY = 0.0;

	InitWindow(screenWidth, screenHeight, "Mandlebrot Set");
	SetTargetFPS(60);

	while (!WindowShouldClose())
	{

		Vector2 mouseWheel = GetMouseWheelMoveV();
		if (mouseWheel.y != 0) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();

			// Map mouse position to the complex plane
			double real = minReal + (mouseX / (double)screenWidth) * (maxReal - minReal);
			double imag = minImag + (mouseY / (double)screenHeight) * (maxImag - minImag);

			// Adjust the zoom factor and offsets
			if (mouseWheel.y > 0) {
				zoomFactor *= 0.9;  // Zoom in
			} else {
				zoomFactor *= 1.1;  // Zoom out
				if(zoomFactor > 10.0) zoomFactor = 10.0;
			}

			// Update the complex plane bounds based on zoom factor and mouse position
			double zoomWidth = (maxReal - minReal) * zoomFactor;
			double zoomHeight = (maxImag - minImag) * zoomFactor;

			minReal = real - zoomWidth / 2.0;
			maxReal = real + zoomWidth / 2.0;
			minImag = imag - zoomHeight / 2.0;
			maxImag = imag + zoomHeight / 2.0;
		}
		if (IsKeyPressed(KEY_R)) {
            // Reset the zoom factor and bounds to their initial values
            zoomFactor = 1.0;
            minReal = -2.5;
            maxReal = 1.5;
            minImag = -1.5;
            maxImag = 1.5;
        }
		// Handle WASD movement
		if (IsKeyDown(KEY_W)) offsetY -= 0.01;  // Move up
		if (IsKeyDown(KEY_S)) offsetY += 0.01;  // Move down
		if (IsKeyDown(KEY_A)) offsetX -= 0.01;  // Move left
		if (IsKeyDown(KEY_D)) offsetX += 0.01;  // Move right

		// Update the complex plane bounds based on the offset
		double width = maxReal - minReal;
		double height = maxImag - minImag;
		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {
            minImag += offsetY * height;  // Update minImag and maxImag
            maxImag += offsetY * height;
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
            minReal += offsetX * width;  // Update minReal and maxReal
            maxReal += offsetX * width;
        }

		BeginDrawing();
		ClearBackground(BLACK);

		#pragma omp parallel for
		for (int x = 0; x < screenWidth; x++) {
			for (int y = 0; y < screenHeight; y++) {
				double real = minReal + (x / (double)screenWidth) * (maxReal - minReal);
				double imag = minImag + (y / (double)screenHeight) * (maxImag - minImag);

				colorBuffer[y * screenWidth + x] = Mandelbrot(real, imag, maxIterations);
			}
		}


		for(int x = 0; x < screenWidth; x++) {
			for(int y = 0; y < screenHeight; y++) {
				DrawPixel(x, y, colorBuffer[y*screenWidth + x]);
			}
		}

		EndDrawing();
	}
	free(colorBuffer);
	CloseWindow();

	return 0;
}
