#include "vector.h"
# include "spheres.h"
# include "color.h"

// Create representations for viewport and camera
//Create representations for light and background color
typedef struct {
    Vec3 position;
    float focal_length;
} camera;

typedef struct {
    float height;
    float width;
    float z;
} viewport;

typedef struct {
    Vec3 position;
    Vec3 direction;
} ray;

typedef struct {
    Vec3 position; 
    float brightness; 
} Light;

void read_files(const char *file_path);
void output(const char *outputFilePath);
void initializeViewport();
int isInShadow(Vec3 intersectionPoint, Vec3 normal, Light light, Sphere *spheres, int numSpheres);
void renderPipeline(int imageWidth, int imageHeight, Vec3 rayPos, Sphere *spheres, int numSpheres, Light light, Vec3 backgroundColor, const char *outputFilePath);
void renderPipelineFS(int imageWidth, int imageHeight, Vec3 rayPos, Sphere *spheres, int numSpheres, Light light, Vec3 backgroundColor, const char *outputFilePath);
