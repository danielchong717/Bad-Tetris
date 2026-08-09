/* Headers */
//Using SDL, SDL_image, and STL string
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>

/* Constants */
//Screen dimension constants
constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };



/* Class Prototypes */
class LTexture
{
public:
    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Remove copy constructor
	LTexture(const LTexture&) = delete;

    //Remove copy assignment
    LTexture& operator=(const LTexture&) = delete;

	//Remove move constructor
    LTexture(LTexture&&) = delete;

    //Remove move assignment
    LTexture& operator=(LTexture&&) = delete;

    //Loads texture from disk
    bool loadFromFile(std::string path);

    //Cleans up texture
    void destroy();

    //Draws texture
    void render(float x, float y);

    //Gets texture attributes
    int getWidth();
    int getHeight();
    bool isLoaded();

private:
    //Contains texture data
    SDL_Texture* mTexture;

    //Texture dimensions
    int mWidth;
    int mHeight;
};



/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWindow{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//Images we will be rendering per direction
LTexture gUpTexture, gDownTexture, gLeftTexture, gRightTexture;



/* Class Implementations */
//LTexture Implementation
LTexture::LTexture() :
    //Constructor to initialize texture variables
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 }
{

}

LTexture::~LTexture()
{
    //Destructor to clean up texture
    destroy();
}



bool LTexture::loadFromFile(std::string path)
{
    //Clean up texture if it already exists
    destroy();

    //Load surface
    if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr)
    {
        SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        //Create texture from surface
        if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr)
        {
            SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Clean up loaded surface
        SDL_DestroySurface(loadedSurface);
    }

    //Return success if texture loaded
    return mTexture != nullptr;
}



//Call DestroyTexture to release texture after use.
void LTexture::destroy()
{
    //Clean up texture
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}



void LTexture::render(float x, float y)
{
    //Set texture position
	SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

    //Render texture
    SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}



int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

bool LTexture::isLoaded()
{
    return mTexture != nullptr;
}



/* Function Prototypes */
//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();



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
        //Create window with renderer
        if (SDL_CreateWindowAndRenderer("SDL3 Tutorial: Textures and Extension Libraries", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer) == false)
        {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            success = false;
        }
    }

    return success;
}



bool loadMedia()
{
    //File loading flag
    bool success{ true };

    //Load directional images
    if (gUpTexture.loadFromFile("03-key-presses-and-key-states/up.png") == false)
    {
        SDL_Log("Unable to load up png image!\n");
        success = false;
    }
	if (gDownTexture.loadFromFile("03-key-presses-and-key-states/down.png") == false)
	{
		SDL_Log("Unable to load down png image!\n");
		success = false;
	}
	if (gLeftTexture.loadFromFile("03-key-presses-and-key-states/left.png") == false)
	{
		SDL_Log("Unable to load left png image!\n");
		success = false;
	}
    	if (gRightTexture.loadFromFile("03-key-presses-and-key-states/right.png") == false)
	{
		SDL_Log("Unable to load right png image!\n");
		success = false;
	}

    return success;
}

void close()
{
	//Destroy texture
    gUpTexture.destroy();
    gDownTexture.destroy();
    gLeftTexture.destroy();
    gRightTexture.destroy();

	//Destroy window we created
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

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

			//Current rendered texture
			LTexture* currentTexture = &gUpTexture;

            //Default background color is white
			SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };

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
                    } //For keyboard presses
                    else if (e.type == SDL_EVENT_KEY_DOWN) {
                        switch (e.key.key) {
                            case (SDLK_UP): currentTexture = &gUpTexture; break;
                            case (SDLK_DOWN): currentTexture = &gDownTexture; break;
                            case (SDLK_LEFT): currentTexture = &gLeftTexture; break;
                            case (SDLK_RIGHT): currentTexture = &gRightTexture; break;
                        }
                    }
                }

                //Reset background color to render with less issues
				bgColor.r = 0xFF;
				bgColor.g = 0xFF;
				bgColor.b = 0xFF;

                //Using keystate, set color
                const bool* keyStates = SDL_GetKeyboardState(nullptr);
				if (keyStates[SDL_SCANCODE_UP]) {
					bgColor.r = 0xFF;
					bgColor.g = 0x00;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_DOWN]) {
					bgColor.r = 0x00;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_LEFT]) {
					bgColor.r = 0x00;
					bgColor.g = 0x00;
					bgColor.b = 0xFF;
				}
				else if (keyStates[SDL_SCANCODE_RIGHT]) {
					bgColor.r = 0xFF;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}

                //Fill render white
                //First argument is renderer                
                //Second is the region of the screen we want to fill(whole screen from null),
                //Third is the pixel we want to fill the surface with, we use SDL_MapSurfaceRGB to get the pixel value for white color
				SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
                SDL_RenderClear(gRenderer);
                
                //Render image on screen
                currentTexture->render((kScreenWidth - currentTexture->getWidth() ) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f);

                //Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    //Clean up after break out of main loop to free resources, quit SDL, and return exitCode.
    close();

    return exitCode;
}