/* Headers */
//Using SDL and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>

/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };



/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The surface contained by the window
SDL_Surface* gScreenSurface{ nullptr };

//The image we will load and show on the screen
SDL_Surface* gHelloWorld{ nullptr };



/* Function Implementations */
bool init()
{
    //Initialization flag
    bool success{ true };

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Create window
        if (gWindow = SDL_CreateWindow("SDL3 Tutorial: Hello SDL3", kScreenWidth, kScreenHeight, 0); gWindow == nullptr)
        {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Get window surface
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

bool loadMedia()
{
    //File loading flag
    bool success{ true };

    //Load splash image
    std::string imagePath{ "01-hello-sdl3/hello-sdl3.bmp" };
    if (gHelloWorld = SDL_LoadBMP(imagePath.c_str()); gHelloWorld == nullptr)
    {
        SDL_Log("Unable to load image %s! SDL Error: %s\n", imagePath.c_str(), SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
	//Destroy surface we loaded
    SDL_DestroySurface(gHelloWorld);
	gHelloWorld = nullptr;

	//Destroy window we created
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gScreenSurface = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
    //Final exit code
	int exitCode{ 0 };

    ///Initialize
    if (init() == false)
    {
        SDL_Log("Unable to initialize program!\n");
        exitCode = 1;
    }
    else
    {
        //Load media
        if (loadMedia() == false) {
			SDL_Log("Unable to load media!\n");
            exitCode = 2;
        }
        else {
            //Create a quit flag
			bool quit{ false };

            //Event data
            SDL_Event e;
            SDL_zero(e);

            //Main loop of program
            while (quit == false)
            {
                //Get event data
                while (SDL_PollEvent(&e) == true)
                {
                    //If event is to quit(or of quit type essentially)
                    if (e.type == SDL_EVENT_QUIT)
                    {
                        //End main loop
                        quit = true;
                    }
                }

                //Fill the surface white, 
                //First argument is surface we want to fill, 
                //Second is the region of the screen we want to fill(whole screen from null),
                //Third is the pixel we want to fill the surface with, we use SDL_MapSurfaceRGB to get the pixel value for white color
                SDL_FillSurfaceRect(gScreenSurface, nullptr, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));

                //Render image on screen by 'blitting' it to the screen surface(copy data from source surface,
                //onto the destination surface like a rubber stamp.
                //First argument is the source surface, 
                //Second is the region of the source surface we want to blit(whole image from null),
                //Third is the actual destination surface we want to blit to,
                //Fourth is the region of the destination surface we want to blit to(whole screen from null)
                SDL_BlitSurface(gHelloWorld, nullptr, gScreenSurface, nullptr);

                //Update the surface(or update the rendering for screen surface). Once this is done,
                //go back to the top, check for inputs, etc etc. until users X's out of program
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }

    //Clean up after break out of main loop to free resources, quit SDL, and return exitCode.
    close();

    return exitCode;
}