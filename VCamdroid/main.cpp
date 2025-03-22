#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
#include <softcam/softcam.h>

const int WIDTH = 320;
const int HEIGHT = 240;

void draw(unsigned char* data)
{
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            data[0] = rand() % 255;
            data[1] = rand() % 255;
            data[2] = rand() % 255;
            data += 3;
        }
    }
}

int main()
{
    // First, create a virtual camera instance with scCreateCamera().
    // A virtual camera is a source of a video image stream.
    // The dimension width and height can be any positive number
    // that is a multiple of 4.
    // The third argument framerate is used to make sending frames at regular intervals.
    // This framerate argument can be omitted, and the default framerate is 60.
    // If you want to send every frame immediately without the frame rate regulator,
    // specify 0 to the framerate argument, then it will be a variable frame rate.
    scCamera cam = scCreateCamera(WIDTH, HEIGHT, 60);
    if (!cam)
    {
        std::printf("failed to create camera\n");
        return 1;
    }
    std::printf("Ok, Softcam is now active.\n");

    // Here, we wait for an application to connect to this camera.
    // You can comment out this line to start sending frames immediately
    // no matter there is a receiver or not.
    scWaitForConnection(cam);

    // Our canvas is a simple array of RGB pixels.
    // Note that the color component order is BGR, not RGB.
    // This is due to the convention of DirectShow.
    std::vector<unsigned char> image(WIDTH * HEIGHT * 3);

    for (int i = 0; i < 100000; i++)
    {
        draw(image.data());
        // Send the image as a newly captured frame of the camera.
        scSendFrame(cam, image.data());
    }

    // Delete the camera instance.
    // The receiver application will no longer receive new frames from this camera.
    scDeleteCamera(cam);
    std::printf("Softcam has been shut down.\n");
}
