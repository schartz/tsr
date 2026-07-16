#include "tgaimage.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <utility>

constexpr TGAColor white = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &buff, TGAColor color) {
  bool should_transpose_image = std::abs(ax - bx) < std::abs(ay - by);

  // if the line is steep, we transpose the image
  if (should_transpose_image) {
    std::swap(ax, ay);
    std::swap(bx, by);
  }

  // if the coordinates are given RTL we make them LTR
  if (ax > bx) {
    std::swap(ax, bx);
    std::swap(ay, by);
  }
  int y = ay;
  int ierror = 0;
  for (int x = ax; x <= bx; x++) {
    float t = (x - ax) / static_cast<float>(bx - ax);
    int y = std::round(ay + (by - ay) * t);
    // if the line was transposed in past, we detranspose it
    //
    if (should_transpose_image)
      buff.set(y, x, color);
    else
      buff.set(x, y, color);

    ierror += 2 * std::abs(by - ay);
    if (ierror > bx - ax) {
      y += by > ay ? 1 : -1;
      ierror -= 2 * (bx - ax);
    }
  }

  /** Initial simple approach
    for (float t = 0.; t <= 1.; t += 0.02) {
    int x = std::round(ax + (t * (bx - ax)));
    int y = std::round(ay + (t * (by - ay)));

    buff.set(x, y, color);
    }
  **/
};

void stress_test() {

  constexpr int width = 64;
  constexpr int height = 64;
  TGAImage framebuffer(width, height, TGAImage::RGB);

  std::srand(std::time({}));
  for (int i = 0; i < (1 << 24); i++) {
    int ax = rand() % width, ay = rand() % height;
    int bx = rand() % width, by = rand() % height;
    line(ax, ay, bx, by, framebuffer,
         {rand() % 255, rand() % 255, rand() % 255, rand() % 255});
  }
  framebuffer.write_tga_file("renders/stress_test.tga");
}

void draw_image() {
  constexpr int width = 64;
  constexpr int height = 64;
  TGAImage framebuffer(width, height, TGAImage::RGB);

  int ax = 7, ay = 3;
  int bx = 12, by = 37;
  int cx = 62, cy = 53;

  framebuffer.set(ax, ay, white);
  framebuffer.set(bx, by, white);
  framebuffer.set(cx, cy, white);

  line(ax, ay, bx, by, framebuffer, blue);
  line(cx, cy, bx, by, framebuffer, green);

  line(cx, cy, ax, ay, framebuffer, yellow);
  line(ax, ay, cx, cy, framebuffer, red);

  framebuffer.write_tga_file("renders/framebuffer.tga");
}

int main(int argc, char **argv) {
  // ensure we have renders folder
  std::filesystem::create_directory("./renders");
  draw_image();
  stress_test();
  return 0;
}
