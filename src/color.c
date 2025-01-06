# include "color.h"

int compareColor(const void *a, const void *b)
{
    int a1 = 0, b1 = 0;
    for (int i = 0; i < sizeof(int); i++)
    {
        a1 |= (*((unsigned char*)a + i) & 0x0F) << (i * 8);
        b1 |= (*((unsigned char*)b + i) & 0x0F) << (i * 8);
    }
    
    return (a1 < b1) ? -1 : (b1 < a1) ? 1 : (*((int*)a) < *((int*)b)) ? -1 : (*((int*)a) > *((int*)b)) ? 1 : 0;
}
Vec3 unpackRGB(unsigned int packedRGB) {
    // 提取 R, G, B 分量
    unsigned int R = (packedRGB >> 16) & 0xFF; // 提取最高 8 位
    unsigned int G = (packedRGB >> 8) & 0xFF;  // 提取中间 8 位
    unsigned int B = packedRGB & 0xFF;         // 提取最低 8 位

    // 归一化到 [0.0, 1.0]
    float normalized_R = R / 255.0;
    float normalized_G = G / 255.0;
    float normalized_B = B / 255.0;

    // 返回 Vec3
    Vec3 color = {normalized_R, normalized_G, normalized_B};
    return color;
}
void writeColour(FILE *ppmFile, Vec3 color) {
    int r = (int)(color.x * 255.999); 
    int g = (int)(color.y * 255.999);
    int b = (int)(color.z * 255.999);

    r = r < 0 ? 0 : r > 255 ? 255 : r;
    g = g < 0 ? 0 : g > 255 ? 255 : g;
    b = b < 0 ? 0 : b > 255 ? 255 : b;

    fprintf(ppmFile, "%d %d %d ", r, g, b);
}