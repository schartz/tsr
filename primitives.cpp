#include "primitives.h"
#include "tgaimage.h"
#include <array>
#include <cmath>
#include <utility>

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

double triangle_area_shoelace_method(int ax, int ay, int bx, int by, int cx, int cy) {
    // for explaination check: https://en.wikipedia.org/wiki/Shoelace_formula#Triangle_formula
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &zbuffer,
              TGAImage &framebuffer, TGAColor color) {

    int bbminx = std::min(std::min(ax, bx), cx);
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);

    double total_area = triangle_area_shoelace_method(ax, ay, bx, by, cx, cy);
    if (total_area < 0.)
        return;

#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {

            double alpha = triangle_area_shoelace_method(x, y, bx, by, cx, cy) / total_area;
            double beta = triangle_area_shoelace_method(x, y, cx, cy, ax, ay) / total_area;
            double gamma = triangle_area_shoelace_method(x, y, ax, ay, bx, by) / total_area;
            unsigned char z = (alpha * az + beta * bz + gamma * cz);
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;
            zbuffer.set(x, y, {z});
            framebuffer.set(x, y, color);
        }
    }
}
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {

    int bbminx = std::min(std::min(ax, bx), cx);
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);

    double total_area = triangle_area_shoelace_method(ax, ay, bx, by, cx, cy);
    if (total_area < 0.)
        return;

#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {

            double alpha = triangle_area_shoelace_method(x, y, bx, by, cx, cy) / total_area;
            double beta = triangle_area_shoelace_method(x, y, cx, cy, ax, ay) / total_area;
            double gamma = triangle_area_shoelace_method(x, y, ax, ay, bx, by) / total_area;

            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;
            framebuffer.set(x, y, color);
        }
    }
}
