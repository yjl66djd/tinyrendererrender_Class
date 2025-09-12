#include "tgaimage.h"
#include <cmath>

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) 
{ 
    //判断线段的y轴是否比x轴陡峭
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    //如果为陡峭线则交换xy坐标（后续采样时根据x轴进行采样，交换xy让y轴能完全采样）
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    //确保输入的起始点在终点的右侧
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    //使用x轴像素进行间隔采样（采样x轴的所有点，如为陡峭曲线则在上文中交换了xy）
    for (int x = ax; x <=bx ; x ++) {
        
        //t值计算方式为x移动到两点中间的哪个位置了
        float t = (x - ax) / static_cast<float>(bx - ax);
        int y = std::round(ay + (by - ay) * t);
        //如果为陡峭线段则由于上方交换了xy的数值，在绘制时也要将两者位置交换回来
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
    }

    //对使用重心坐标的方式在直线上进行等距离采样
    //for (float t = 0; t < 1; t += .02) {
    //    int x = std::round(ax + (bx - ax) * t);
    //    int y = std::round(ay + (by - ay) * t);+
    //    framebuffer.set(x, y, color);
    //}
}

int main(int argc, char** argv) {
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 7, ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    line(ax, ay, bx, by, framebuffer, blue);
    line(bx, by, cx, cy, framebuffer, red);
    line(cx, cy, ax, ay, framebuffer, green);

    framebuffer.set(ax, ay, red);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}