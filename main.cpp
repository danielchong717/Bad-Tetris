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

//Set color constants
constexpr int kColorMagnitudeCount = 3;
constexpr Uint8 kColorMagnitudes[kColorMagnitudeCount] = { 0x00, 0x7F, 0xFF };
enum class eColorChannel
{
    TextureRed = 0,
    TextureGreen = 1,
    TextureBlue = 2,
    TextureAlpha = 3,

    BackgroundRed = 4,
    BackgroundGreen = 5,
    BackgroundBlue = 6,

    Total = 7,
    Unknown = 8
};

/* Class Prototypes */
class LTexture
{
public:
    //Symbolic constant
    static constexpr float kOriginalSize = -1.f;

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

    //Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set opacity
	void setAlpha(Uint8 alpha);

    //Set blending mode
    void setBlending(SDL_BlendMode blendMode);

    //Draws texture
    void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize, double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

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

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

//Images we will be rendering per direction
LTexture gColorsTexture, gArrowTexture, gSpriteSheetTexture, gBgTexture, gFooTexture, gUpTexture, gDownTexture, gLeftTexture, gRightTexture;



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
        //Color key image
		if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x00, 0xFF, 0xFF)) == false)
		{
			SDL_Log("Unable to set color key for image %s! SDL error: %s\n", path.c_str(), SDL_GetError());
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

void LTexture::render(float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipmode )
{
    //Set texture position
	SDL_FRect dstRect{ x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

    //Default to clip dimensions if clip is given
	if (clip != nullptr)
	{
		dstRect.w = clip->w;
		dstRect.h = clip->h;
	}

    //Resize if there are new dimensions
    if (width > 0)
    {
        dstRect.w = width;
    }
    if (height > 0)
    {
        dstRect.h = height;
    }

    //Render texture
    SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipmode);
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

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::setBlending(SDL_BlendMode blendMode)
{
    SDL_SetTextureBlendMode(mTexture, blendMode);
}

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

    //Load relevant images
    if (gFooTexture.loadFromFile("04-color-keying/foo.png") == false)
    {
        SDL_Log("Unable to load up png image!\n");
        success = false;
    }
	if (gBgTexture.loadFromFile("04-color-keying/background.png") == false)
	{
		SDL_Log("Unable to load background image!\n");
		success = false;
	}
    if (gSpriteSheetTexture.loadFromFile("05-sprite-clipping-and-stretching/dots.png") == false)
    {
        SDL_Log("Unable to load sprite sheet image!\n");
        success = false;
    }
    if (gArrowTexture.loadFromFile("06-rotation-and-flipping/arrow.png") == false)
    {
        SDL_Log("Unable to load arrow image!\n");
        success = false;
    }
    if (gColorsTexture.loadFromFile("07-color-modulation-and-alpha-blending/colors.png") == false)
    {
        SDL_Log("Unable to load colors image!\n");
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
	gBgTexture.destroy();
	gFooTexture.destroy();
    gSpriteSheetTexture.destroy();
    gArrowTexture.destroy();

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

            //Rotation degrees
            double degrees = 0.0;

            //Flipmode
            SDL_FlipMode flipMode = SDL_FLIP_NONE;

			//Current rendered texture
			LTexture* currentTexture = &gUpTexture;

            //Default background color is white
			SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };

            //Initialize colors
            Uint8 colorChannelsIndices[static_cast<size_t>(eColorChannel::Total)];
            colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureRed)] = 2;
            colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureGreen)] = 2;
            colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureBlue)] = 2;
            colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureAlpha)] = 2;

            colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundRed)] = 2;
			colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundGreen)] = 2;
            colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundBlue)] = 2;

            //Initialize blending
            gColorsTexture.setBlending(SDL_BLENDMODE_BLEND);

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
                    // On key press
                    else if (e.type == SDL_EVENT_KEY_DOWN )
                    { 
                        eColorChannel channelToUpdate = eColorChannel::Unknown;
                        switch (e.key.key)
                        {
                            //Update texture color
                            case SDLK_A:
                            channelToUpdate = eColorChannel::TextureRed;
                            break;
                            case SDLK_S:
                            channelToUpdate = eColorChannel::TextureGreen;
                            break;
                            case SDLK_D:
							channelToUpdate = eColorChannel::TextureBlue;
                            break;
                            case SDLK_F:
                            channelToUpdate = eColorChannel::TextureAlpha;
                            break;

							//Update background color
                            case SDLK_Q:
                            channelToUpdate = eColorChannel::BackgroundRed;
                            break;
                            case SDLK_W:
                            channelToUpdate = eColorChannel::BackgroundGreen;
                            break;
                            case SDLK_E:
                            channelToUpdate = eColorChannel::BackgroundBlue;
                            break;

                            //Rotate on left/right press
                            case SDLK_LEFT:
                            degrees -= 36;
                            break;
                            case SDLK_RIGHT:
                            degrees += 36;
                            break;

                            //Set flip mode based on 1/2/3 key press
                            case SDLK_1:
                            flipMode = SDL_FLIP_HORIZONTAL;
                            break;
                            case SDLK_2:
                            flipMode = SDL_FLIP_NONE;
                            break;
                            case SDLK_3:
                            flipMode = SDL_FLIP_VERTICAL;
                            break;
                        }

						//Tutorial #7
                        //If channel key was pressed
                        if (channelToUpdate != eColorChannel::Unknown)
                        {
                            //Cycle through channel values
                            colorChannelsIndices[static_cast<size_t>(channelToUpdate)]++;
                            if (colorChannelsIndices[static_cast<size_t>(channelToUpdate)] >= kColorMagnitudeCount)
                            {
                                colorChannelsIndices[static_cast<size_t>(channelToUpdate)] = 0;
                            }

                            //Write color values to console
                            SDL_Log("Texture - R:%d G: %d B: %d A: %d | Background - R:%d G:%d B:%d",
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureRed)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureGreen)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureBlue)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureAlpha)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundRed)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundGreen)]],
                                kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundBlue)]]
                            );
                        }
                    }
                }
                
                //Fill render white
                //First argument is renderer                
                //Second is the region of the screen we want to fill(whole screen from null),
                //Third is the pixel we want to fill the surface with, we use SDL_MapSurfaceRGB to get the pixel value for white color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
               
                //Tutorial #4
                //Render image on screen
                gBgTexture.render(0.f, 0.f);
                gFooTexture.render(240.f, 190.f);

                //Tutorial #5
                //Init sprite clip
                constexpr float kSpriteSize = 100.f;
                SDL_FRect spriteClip{ 0.f, 0.f, kSpriteSize, kSpriteSize };

                //Init sprite size
                SDL_FRect spriteSize{ 0.f, 0.f, kSpriteSize, kSpriteSize };

                //Top left sprite
                spriteClip.x = 0.f;
                spriteClip.y = 0.f;

                //Set sprite size to original size
                spriteSize.w = kSpriteSize;
                spriteSize.h = kSpriteSize;

                //Draw original sized sprite
                gSpriteSheetTexture.render(0.f, 0.f, &spriteClip, spriteSize.w, spriteSize.h);

                //Top right sprite
                spriteClip.x = kSpriteSize;
                spriteClip.y = 0.f;

                //Set sprite to half size
                spriteSize.w = kSpriteSize * 0.5f;
                spriteSize.h = kSpriteSize * 0.5f;

                //Draw half size sprite
                gSpriteSheetTexture.render(kScreenWidth - spriteSize.w, 0.f, &spriteClip, spriteSize.w, spriteSize.h);

                //Bottom left sprite
                spriteClip.x = 0.f;
                spriteClip.y = kSpriteSize;

                //Set sprite to double size
                spriteSize.w = kSpriteSize * 2.f;
                spriteSize.h = kSpriteSize * 2.f;

                //Draw double size sprite
                gSpriteSheetTexture.render(0.f, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h);

                //Bottom right sprite
                spriteClip.x = kSpriteSize;
                spriteClip.y = kSpriteSize;

                //Squish sprite vertically
                spriteSize.w = kSpriteSize;
                spriteSize.h = kSpriteSize * 0.5f;

                //Draw squished sprite
                gSpriteSheetTexture.render(kScreenWidth - spriteSize.w, kScreenHeight - spriteSize.h, &spriteClip, spriteSize.w, spriteSize.h);

				//Tutorial #6
                //Define the center from the corner of the image
                SDL_FPoint center{ gArrowTexture.getWidth() / 2.f, gArrowTexture.getHeight() / 2.f };

                //Draw texture rotated/flipped
                gArrowTexture.render((kScreenWidth - gArrowTexture.getWidth()) / 2.f, (kScreenHeight - gArrowTexture.getHeight()) / 2.f, nullptr, LTexture::kOriginalSize, LTexture::kOriginalSize, degrees, &center, flipMode);

				//Tutorial #7
                //Fill the background
                SDL_SetRenderDrawColor(gRenderer,
					kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundRed)]], 
					kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundGreen)]], 
					kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::BackgroundBlue)]], 
					0xFF);
                SDL_RenderClear(gRenderer);

				//Set texture color and render
                gColorsTexture.setColor(
                    kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureRed)]],
                    kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureGreen)]],
                    kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureBlue)]]
                );
				gColorsTexture.setAlpha(kColorMagnitudes[colorChannelsIndices[static_cast<size_t>(eColorChannel::TextureAlpha)]]);
                gColorsTexture.render((kScreenWidth - gColorsTexture.getWidth()) / 2.f, (kScreenHeight - gColorsTexture.getHeight()) / 2.f);

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Clean up after break out of main loop to free resources, quit SDL, and return exitCode.
    close();

    return exitCode;
}