#include "tgaimage.h"
#include <cmath>

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) 
{ 
    //�ж��߶ε�y���Ƿ��x�ᶸ��
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    //���Ϊ�������򽻻�xy���꣨��������ʱ����x����в���������xy��y������ȫ������
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    //ȷ���������ʼ�����յ���Ҳ�
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    //ʹ��x�����ؽ��м������������x������е㣬��Ϊ�����������������н�����xy��
    for (int x = ax; x <=bx ; x ++) {
        
        //tֵ���㷽ʽΪx�ƶ��������м���ĸ�λ����
        float t = (x - ax) / static_cast<float>(bx - ax);
        int y = std::round(ay + (by - ay) * t);
        //���Ϊ�����߶��������Ϸ�������xy����ֵ���ڻ���ʱҲҪ������λ�ý�������
        if (steep)
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
    }

    //��ʹ����������ķ�ʽ��ֱ���Ͻ��еȾ������
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