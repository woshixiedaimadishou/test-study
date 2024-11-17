#include <iostream>
#include <cmath>

// Bresenham 直线算法
void bresenhamLine(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1; // x步进方向
    int sy = (y0 < y1) ? 1 : -1; // y步进方向
    int err = dx - dy;           // 初始误差

    while (true)
    {
        // 输出当前点的坐标
        std::cout << "(" << x0 << ", " << y0 << ")" << std::endl;

        // 当到达终点时，退出循环
        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        // 保存当前误差
        int e2 = 2 * err;

        // 判断是否沿 x 方向移动
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        // 判断是否沿 y 方向移动
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

int main()
{
    int x0 = 20, y0 = 10; // 起点
    int x1 = 30, y1 = 18; // 终点

    // 计算并输出像素点
    bresenhamLine(x0, y0, x1, y1);
    system("pause");
    return 0;
}