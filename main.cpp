/* Headers */
//Using SDL, SDL_image, SDL_ttf, and STL string/stringstream
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <sstream>

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

	//Checks if SDL_TTF_MAJOR_VERSION is defined. If it is not, omit code inside the ifdef block.
    #if defined(SDL_TTF_MAJOR_VERSION)
    //Creates texture from text
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
    #endif

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

class LButton
{
    public:
        //Button dimensions
        static constexpr int kButtonWidth = 300;
        static constexpr int kButtonHeight = 200;

        //Initializes internal variables
        LButton();

        //Sets top left position
        void setPosition(float x, float y);

        //handles mouse event
        void handleEvent(SDL_Event* e);

        //Shows button sprite
        void render();
    private:
        enum class eButtonSprite
        {
            MouseOut = 0,
            MouseOverMotion = 1,
            MouseDown = 2,
            MouseUp = 3
        };

        //Top left position
        SDL_FPoint mPosition;

        //Currently used global sprite
        eButtonSprite mCurrentSprite;
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

//Global font
TTF_Font* gFont{ nullptr };

//Textures to render
LTexture gTimeTextTexture, gButtonSpriteTexture, gTextTexture, gColorsTexture, gArrowTexture, gSpriteSheetTexture, gBgTexture, gFooTexture, gUpTexture, gDownTexture, gLeftTexture, gRightTexture;



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

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    //Clean up existing texture
    destroy();

    //Load text surface
    if (SDL_Surface* textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), 0, textColor); textSurface == nullptr)
    {
        SDL_Log("Unable to render text surface! SDL_ttf error: %s\n", SDL_GetError());
    }
    else
    {
        //Create texture from surface pixels
        if (mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface); mTexture == nullptr)
        {
            SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Free temp surface
        SDL_DestroySurface(textSurface);
    }

    //Return success if texture loaded
    return mTexture != nullptr;
}
#endif

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

//Tutorial #9
//LButton implementation
LButton::LButton() :
    mPosition{ 0.f, 0.f },
    mCurrentSprite{ eButtonSprite::MouseOut }
{

}

void LButton::setPosition(float x, float y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
    //If mouse event happened
    if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        //Get mouse position
        float x = -1.f, y = -1.f;
        SDL_GetMouseState(&x, &y);

        //Check if mosue is in button
        bool inside = true;

        //Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        //Mouse is right of the button
        else if (x > mPosition.x + kButtonWidth)
        {
            inside = false;
        }
        // Mouse is above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        else if (y > mPosition.y + kButtonHeight)
        {
            inside = false;
        }

        //Handle mouse events
        //Mouse is outside button
        if (!inside)
        {
            mCurrentSprite = eButtonSprite::MouseOut;
        }
        //Mouse is inside button
        else
        {
            //Set mouse over sprite
            switch (e->type)
            {
                case SDL_EVENT_MOUSE_MOTION:
                mCurrentSprite = eButtonSprite::MouseOverMotion;
                break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                mCurrentSprite = eButtonSprite::MouseDown;
                break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                mCurrentSprite = eButtonSprite::MouseUp;
                break;
            }
        }
    }
}

void LButton::render()
{
    //Define sprites
    SDL_FRect spriteClips[] = {
        {0.f, 0 * kButtonHeight, kButtonWidth, kButtonHeight },
        {0.f, 1 * kButtonHeight, kButtonWidth, kButtonHeight },
        {0.f, 2 * kButtonHeight, kButtonWidth, kButtonHeight },
        {0.f, 3 * kButtonHeight, kButtonWidth, kButtonHeight },
    };

    //Show current button sprite
    gButtonSpriteTexture.render(mPosition.x, mPosition.y, &spriteClips[ static_cast<size_t>( mCurrentSprite ) ] );
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
        else
        {
            //Check for initialization of font loading
            if (TTF_Init() == false)
            {
                SDL_Log("SDL_ttf could not initialize! SDL_ttf error: %s\n", SDL_GetError());
                success = false;
            }
        }
    }

    return success;
}

