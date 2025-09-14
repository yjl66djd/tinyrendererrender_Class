#include <cmath>
#include <tuple>
#include <iostream>
#include <string>
#include "Timer.h"  
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

constexpr int width = 800;
constexpr int height = 800;

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

	//优化策略：减少一次y从浮点到整数的转换
	float current_y = ay;
	//float error = 0;//误差值用于处理四舍五入
	int ierror = 0;//整数优化方法 

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

		//error += (by - ay) / static_cast<float>(bx - ax);
		////当误差值大于0.5时四舍五入，y轴移动一个像素
		//if (error > 0.5) {
		//	y += by > ay ? 1: -1 ;
		//	error -= 1.0;
		//}

		//整数优化方法
		ierror += std::abs(by - ay);
        if (ierror >= (bx - ax)) {
            current_y += by > ay ? 1 : -1;
            ierror -= (bx - ax);
        }
		//优化掉if判断
        /*y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -=  (bx - ax) * (ierror > bx - ax);*/
	}

    //对使用重心坐标的方式在直线上进行等距离采样
    //for (float t = 0; t < 1; t += .02) {
    //    int x = std::round(ax + (bx - ax) * t);
    //    int y = std::round(ay + (by - ay) * t);+
    //    framebuffer.set(x, y, color);
    //}
}

//正交投影变化将[-1,1]区间的坐标映射到屏幕坐标
std::tuple<int,int> project(vec3 v) {
	return { ((v.x + 1.) * width / 2.),
			 ((v.y + 1.) * height / 2.) }; 
}

int main(int argc, char** argv) {  
   std::string modelPath;
   
   // 如果没有提供命令行参数，提示用户输入或使用默认模型
   if (argc != 2) {  
       std::cout << "请输入模型文件路径 (或直接按Enter使用默认的diablo3_pose.obj): ";
       std::getline(std::cin, modelPath);
       
       if (modelPath.empty()) {
           modelPath = "obj/diablo3_pose/diablo3_pose.obj";
           std::cout << "使用默认模型: " << modelPath << std::endl;
       }
   } else {
       modelPath = argv[1];
   }

   Model model(modelPath);
   
   // 检查模型是否成功加载
   if (model.nverts() == 0) {
       std::cerr << "错误: 无法加载模型文件 '" << modelPath << "'" << std::endl;
       std::cerr << "请确保文件路径正确且文件存在。" << std::endl;
       return 1;
   }
   
   std::cout << "成功加载模型: 顶点数=" << model.nverts() << ", 面数=" << model.nfaces() << std::endl;
   
   TGAImage framebuffer(width, height, TGAImage::RGB);  

   //绘制线框
   {  
       Timer t;   
       for (int i = 0; i < model.nfaces(); i++) { // iterate through all triangles
           auto [ax, ay] = project(model.vert(i, 0));
           auto [bx, by] = project(model.vert(i, 1));
           auto [cx, cy] = project(model.vert(i, 2));
           line(ax, ay, bx, by, framebuffer, red);
           line(bx, by, cx, cy, framebuffer, red);
           line(cx, cy, ax, ay, framebuffer, red);
       }

       for (int i = 0; i < model.nverts(); i++) { // iterate through all vertices
           vec3 v = model.vert(i);            // get i-th vertex
           auto [x, y] = project(v);          // project it to the screen
           framebuffer.set(x, y, white);
       }
   }  

   framebuffer.write_tga_file("framebuffer.tga");
   std::cout << "渲染完成，输出文件: framebuffer.tga" << std::endl;
   return 0;  
}