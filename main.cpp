#include "geometry_types.h"
#include "obj_loader.h"
#include "primitives.h"
#include "tgaimage.h"
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <tuple>

void stress_test() {

    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::srand(std::time({}));
    for (int i = 0; i < (1 << 24); i++) {
        int ax = rand() % width, ay = rand() % height;
        int bx = rand() % width, by = rand() % height;
        line(ax, ay, bx, by, framebuffer,
             {static_cast<std::uint8_t>(rand() % 255), static_cast<std::uint8_t>(rand() % 255),
              static_cast<std::uint8_t>(rand() % 255), static_cast<std::uint8_t>(rand() % 255)});
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

std::tuple<int, int> project(vec3 v, int width, int height) {
    return {(v.x + 1.) * width / 2, // Second, since the input models are scaled to have fit in the
                                    // [-1,1]^3 world coordinates,
            (v.y + 1.) * height / 2};
}

int draw_model_wireframe(int argc, char **argv) {
    constexpr int width = 800;
    constexpr int height = 800;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/model.obj" << std::endl;
        return 1;
    }

    ObjModel model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < model.nfaces(); i++) {
        auto [ax, ay] = project(model.vert(i, 0), width, height);
        auto [bx, by] = project(model.vert(i, 1), width, height);
        auto [cx, cy] = project(model.vert(i, 2), width, height);

        line(ax, ay, bx, by, framebuffer, green);
        line(bx, by, cx, cy, framebuffer, green);
        line(cx, cy, ax, ay, framebuffer, green);
    }

    for (int i = 0; i < model.nverts(); i++) {   // iterate through all vertices
        vec3 v = model.vert(i);                  // get i-th vertex
        auto [x, y] = project(v, width, height); // project it to the screen
        framebuffer.set(x, y, white);
    }

    framebuffer.write_tga_file("./renders/model_wireframe.tga");
    return 0;
}

int draw_model_trig_raster(int argc, char **argv) {
    constexpr int width = 800;
    constexpr int height = 800;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " path/to/model.obj" << std::endl;
        return 1;
    }

    ObjModel model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int i = 0; i < model.nfaces(); i++) { // iterate through all triangles
        auto [ax, ay] = project(model.vert(i, 0), width, height);
        auto [bx, by] = project(model.vert(i, 1), width, height);
        auto [cx, cy] = project(model.vert(i, 2), width, height);
        TGAColor rnd;
        for (int c = 0; c < 3; c++)
            rnd[c] = std::rand() % 255;
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }
    framebuffer.write_tga_file("./renders/model_trig_raster.tga");
    return 0;
}

void draw_single_raster_triangle() {
    int width = 128, height = 128;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
    triangle(120, 35, 90, 5, 45, 110, framebuffer, white);
    triangle(115, 83, 80, 90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("./renders/triangle.tga");
}

int main(int argc, char **argv) {
    // ensure we have renders folder
    std::filesystem::create_directory("./renders");
    // draw_image();
    // stress_test();
    // draw_model(argc, argv);
    draw_model_wireframe(argc, argv);
    draw_model_trig_raster(argc, argv);

    return 0;
}