bool loadMedia()
{
    //File loading flag
    bool success{ true };

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
    if (gButtonSpriteTexture.loadFromFile("09-mouse-events/button.png") == false)
    {
        SDL_Log("Unable to load button image!\n");
        success = false;
    }

    //Tutorial #8
    //Load scene font
    std::string fontPath{ "08-true-type-fonts/lazy.ttf" };

    if (gFont = TTF_OpenFont(fontPath.c_str(), 28); gFont == nullptr)
    {
        SDL_Log("Could not load %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
        success = false;
    }
    else
    {
        //Load text
        SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
        if (gTextTexture.loadFromRenderedText("ttf study and font/text render demonstrate", textColor) == false)
        {
            SDL_Log("Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
            success = false;
        }
    }

    //Tutorial #10
    fontPath = "10-timing/lazy.ttf" ;
    if (gFont = TTF_OpenFont(fontPath.c_str(), 28); gFont == nullptr)
    {
        SDL_Log("Could not load %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
        success = false;
    }
    else
    {
        //Load text
        SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
        if (gTextTexture.loadFromRenderedText("ttf study and font/text render demonstrate", textColor) == false)
        {
            SDL_Log("Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
            success = false;
        }
    }

    return success;
}

void close()
{
	//Destroy texture
    gTextTexture.destroy();
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
    TTF_Quit();
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

            //Tutorial #9
            //Place buttons
            constexpr int kButtonCount = 4;
            LButton buttons[kButtonCount];
            buttons[0].setPosition(0, 0);
            buttons[1].setPosition(kScreenWidth - LButton::kButtonWidth, 0);
            buttons[2].setPosition(0, kScreenHeight - LButton::kButtonHeight);
            buttons[3].setPosition(kScreenWidth - LButton::kButtonWidth, kScreenHeight - LButton::kButtonHeight);

            //Tutorial #10
            //Timer start time variable
            Uint64 startTime = 0;

            //In memory text stream to turn time into text
            std::stringstream timeText;

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
                    //Tutorial #10
                    //Reset start time on return keypress
                    else if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_RETURN)
                    {
                        //Set the new start time when ENTER key is pressed
                        //Timer starts by getting current application time with SDL_GetTicks()
                        startTime = SDL_GetTicks();
                    }
                    //Tutorial #7
                    // On key press
                    else if (e.type == SDL_EVENT_KEY_DOWN)
                    {
                        eColorChannel channelToUpdate = eColorChannel::Unknown;
                        switch (e.key.key)
                        {
                            //Update texture color
                        case SDLK_A:g
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

                    //Tutorial #9
                    //Handle button events
                    for (int i = 0; i < kButtonCount; ++i)
                    {
                        //Passing event from event loop to our buttons, &e being the events
                        buttons[i].handleEvent(&e);
                    }
                }

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

                //Tutorial #8
                //Render text
                gTextTexture.render((kScreenWidth - gTextTexture.getWidth()) / 2.f, (kScreenHeight - gTextTexture.getHeight()) / 2.f);

                //Tutorial #9
                //Render buttons
                for (int i = 0; i < kButtonCount; i++)
                {
                    buttons[i].render();
                }

                //Tutorial #10
                //If the timer has started
                if (startTime != 0)
                {
                    //Update text
                    timeText.str("");
                    timeText << "Milliseconds since start time " << SDL_GetTicks() - startTime;
                    SDL_Color textColor{ 0x00, 0x00, 0x00, 0xFF };
                    gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor);
                }
                
                //Fill render white for background
                //First argument is renderer                
                //Second is the region of the screen we want to fill(whole screen from null),
                //Third is the pixel we want to fill the surface with, we use SDL_MapSurfaceRGB to get the pixel value for white color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Tutorial #10
                //Draw text
                gTimeTextTexture.render((kScreenWidth - gTimeTextTexture.getWidth()) / 2.f, (kScreenHeight - gTimeTextTexture.getHeight()) / 2.f);

                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }

    //Clean up after break out of main loop to free resources, quit SDL, and return exitCode.
    close();

    return exitCode;
}