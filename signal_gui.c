#include "raylib.h"
#include <complex.h>
#include <math.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 650
#define SAMPLE_RATE 44100
#define FFT_SIZE 128

typedef double complex cplx;

void fft(cplx buf[], cplx out[], int n, int step) {
  if (step < n) {
    fft(out, buf, n, step * 2);
    fft(out + step, buf + step, n, step * 2);
    for (int i = 0; i < n; i += 2 * step) {
      cplx t = cexp(-I * PI * i / n) * out[i + step];
      buf[i / 2] = out[i] + t;
      buf[(i + n) / 2] = out[i] - t;
    }
  }
}

double low_pass_filter(double input, double prev_output, double alpha) {
  return alpha * input + (1.0 - alpha) * prev_output;
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
             "SignalForge GUI - Real-Time DSP Workbench");
  SetTargetFPS(60);

  cplx wave[FFT_SIZE];
  cplx scratch[FFT_SIZE];
  double time_domain[FFT_SIZE];
  double magnitudes[FFT_SIZE / 2];

  double t = 0.0;
  double dt = 1.0 / SAMPLE_RATE;
  double lpf_out = 0.0;
  float alpha = 0.3f;
  bool is_dragging_slider = false;

  while (!WindowShouldClose()) {
    // --- Input Handling ---
    Vector2 mouse_pos = GetMousePosition();
    Rectangle slider_rect = {180, 585, 300, 20};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(mouse_pos, slider_rect)) {
      is_dragging_slider = true;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      is_dragging_slider = false;
    }
    if (is_dragging_slider) {
      alpha = (mouse_pos.x - slider_rect.x) / slider_rect.width;
      if (alpha < 0.01f)
        alpha = 0.01f;
      if (alpha > 1.00f)
        alpha = 1.00f;
    }

    // --- DSP Calculations ---
    for (int i = 0; i < FFT_SIZE; i++) {
      double raw_sig = sin(2 * PI * 440 * t) + 0.6 * sin(2 * PI * 2200 * t) +
                       0.3 * sin(2 * PI * 4000 * t);
      lpf_out = low_pass_filter(raw_sig, lpf_out, alpha);

      time_domain[i] = lpf_out;
      wave[i] = lpf_out;
      scratch[i] = lpf_out;
      t += dt;
    }

    fft(wave, scratch, FFT_SIZE, 1);

    for (int i = 0; i < FFT_SIZE / 2; i++) {
      magnitudes[i] = cabs(wave[i]) / (FFT_SIZE / 2);
    }

    // --- GUI Rendering ---
    BeginDrawing();
    ClearBackground((Color){15, 17, 26, 255});

    DrawText("SignalForge DSP Workbench", 30, 20, 22, RAYWHITE);
    DrawText("C + Raylib Real-Time Visualizer", 30, 48, 14, GRAY);

    // 1. Time-Domain Oscilloscope Panel
    Rectangle scope_box = {30, 80, 940, 220};
    DrawRectangleRec(scope_box, (Color){22, 26, 38, 255});
    DrawRectangleLinesEx(scope_box, 1, (Color){45, 55, 72, 255});
    DrawText("Oscilloscope (Time Domain)", 45, 92, 14, SKYBLUE);
    DrawLine(scope_box.x, scope_box.y + 110, scope_box.x + scope_box.width,
             scope_box.y + 110, (Color){40, 48, 62, 255});

    for (int i = 0; i < FFT_SIZE - 1; i++) {
      float x1 = scope_box.x + (i * (scope_box.width / (float)FFT_SIZE));
      float y1 = scope_box.y + 110 - (float)(time_domain[i] * 70.0);
      float x2 = scope_box.x + ((i + 1) * (scope_box.width / (float)FFT_SIZE));
      float y2 = scope_box.y + 110 - (float)(time_domain[i + 1] * 70.0);

      DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 2.0f, GREEN);
    }

    // 2. FFT Frequency Spectrum Panel
    Rectangle fft_box = {30, 320, 940, 220};
    DrawRectangleRec(fft_box, (Color){22, 26, 38, 255});
    DrawRectangleLinesEx(fft_box, 1, (Color){45, 55, 72, 255});
    DrawText("FFT Spectrum Analyzer (Frequency Domain)", 45, 332, 14, PURPLE);

    int num_bars = FFT_SIZE / 2;
    float bar_width = (fft_box.width - 40) / (float)num_bars;

    for (int i = 0; i < num_bars; i++) {
      float bar_height = (float)(magnitudes[i] * 150.0);
      if (bar_height > 180.0f)
        bar_height = 180.0f;

      float x = fft_box.x + 20 + (i * bar_width);
      float y = fft_box.y + 200 - bar_height;

      Color bar_color = (i < 10) ? GREEN : (i < 25) ? YELLOW : RED;
      DrawRectangle((int)x, (int)y, (int)bar_width - 2, (int)bar_height,
                    bar_color);
    }

    // 3. Control Panel (Low-Pass Filter Slider)
    DrawRectangle(30, 560, 940, 60, (Color){22, 26, 38, 255});
    DrawText("LPF Cutoff (Alpha):", 45, 587, 14, RAYWHITE);
    DrawRectangleRec(slider_rect, (Color){45, 55, 72, 255});

    float handle_x = slider_rect.x + (alpha * slider_rect.width);
    DrawRectangle((int)handle_x - 5, (int)slider_rect.y - 4, 10, 28, SKYBLUE);
    DrawText(TextFormat("%.2f", alpha), 495, 587, 14, SKYBLUE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}