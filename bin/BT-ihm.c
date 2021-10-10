
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ihm-header.h"
#include "../headers/lang-header.h"
#include <ctype.h>
#include <time.h>

static SDL_Window *window;
static double sizeFactor;

profile getClientProfile();

profile initBlankProfile();

userSettings getUserSettings();

void setPacketThreadTimeOut();

void setUserSettings(userSettings settings);

int netRequest(void *temp);

netPacket sandPackedRequest(char* instruction, netPacket *packet) {

    strcpy(packet->instruction, instruction);

    SDL_Thread* threadID = SDL_CreateThread(netRequest, "NetThread", packet);
    setPacketThreadTimeOut(packet,200);
    free(threadID);

    return *packet;
}

HSVColor rgb_to_hsv(RGBColor in)
{
    HSVColor         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0;
        return out;
    }
    if ( max > 0.0 ) {
        out.s = (delta / max);
    } else {

        out.s = 0.0;
        out.h = NAN;
        return out;
    }
    if ( in.r >= max )
        out.h = ( in.g - in.b ) / delta;
    else if ( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;

    out.h *= 60.0;

    if ( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


RGBColor hsv_to_rgb(HSVColor in)
{
    double      hh, p, q, t, ff;
    long        i;
    RGBColor         out;

    if (in.s <= 0.0) {
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

SDL_Color from_RGBColor(RGBColor rgb_color)
{
    SDL_Color color = {
        rgb_color.r * 255,
        rgb_color.g * 255,
        rgb_color.b * 255,
        0xff
    };
    return color;
}

int getScreenWidth() {

    SDL_DisplayMode dm;

    SDL_GetDesktopDisplayMode(0, &dm);

    return dm.w;
}

int sf(int value) {

    return value * sizeFactor;
}

char *removeSpace(char *str)
{
    char *end;

    while ( isspace(*str) )
    {
        str++;
    }

    end = str + strlen(str) - 1;
    while ( (end > str) && (isspace(*end)) )
    {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

int getScreenWidth_DisplayRes(int resMode) {

    int resList[6] = {320, 640, 1280, 1920, 2560, 3840};

    return resList[resMode];
}

int getApropriateDisplayRes(int screenWidth) {

    for (int i = 0; i < 6; ++i)
    {
        if (getScreenWidth_DisplayRes(i) == getScreenWidth())
        {
            return i;
        }
    }

    return 2;
}

int isSettingsInitialized() {

    char temp[2] = {0};

    FILE *file;

    file = fopen("../initSettings.ini", "r");

    fgets(temp, 2, file);

    fclose(file);

    return atoi(temp);
}

void initSettings() {

    userSettings tempSettings;

    FILE *file;

    file = fopen("../game.cfg", "r");

    char temp[40];

    for (int i = 0; i < 11; ++i)
    {
        fgets(temp, 20, file);
    }

    tempSettings.selectedLanguage = atoi(temp);

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < 15; ++i)
    {
        fgets(temp, 20, file);
    }

    tempSettings.musicVolume = atoi(temp);

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < 18; ++i)
    {
        fgets(temp, 20, file);
    }

    tempSettings.effectsVolume = atoi(temp);

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < 22; ++i)
    {
        fgets(temp, 20, file);
    }

    tempSettings.displayMode = atoi(temp);

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < 25; ++i)
    {
        fgets(temp, 20, file);
    }

    tempSettings.displayRes = atoi(temp);

    fseek(file, 0, SEEK_SET);

    for (int i = 0; i < 29; ++i)
    {
        fgets(temp, 40, file);
    }

    tempSettings.rotateKey = SDL_GetKeyFromName(removeSpace(temp));

    if (tempSettings.rotateKey == SDLK_UNKNOWN)
    {
        tempSettings.rotateKey = 120;
    }

    if (!isSettingsInitialized()) {

        tempSettings.displayRes = getApropriateDisplayRes(getScreenWidth());
    }

    setUserSettings(tempSettings);

    setCurrentLang(tempSettings.selectedLanguage);

    fclose(file);
}

void setSettingsInitialized() {

    FILE *file;

    file = fopen("../initSettings.ini", "w");

    fprintf(file, "1");

    fclose(file);
}

void saveSettings() {

    FILE *file;

    file = fopen("../game.cfg", "w");

    fprintf(file, "CONFIG_FILE_START\n\n");
    fprintf(file, "\tLANG\n");
    fprintf(file, "\t\t0 ENGLISH\n");
    fprintf(file, "\t\t1 FRENCH\n");
    fprintf(file, "\t\t2 SPANISH\n");
    fprintf(file, "\t\t3 PORTUGUESE\n");
    fprintf(file, "\t\t4 GERMAN\n");
    fprintf(file, "\t\t5 ITALIAN\n\n");
    fprintf(file, "\t\t%d\n\n", getUserSettings().selectedLanguage);
    fprintf(file, "\tVOLUME\n");
    fprintf(file, "\t\tMUSIC\n");
    fprintf(file, "\t\t%d\n\n", getUserSettings().musicVolume);
    fprintf(file, "\t\tEFFECTS\n");
    fprintf(file, "\t\t%d\n\n", getUserSettings().effectsVolume);
    fprintf(file, "\tDISPLAY\n");
    fprintf(file, "\t\tMODE\n");
    fprintf(file, "\t\t%d\n\n", getUserSettings().displayMode);
    fprintf(file, "\t\tRESOLUTION\n");
    fprintf(file, "\t\t%d\n\n", getUserSettings().displayRes);
    fprintf(file, "\tKEYS\n");
    fprintf(file, "\t\tROTATE_KEY\n");
    fprintf(file, "\t\t%s\n\n", SDL_GetKeyName(getUserSettings().rotateKey));
    fprintf(file, "CONFIG_FILE_END");

    fclose(file);
}

void setSizeFactor(int screenWidth) {

    sizeFactor = screenWidth / (double)1280;
}

void SDL_start(SDL_Renderer **renderer, Mix_Music *music) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();
    SDLNet_Init();

    initSettings();

    if (getUserSettings().displayMode == 1)
    {
        setSizeFactor(getScreenWidth());
    }
    else {
        setSizeFactor(getScreenWidth_DisplayRes(getUserSettings().displayRes));
    }

    window = SDL_CreateWindow(gs("Battleship"),
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              sf(1280), sf(720), 0);

    switch (getUserSettings().displayMode) {

    case 0:
        SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN);
        break;
    case 1:
        SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        break;

    case 2:
        SDL_SetWindowFullscreen( window, 0);
        break;
    }

    SDL_SetWindowResizable(window, SDL_FALSE);

    *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Mix_AllocateChannels(1);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 );

    SDL_Surface* icon = IMG_Load("../images/icon.png");
    SDL_SetWindowIcon(window, icon);

    SDL_FreeSurface(icon);

    music = Mix_LoadMUS("../sound/music/music-menu.ogg");

    Mix_PlayMusic( music, -1);
}

void SDL_stop(SDL_Renderer **renderer, Mix_Music **music) {

    saveSettings();

    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(window);
    Mix_FreeMusic(*music);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
}

int prop(int value, int prop) {

    return (value * prop) / 100;
}

void drawRoundedBoxShadow(SDL_Renderer **renderer, int x, int y, int w, int h,
                          int radius, int shadowSize, int tempAlpha) {

    int alpha = tempAlpha / shadowSize;

    for (int i = 0; i < shadowSize; ++i)
    {
        roundedBoxColor(*renderer, x + i, y + i, w + i , h + i, radius,
                        SDL_FOURCC( 0, 0, 0, alpha));
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

void drawBoxShadow(SDL_Renderer **renderer, int x, int y, int w, int h,
                   int shadowSize, int tempAlpha) {

    int alpha = tempAlpha / shadowSize;

    SDL_Rect toRender;

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, alpha);

    for (int i = 0; i < shadowSize; ++i)
    {
        toRender.x = x + i;
        toRender.y = y + i;
        toRender.h = h + i;
        toRender.w = w + i;

        SDL_RenderFillRect(*renderer, &toRender);
    }
}

button mainMenuCreateTextureButtonRGBA(SDL_Renderer **renderer ,
                                       char *pathToTexture, int textureSize,
                                       int x, int y, int w, int h, int id,
                                       int r, int g , int b, int a) {

    int tempH;
    int tempW;

    double ratio;

    button element;

    element.clip.x = x;
    element.clip.y = y;
    element.clip.w = w;
    element.clip.h = h;

    element.label.texture = loadTexture(&*renderer, pathToTexture);

    SDL_QueryTexture(element.label.texture, NULL, NULL, &tempW, &tempH);

    ratio = tempH / tempW;

    element.label.clip.w = textureSize;
    element.label.clip.h = textureSize * (double)ratio;
    element.label.clip.x = (element.clip.x + (element.clip.w / 2)) -
                           (element.label.clip.w / 2);
    element.label.clip.y = (element.clip.y + (element.clip.h / 2)) -
                           (element.label.clip.h / 2);

    element.color.r = r;
    element.color.g = g;
    element.color.b = b;
    element.color.a = a;

    element.isSelected = 0;

    element.activated = 1;

    element.id = id;

    return element;
}

void mainMenuButtonTextureRender(SDL_Renderer **renderer, button btn, int angle) {

    int propAlpha = btn.color.a / 255 * 100;

    if (btn.isSelected == 1)
    {
        roundedBoxColor(*renderer, btn.clip.x, btn.clip.y, btn.clip.x +
                        btn.clip.w , btn.clip.y + btn.clip.h, sf(5),
                        SDL_FOURCC( 255, 255, 255, prop(64, propAlpha)));
    }

    SDL_SetTextureAlphaMod(btn.label.texture, btn.color.a);

    SDL_RenderCopyEx(*renderer, btn.label.texture, NULL, &btn.label.clip, angle,
                     NULL, SDL_FLIP_NONE);

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

void renderMenuBar(SDL_Renderer **renderer, mainBar menuBar) {

    drawBoxShadow(&*renderer, menuBar.barRect.x, menuBar.barRect.y,
                  menuBar.barRect.w, menuBar.barRect.h, 12,
                  prop(255, menuBar.tab[0].alphaProp));

    for (int i = 0; i < 5; ++i)
    {

        drawRoundedBoxShadow(&*renderer, menuBar.tab[i].clip.x,
                             menuBar.tab[i].clip.y, menuBar.tab[i].clip.x +
                             menuBar.tab[i].clip.w, menuBar.tab[i].clip.y +
                             menuBar.tab[i].clip.h, sf(5), 12,
                             prop(255, menuBar.tab[i].alphaProp));

        roundedBoxColor(*renderer, menuBar.tab[i].clip.x, menuBar.tab[i].clip.y,
                        menuBar.tab[i].clip.x + menuBar.tab[i].clip.w ,
                        menuBar.tab[i].clip.y + menuBar.tab[i].clip.h,
                        sf(5), SDL_FOURCC( prop(menuBar.tab[i].color.r,
                                                menuBar.tab[i].colorProp),
                                           prop(menuBar.tab[i].color.g,
                                                menuBar.tab[i].colorProp),
                                           prop(menuBar.tab[i].color.b,
                                                menuBar.tab[i].colorProp),
                                           prop(menuBar.tab[i].color.a,
                                                menuBar.tab[i].alphaProp)));

        SDL_SetTextureAlphaMod(menuBar.tab[i].name.texture,
                               prop(menuBar.tab[i].textAlpha,
                                    menuBar.tab[i].alphaProp));

        SDL_RenderCopy(*renderer, menuBar.tab[i].name.texture, NULL,
                       &menuBar.tab[i].name.clip);
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(*renderer,
                           prop(menuBar.tab[menuBar.actualTab].color.r,
                                menuBar.tab[menuBar.actualTab].colorProp),
                           prop(menuBar.tab[menuBar.actualTab].color.g,
                                menuBar.tab[menuBar.actualTab].colorProp),
                           prop(menuBar.tab[menuBar.actualTab].color.b,
                                menuBar.tab[menuBar.actualTab].colorProp),
                           prop(255, menuBar.tab[0].alphaProp));

    SDL_RenderFillRect(*renderer, &menuBar.barRect);

    mainMenuButtonTextureRender(&*renderer, menuBar.closeButton, 0);
}

void updateMenuBar(mainBar *bar, SDL_Renderer **renderer) {

    bar->barRect.x = 0;
    bar->barRect.y = 0;
    bar->barRect.h = sf(50);
    bar->barRect.w = sf(1280);

    int tabOffset = sf(10);

    char tabName[5][30];

    strcpy(tabName[0], gs("Home"));
    strcpy(tabName[1], gs("Scores"));
    strcpy(tabName[2], gs("Settings"));
    strcpy(tabName[3], gs("Statistics"));
    strcpy(tabName[4], gs("Credits"));

    for (int i = 0; i < 5; ++i)
    {
        bar->tab[i].clip.h = sf(35);
        bar->tab[i].clip.w = sf(200);
        bar->tab[i].clip.x = (tabOffset * (i + 1)) +
                             (bar->tab[i].clip.w * i);
        bar->tab[i].clip.y = bar->barRect.h - sf(5);

        bar->tab[i].name = createTextTexture(&*renderer, tabName[i],
                                             FONT_STANDARD,
                                             sf(20), bar->tab[i].clip.x +
                                             (bar->tab[i].clip.w / 2),
                                             bar->tab[i].clip.y +
                                             (bar->tab[i].clip.h / 2) +
                                             sf(5), HCENTER | VCENTER);
    }

    bar->closeButton = mainMenuCreateTextureButtonRGBA(&*renderer,
                       "../images/close.png", sf(25), sf(1255) - sf(35),
                       bar->barRect.h / 2 - (sf(35) / 2), sf(35), sf(35),
                       2, 231, 76, 60, 255);
}

mainBar initMenuBar(SDL_Renderer **renderer) {

    mainBar element;

    element.firstLoad = 1;

    element.tabChanged = -1;

    for (int i = 0; i < 5; ++i)
    {
        element.tab[i].isHovered = 0;

        element.tab[i].colorProp = 60;
        element.tab[i].textAlpha = 128;

        element.tab[i].colorAnimIn = 0;
        element.tab[i].colorAnimOut = 0;

        element.tab[i].color.a = 255;

        element.tab[i].alphaProp = 0;
    }

    element.tab[0].color.r = 40;
    element.tab[0].color.g = 55;
    element.tab[0].color.b = 71;

    element.tab[1].color.r = 46;
    element.tab[1].color.g = 64;
    element.tab[1].color.b = 83;

    element.tab[2].color.r = 52;
    element.tab[2].color.g = 73;
    element.tab[2].color.b = 94;

    element.tab[3].color.r = 93;
    element.tab[3].color.g = 109;
    element.tab[3].color.b = 126;

    element.tab[4].color.r = 133;
    element.tab[4].color.g = 146;
    element.tab[4].color.b = 158;

    element.actualTab = 0;

    element.tab[0].colorProp = 100;
    element.tab[0].textAlpha = 255;

    element.barAlpha = 0;

    element.animStart = 1;

    updateMenuBar(&element, &*renderer);

    return element;
}

void menuBarGetSate(SDL_Event event, mainBar *menuBar, SDL_Point mousePosition,
                    int *isChange) {

    switch (event.type) {

    case SDL_MOUSEMOTION:

        for (int i = 0; i < 5; ++i)
        {
            if (ifPointInSquare(menuBar->tab[i].clip, mousePosition))
            {
                if (menuBar->tab[i].isHovered == 0)
                {
                    *isChange = 1;
                    menuBar->tab[i].isHovered = 1;
                }
            }
            else {

                if (menuBar->tab[i].isHovered == 1)
                {
                    *isChange = 1;
                    menuBar->tab[i].isHovered = 0;
                }
            }

            if (menuBar->actualTab != i && menuBar->tab[i].colorAnimOut == 0
                    && menuBar->tab[i].colorAnimIn == 0)
            {
                if (menuBar->tab[i].isHovered == 1 )
                {
                    menuBar->tab[i].colorProp = 72;
                    menuBar->tab[i].textAlpha = 128;
                }
                else {

                    menuBar->tab[i].colorProp = 60;
                    menuBar->tab[i].textAlpha = 128;
                }
            }
        }

        break;

    case SDL_MOUSEBUTTONDOWN:

        for (int i = 0; i < 5; ++i)
        {
            if (menuBar->tab[i].isHovered == 1 && i != menuBar->actualTab)
            {
                menuBar->tab[menuBar->actualTab].colorAnimOut = 1;

                menuBar->tabChanged = menuBar->actualTab;

                menuBar->actualTab = i;

                menuBar->tab[i].colorAnimIn = 1;

                *isChange = 1;
            }
        }

        break;
    }
}

void menuBarAnimation(mainBar *menuBar, int *isChange, int actual,
                      int alpha) {

    int crossValue = 50;

    if (menuBar->openAnimation == 1)
    {
        if (menuBar->elementsAlpha <= 255)
        {
            if (actual > menuBar->prev2 + 10)
            {
                menuBar->prev2 = SDL_GetTicks();

                menuBar->elementsAlpha =
                    menuBar->elementsAlpha + 16;

                if (menuBar->elementsAlpha >= 255)
                {
                    menuBar->elementsAlpha = 255;
                    menuBar->openAnimation = 0;
                }

                *isChange = 1;
            }
        }
    }

    if (menuBar->animStart == 1) {

        if (menuBar->tab[0].alphaProp <= 100)
        {
            if (actual > menuBar->prev + 10)
            {
                menuBar->prev = SDL_GetTicks();

                menuBar->tab[0].alphaProp =
                    menuBar->tab[0].alphaProp + 4;

                if (menuBar->tab[0].alphaProp >= 100)
                {
                    menuBar->tab[0].alphaProp = 100;
                }

                *isChange = 1;

                if (menuBar->tab[0].alphaProp >= crossValue)
                {
                    menuBar->tab[1].alphaProp =
                        menuBar->tab[1].alphaProp + 4;

                    if (menuBar->tab[1].alphaProp >= 100)
                    {
                        menuBar->tab[1].alphaProp = 100;
                    }

                    *isChange = 1;

                    if (menuBar->tab[1].alphaProp >= crossValue)
                    {
                        menuBar->tab[2].alphaProp =
                            menuBar->tab[2].alphaProp + 4;

                        if (menuBar->tab[2].alphaProp >= 100)
                        {
                            menuBar->tab[2].alphaProp = 100;
                        }

                        *isChange = 1;

                        if (menuBar->tab[2].alphaProp >= crossValue)
                        {
                            menuBar->tab[3].alphaProp =
                                menuBar->tab[3].alphaProp + 4;

                            if (menuBar->tab[3].alphaProp >= 100)
                            {
                                menuBar->tab[3].alphaProp = 100;
                            }

                            *isChange = 1;

                            if (menuBar->tab[3].alphaProp >= crossValue)
                            {
                                menuBar->tab[4].alphaProp =
                                    menuBar->tab[4].alphaProp + 4;

                                if (menuBar->tab[4].alphaProp >= 100)
                                {
                                    menuBar->tab[4].alphaProp = 100;
                                    menuBar->animStart = 0;
                                }

                                *isChange = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    else {

        if (menuBar->tab[menuBar->actualTab].colorAnimIn == 1)
        {
            if (menuBar->tab[menuBar->actualTab].colorProp < 100 ||
                    menuBar->tab[menuBar->actualTab].textAlpha < 255)
            {
                if (actual > menuBar->prev + 10)
                {
                    menuBar->prev = SDL_GetTicks();

                    menuBar->tab[menuBar->actualTab].colorProp =
                        menuBar->tab[menuBar->actualTab].colorProp + 3;

                    menuBar->tab[menuBar->actualTab].textAlpha =
                        menuBar->tab[menuBar->actualTab].textAlpha + 8;

                    if (menuBar->tab[menuBar->actualTab].colorProp >= 100)
                    {
                        menuBar->tab[menuBar->actualTab].colorProp = 100;
                    }

                    if (menuBar->tab[menuBar->actualTab].textAlpha >= 255)
                    {
                        menuBar->tab[menuBar->actualTab].textAlpha = 255;
                    }

                    if (menuBar->tab[menuBar->actualTab].textAlpha == 255 &&
                            menuBar->tab[menuBar->actualTab].colorProp == 100)
                    {
                        menuBar->tab[menuBar->actualTab].colorAnimIn = 0;
                    }

                    *isChange = 1;
                }
            }
        }

        for (int i = 0; i < 5; ++i)
        {
            if (menuBar->tab[i].colorAnimOut == 1)
            {
                if (menuBar->tab[i].colorProp > 60 ||
                        menuBar->tab[i].textAlpha > 128)
                {
                    if (actual > menuBar->prev + 10)
                    {
                        menuBar->prev = SDL_GetTicks();

                        menuBar->tab[i].colorProp =
                            menuBar->tab[i].colorProp - 3;

                        menuBar->tab[i].textAlpha =
                            menuBar->tab[i].textAlpha - 8;

                        if (menuBar->tab[i].colorProp <= 60)
                        {
                            menuBar->tab[i].colorProp = 60;
                        }

                        if (menuBar->tab[i].textAlpha <= 128)
                        {
                            menuBar->tab[i].textAlpha = 128;
                        }

                        if (menuBar->tab[i].textAlpha == 128 &&
                                menuBar->tab[i].colorProp == 60)
                        {
                            menuBar->tab[i].colorAnimOut = 0;
                        }

                        *isChange = 1;
                    }
                }
            }
        }
    }
}

void mainMenuButtonRender(SDL_Renderer **renderer, button btn, int shadowSize) {

    int colorOffset = 20;

    drawRoundedBoxShadow(&*renderer, btn.clip.x,
                         btn.clip.y, btn.clip.x + btn.clip.w,
                         btn.clip.y + btn.clip.h, sf(5), shadowSize, btn.color.a);

    roundedBoxColor(*renderer, btn.clip.x, btn.clip.y,
                    btn.clip.x + btn.clip.w , btn.clip.y + btn.clip.h,
                    sf(5), SDL_FOURCC( prop(btn.color.r, (100 - colorOffset) +
                                            (btn.isSelected * colorOffset)),
                                       prop(btn.color.g, (100 - colorOffset) +
                                            (btn.isSelected * colorOffset)),
                                       prop(btn.color.b, (100 - colorOffset) +
                                            (btn.isSelected * colorOffset)),
                                       btn.color.a));

    SDL_SetTextureAlphaMod(btn.label.texture, btn.color.a);

    SDL_RenderCopy(*renderer, btn.label.texture, NULL, &btn.label.clip);

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

}

button mainMenuCreateButtonRGBA(SDL_Renderer **renderer , int fontSize,
                                char *text, int x, int y, int w, int h, int id,
                                int r, int g, int b, int a) {

    button element;

    element.clip.x = x;
    element.clip.y = y;
    element.clip.w = w;
    element.clip.h = h;

    element.color.r = r;
    element.color.g = g;
    element.color.b = b;
    element.color.a = a;

    element.label = createTextTexture(&*renderer, text, FONT_STANDARD, fontSize,
                                      element.clip.x + (element.clip.w / 2),
                                      element.clip.y + (element.clip.h / 2),
                                      HCENTER | VCENTER);

    element.activated = 1;

    element.isSelected = 0;

    element.alpha = 255;

    element.id = id;

    return element;
}

void selectBarGetState(selectBar *bar, SDL_Event event, SDL_Point mousePosition,
                       int *isChange) {

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:

        if (bar->deploy == 0)
        {
            if (ifPointInSquare(bar->clip, mousePosition) == 1 ) {

                bar->deployAnim = 1;
                *isChange = 1;
            }
        }
        else {

            for (int i = 1; i < bar->nbItems; ++i)
            {
                if (ifPointInSquare(bar->item[i].clip, mousePosition))
                {
                    setSelectBarValue(bar, bar->item[i].itemId);

                    bar->changedValue = 1;
                }
            }

            bar->retractAnim = 1;
        }

        break;

    case SDL_MOUSEMOTION:

        if (bar->deploy == 1)
        {
            for (int i = 0; i < bar->nbItems; ++i)
            {
                if (ifPointInSquare(bar->item[i].clip, mousePosition))
                {
                    if (bar->item[i].hovered == 0)
                    {
                        bar->item[i].hovered = 1;
                        *isChange = 1;
                    }
                }
                else {

                    if (bar->item[i].hovered == 1)
                    {
                        bar->item[i].hovered = 0;
                        *isChange = 1;
                    }
                }
            }

            if (bar->hovered == 1)
            {
                bar->hovered = 0;
            }
        }
        else {

            if (ifPointInSquare(bar->clip, mousePosition) && bar->hovered == 0)
            {
                bar->hovered = 1;
                *isChange = 1;
            }
            else if (bar->hovered == 1)
            {
                bar->hovered = 0;
                *isChange = 1;
            }
        }

        break;
    }
}

void selectBarAnimation(selectBar *bar, int *isChange, int actual) {

    if (bar->deployAnim == 1)
    {
        if (bar->animProp < 100)
        {
            if (actual > bar->prev + 10)
            {
                bar->prev = SDL_GetTicks();

                bar->animProp =
                    bar->animProp + 16;

                if (bar->animProp >= 100)
                {
                    bar->animProp = 100;
                    bar->deploy = 1;
                    bar->deployAnim = 0;
                }

                *isChange = 1;
            }
        }
    }
    else if (bar->retractAnim == 1)
    {
        if (bar->animProp > 0)
        {
            if (actual > bar->prev + 10)
            {
                bar->prev = SDL_GetTicks();

                bar->animProp =
                    bar->animProp - 16;

                if (bar->animProp <= 0)
                {
                    bar->animProp = 0;
                    bar->retractAnim = 0;
                    bar->deploy = 0;

                    for (int i = 0; i < bar->nbItems; ++i)
                    {
                        bar->item[i].hovered = 0;
                    }
                }

                *isChange = 1;
            }
        }
    }
}

void updateSelectbarItems(selectBar *bar) {

    double ratio = 0;

    int lineThickness = sf(5);

    for (int i = 0; i < bar->nbItems; ++i)
    {
        bar->item[i].clip.x = bar->clip.x;
        bar->item[i].clip.y = bar->clip.y + (i * bar->clip.h);
        bar->item[i].clip.w = bar->clip.w;
        bar->item[i].clip.h = bar->clip.h;

        bar->item[i].text.clip.x = bar->clip.x + sf(5);
        bar->item[i].text.clip.y = bar->clip.y + (i * bar->clip.h);

        if (i != 0)
        {
            bar->item[i].text.clip.y = bar->item[i].text.clip.y +
                                       ((bar->item[i].clip.h / 2) -
                                        (bar->item[i].text.clip.h / 2)) +
                                       lineThickness + sf(2) ;

            bar->item[i].clip.y = bar->item[i].clip.y + lineThickness;
        }

        SDL_QueryTexture(bar->item[i].texture, NULL, NULL,
                         &bar->item[i].textureClip.w,
                         &bar->item[i].textureClip.h);

        ratio = bar->item[i].textureClip.w / (double)bar->item[i].textureClip.h;

        bar->item[i].textureClip.h = bar->item[i].clip.h * 0.75;
        bar->item[i].textureClip.w = bar->item[i].textureClip.h * ratio;
        bar->item[i].textureClip.y = bar->item[i].clip.y +
                                     (bar->item[i].clip.h / 2) -
                                     (bar->item[i].textureClip.h / 2);

        bar->item[i].textureClip.x = (bar->item[i].clip.x + bar->item[i].clip.w) -
                                     (bar->item[i].textureClip.y  -
                                      bar->item[i].clip.y) -
                                     bar->item[i].textureClip.w;
    }
}

void renderSelectBar(SDL_Renderer **renderer, selectBar bar) {

    int colorProp = 150;

    int lineThickness = sf(5);

    SDL_Rect tempRect = bar.item[1].clip;

    tempRect.h = prop((bar.clip.h * (bar.nbItems - 1)),
                      bar.animProp);

    SDL_SetRenderDrawColor(*renderer, prop(bar.color.r, colorProp),
                           prop(bar.color.g, colorProp),
                           prop(bar.color.b, colorProp), bar.color.a);

    SDL_RenderFillRect(*renderer, &tempRect);

    SDL_SetRenderDrawColor(*renderer, bar.color.r, bar.color.g,
                           bar.color.b, bar.color.a);

    for (int i = 1; i < bar.nbItems; ++i)
    {
        if ((bar.clip.h * i) <= prop((bar.clip.h * bar.nbItems), bar.animProp ))
        {

            SDL_RenderDrawLine(*renderer, bar.item[i].clip.x,
                               bar.item[i].clip.y,
                               bar.item[i].clip.x + bar.item[i].clip.w,
                               bar.item[i].clip.y);

            if (bar.item[i].hovered == 1)
            {
                tempRect = bar.item[i].clip;

                tempRect.y = tempRect.y;

                SDL_SetRenderDrawColor(*renderer, bar.color.r, bar.color.g,
                                       bar.color.b, bar.color.a);

                SDL_RenderFillRect(*renderer, &tempRect);
            }

            bar.item[i].text.clip.y = bar.item[i].text.clip.y;

            bar.item[i].text.clip.h = bar.item[i].text.clip.h * 0.8;
            bar.item[i].text.clip.w = bar.item[i].text.clip.w * 0.8;

            SDL_RenderCopy(*renderer, bar.item[i].text.texture, NULL,
                           &bar.item[i].text.clip);

            SDL_RenderCopy(*renderer, bar.item[i].texture, NULL,
                           &bar.item[i].textureClip);
        }
    }

    SDL_SetTextureColorMod(bar.item[0].text.texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(bar.item[0].text.texture, bar.color.a);

    SDL_SetRenderDrawColor(*renderer, bar.color.r, bar.color.g,
                           bar.color.b, bar.color.a);

    for (int i = 0; i < lineThickness; ++i)
    {
        SDL_RenderDrawLine(*renderer, bar.clip.x, bar.clip.y + bar.clip.h + i,
                           bar.clip.x + bar.clip.w,
                           bar.clip.y + bar.clip.h + i);
    }

    SDL_RenderCopy(*renderer, bar.item[0].text.texture, NULL,
                   &bar.item[0].text.clip);

    SDL_SetTextureAlphaMod(bar.arrowTexture, bar.color.a);

    SDL_RenderCopyEx(*renderer, bar.arrowTexture, NULL, &bar.arrowClip,
                     prop(180, bar.animProp), NULL, SDL_FLIP_NONE);
}

selectBar createSelectBarRGBA(SDL_Renderer **renderer, int x, int y, int h,
                              int w, int nbItems, selectBarItem *items, int r,
                              int g, int b, int a) {

    selectBar bar;

    bar.clip.x = x;
    bar.clip.y = y;
    bar.clip.h = h;
    bar.clip.w = w;

    bar.color.r = r;
    bar.color.g = g;
    bar.color.b = b;
    bar.color.a = a;

    bar.nbItems = nbItems;
    bar.deploy = 0;
    bar.animProp = 0;

    for (int i = 0; i < nbItems; ++i)
    {
        bar.item[i].itemId = i;
        bar.item[i].texture = items[i].texture;
        bar.item[i].text = items[i].text;
    }

    bar.arrowTexture = loadTexture(&*renderer, "../images/arrow.png");

    bar.arrowClip.h = bar.clip.h / 2;
    bar.arrowClip.w = (bar.clip.h / 2);
    bar.arrowClip.y = bar.clip.y + (bar.clip.h / 2 - (bar.arrowClip.h / 2));
    bar.arrowClip.x = (bar.clip.x + bar.clip.w) -
                      (bar.arrowClip.y - bar.clip.y) - bar.arrowClip.w - sf(5);

    bar.prev = 0;

    return bar;
}

void renderGraduateBar(SDL_Renderer **renderer, graduateBar bar) {

    int colorProp = 80;

    roundedBoxColor(*renderer, bar.clip.x, bar.clip.y,
                    bar.clip.x + bar.clip.w ,
                    bar.clip.y + bar.clip.h, sf(5),
                    SDL_FOURCC( bar.color.r,
                                bar.color.g, bar.color.b, bar.color.a));

    roundedBoxColor(*renderer, bar.prevButton.clip.x, bar.prevButton.clip.y,
                    bar.nextButton.clip.x + bar.nextButton.clip.w ,
                    bar.nextButton.clip.y + bar.nextButton.clip.h, sf(5),
                    SDL_FOURCC( prop(bar.color.r, colorProp),
                                prop(bar.color.g, colorProp), prop(bar.color.b,
                                        colorProp), bar.color.a));

    bar.nextButton.color.a = bar.color.a;
    bar.prevButton.color.a = bar.color.a;

    mainMenuButtonTextureRender(&*renderer, bar.prevButton, 90);
    mainMenuButtonTextureRender(&*renderer, bar.nextButton, 270);

    SDL_SetTextureAlphaMod(bar.name.texture, bar.color.a);
    SDL_SetTextureAlphaMod(bar.textValue.texture, bar.color.a);

    SDL_RenderCopy(*renderer, bar.name.texture, NULL, &bar.name.clip);
    SDL_RenderCopy(*renderer, bar.textValue.texture, NULL, &bar.textValue.clip);

}

graduateBar createGraduateBarRGBA(SDL_Renderer **renderer, int minValue,
                                  int maxValue, char *name, int x, int y, int h,
                                  int w, int r, int g, int b, int a) {
    char temp[4];

    graduateBar element;

    element.clip.x = x;
    element.clip.y = y;
    element.clip.h = h;
    element.clip.w = w;

    element.name = createTextTexture(&*renderer, name, FONT_STANDARD, sf(15),
                                     element.clip.x + sf(5), element.clip.y,
                                     LEFT | BOTTOM);

    element.changedValue = 0;

    element.minValue = minValue;
    element.maxValue = maxValue;
    element.value = minValue;

    element.color.r = r;
    element.color.g = g;
    element.color.b = b;
    element.color.a = a;

    int buttonSize = sf(20);

    element.nextButton = mainMenuCreateTextureButtonRGBA(&*renderer ,
                         "../images/arrow.png", sf(15),
                         element.clip.x + element.clip.w - buttonSize -
                         ((element.clip.y + (element.clip.h / 2) -
                           ((element.clip.h / 2) / 2) - element.clip.y) / 2),
                         element.clip.y + (element.clip.h / 2) -
                         (buttonSize / 2), buttonSize, buttonSize, 0,
                         255, 255 , 255, element.color.a);

    element.prevButton = mainMenuCreateTextureButtonRGBA(&*renderer ,
                         "../images/arrow.png", sf(15),
                         element.nextButton.clip.x - buttonSize -
                         ((element.clip.y + (element.clip.h / 2) -
                           ((element.clip.h / 2) / 2) - element.clip.y) / 2),
                         element.nextButton.clip.y, buttonSize, buttonSize,
                         1, 255, 255 , 255, element.color.a);

    sprintf(temp, "%d", element.value);

    element.textValue = createTextTexture(&*renderer, temp, FONT_STANDARD,
                                          sf(15), ((element.prevButton.clip.x -
                                                  element.clip.x) / 2) +
                                          element.clip.x, element.clip.y +
                                          (element.clip.h / 2),
                                          VCENTER | HCENTER);

    return element;
}

void renderNewGameForm(SDL_Renderer **renderer, form1 newGameForm) {

    newGameForm.clip.h = newGameForm.clip.h +
                         prop(newGameForm.finalClip.h -
                              newGameForm.startClip.h, newGameForm.animProp);

    newGameForm.clip.w = newGameForm.clip.w +
                         prop(newGameForm.finalClip.w -
                              newGameForm.startClip.w, newGameForm.animProp);

    newGameForm.clip.y = newGameForm.clip.y +
                         prop(newGameForm.finalClip.y -
                              newGameForm.startClip.y, newGameForm.animProp);

    drawRoundedBoxShadow(&*renderer, newGameForm.clip.x, newGameForm.clip.y,
                         newGameForm.clip.x + newGameForm.clip.w,
                         newGameForm.clip.y + newGameForm.clip.h, sf(5), 12,
                         newGameForm.color.a);

    roundedBoxColor(*renderer, newGameForm.clip.x, newGameForm.clip.y,
                    newGameForm.clip.x + newGameForm.clip.w ,
                    newGameForm.clip.y + newGameForm.clip.h, sf(5),
                    SDL_FOURCC( newGameForm.color.r,
                                newGameForm.color.g, newGameForm.color.b, newGameForm.color.a));

    int barHeight = newGameForm.finalClip.y + sf(48);
    int barWidth =  prop(newGameForm.finalClip.w - sf(50),
                         newGameForm.alphaProp);
    int x1 = (newGameForm.finalClip.x + (newGameForm.finalClip.w / 2)) -
             (barWidth / 2);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, newGameForm.color.a);

    for (int i = 0; i < 2; ++i)
    {
        SDL_RenderDrawLine(*renderer, x1 , barHeight + i, x1 + barWidth ,
                           barHeight + i);
    }

    SDL_SetTextureAlphaMod(newGameForm.name.texture, prop(newGameForm.color.a,
                           newGameForm.alphaProp));

    newGameForm.playButton.color.a = prop(newGameForm.color.a, newGameForm.alphaProp);

    mainMenuButtonRender(&*renderer, newGameForm.playButton, 4);

    SDL_RenderCopy(*renderer, newGameForm.name.texture, NULL,
                   &newGameForm.name.clip);

    newGameForm.smartBar.color.a = prop(newGameForm.color.a, newGameForm.alphaProp);

    scrollBarRender(&*renderer, newGameForm.smartBar);

    newGameForm.difficultyBar.color.a = prop(newGameForm.color.a, newGameForm.alphaProp);
    renderSelectBar(&*renderer, newGameForm.difficultyBar);

    newGameForm.strokeBar.color.a = prop(newGameForm.color.a, newGameForm.alphaProp);
    renderGraduateBar(&*renderer, newGameForm.strokeBar);
}

void renderLogoRect(SDL_Renderer **renderer, logoRect logo) {

    for (int i = 0; i < 4; ++i)
    {
        SDL_SetTextureAlphaMod(logo.label[i].texture, logo.color.a);

        SDL_RenderCopy(*renderer, logo.label[i].texture, NULL,
                       &logo.label[i].clip);

    }
}

void updateTextTexture(SDL_Renderer **renderer, text * element,
                       const char textToPrint[], const char fontLocation[], int fontSize) {

    SDL_Rect previousClip = element->clip;

    SDL_Color fontColor = {255, 255, 255};
    TTF_Font *police = TTF_OpenFont(fontLocation, fontSize);

    SDL_Surface *surface = TTF_RenderUTF8_Blended(police, textToPrint,
                           fontColor);

    element->texture = SDL_CreateTextureFromSurface(*renderer, surface);

    SDL_GetClipRect(surface, &element->clip);

    switch (0xf & element->plcmtMode) {
    case LEFT:

        element->clip.x = previousClip.x;
        break;

    case HCENTER:

        element->clip.x = (previousClip.x + previousClip.w / 2) - (element->clip.w / 2);
        break;

    case RIGHT:

        element->clip.x = (previousClip.x + previousClip.w) - element->clip.w;
        break;
    }

    switch (0xf0 & element->plcmtMode) {
    case TOP:

        element->clip.y = previousClip.y;
        break;

    case VCENTER:

        element->clip.y = (previousClip.y + previousClip.h / 2) - (element->clip.h / 2);
        break;

    case BOTTOM:

        element->clip.y = (previousClip.y + previousClip.h) - element->clip.h;
        break;
    }

    TTF_CloseFont(police);
    SDL_FreeSurface(surface);
}

void getProfileList(profile *list);

int getThreadStatut();

void formatTime(int rawtime, char * output) {

    struct tm * timeinfo;

    timeinfo = localtime ( (time_t*)&rawtime );
    sprintf(output, "%d/%02d/%d", timeinfo->tm_mday,
            timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
}

void formatGameTime(int rawtime, char * output) {

    int totalSeconds = rawtime / 1000;

    int mili = rawtime - (totalSeconds * 1000);

    int totaMinutes = totalSeconds / 60;

    int sec = totalSeconds - totaMinutes * 60;

    int hours = totaMinutes / 60;

    int min = totaMinutes - hours * 60;

    sprintf(output, "%d:%02d:%02d:%04d", hours, min, sec, mili );
}

void updateScoreBoardItems(SDL_Renderer **renderer, scoreBoard *board) {

    if (!getThreadStatut())
    {
        int hexValue = (board->sortID << 4) | board->isReverse;

        char instruction[20];

        sprintf(instruction, "getList-%x", hexValue);

        netPacket packet = sandPackedRequest(instruction, &packet);

        profile *profileList = malloc(board->nbLines * sizeof(profile));

        char temp[6][25];

        for (int i = 0; i < board->nbLines; ++i)
        {
            profileList[i] = initBlankProfile();

            if (packet.received)
            {
                profileList[i] = packet.bestClients[i];
            }

            if (!strcmp(profileList[i].nick, ""))
            {
                strcpy(temp[0], "-----");
            }
            else {

                strcpy(temp[0], profileList[i].nick);
            }

            if (profileList[i].bestTime == 0)
            {
                strcpy(temp[1], "-----");
            }
            else {

                formatGameTime(profileList[i].bestTime, temp[1]);
            }

            if (profileList[i].averageTime == 0)
            {
                strcpy(temp[2], "-----");
            }
            else {

                formatGameTime(profileList[i].averageTime, temp[2]);
            }

            if (profileList[i].firstLog == 0)
            {
                strcpy(temp[3], "-----");
            }
            else {

                formatTime(profileList[i].firstLog, temp[3]);
            }

            if (profileList[i].defeats == 0 && profileList[i].wins != 0)
            {
                strcpy(temp[4], "100");
            }
            else if (profileList[i].defeats == 0 && profileList[i].wins == 0) {

                strcpy(temp[4], "-----");
            }
            else {

                sprintf(temp[4], "%1.2f", (profileList[i].wins / (double)(profileList[i].wins + profileList[i].defeats)) * 100 );
            }

            if (profileList[i].wins == 0)
            {
                strcpy(temp[5], "-----");
            }
            else {

                sprintf(temp[5], "%d", profileList[i].wins);
            }

            for (int j = 0; j < board->nbCols; ++j)
            {
                SDL_DestroyTexture(board->item[i].label[j].texture);

                board->item[i].label[j] = createTextTexture(&*renderer, temp[j],
                                          "../fonts/Gonsterrat.ttf", sf(15),
                                          board->cols[j].lines[i].clip.x +
                                          (board->cols[j].lines[i].clip.w / 2),
                                          board->cols[j].lines[i].clip.y +
                                          (board->cols[j].lines[i].clip.h / 2),
                                          VCENTER | HCENTER);
            }
        }

        free(profileList);
    }
}

scoreBoard scoreBoardInit(SDL_Renderer **renderer, SDL_Color color) {

    scoreBoard element;

    element.clip.x = sf(100);
    element.clip.y = sf(130);
    element.clip.h = sf(720) - element.clip.y - sf(100);
    element.clip.w = sf(1280) - (sf(100) * 2);

    element.nbCols = 6;
    element.nbLines = 11;

    char labelStrings[6][20] = {0};

    strcpy(labelStrings[0], gs("Nickname"));
    strcpy(labelStrings[1], gs("Best Time"));
    strcpy(labelStrings[2], gs("Average Time"));
    strcpy(labelStrings[3], gs("First connection"));
    strcpy(labelStrings[4], gs("Ratio  V / D"));
    strcpy(labelStrings[5], gs("Victories"));

    for (int i = 0; i < element.nbCols; ++i)
    {
        element.cols[i].clip.w = element.clip.w / element.nbCols;

        element.cols[i].clip.x = (element.cols[i].clip.w * i) + element.clip.x;

        element.cols[i].clip.y = element.clip.y;

        element.cols[i].clip.h = element.clip.h;

        element.cols[i].label = createTextTexture(&*renderer, labelStrings[i],
                                FONT_STANDARD, sf(20), element.cols[i].clip.x +
                                (element.cols[i].clip.w / 2),
                                element.cols[i].clip.y + sf(20), HCENTER | BOTTOM);

        element.cols[i].sortButton.clip.x = element.cols[i].clip.x;
        element.cols[i].sortButton.clip.y = element.cols[i].clip.y - (sf(20) / 2);
        element.cols[i].sortButton.clip.w = element.cols[i].clip.w;
        element.cols[i].sortButton.clip.h = sf(40);
        element.cols[i].sortButton.id = i;
        element.cols[i].sortButton.isSelected = 0;

        for (int j = 0; j < element.nbLines; ++j)
        {
            element.cols[i].lines[j].clip.x = element.cols[i].clip.x;
            element.cols[i].lines[j].clip.w = element.cols[i].clip.w;
            element.cols[i].lines[j].clip.h = sf(40);
            element.cols[i].lines[j].clip.y = element.cols[i].clip.y +
                                              (element.cols[i].lines[j].clip.h *
                                               j) + sf(40);

            element.cols[i].lines[j].alpha = 0;
            element.cols[i].lines[j].hovered = 0;
        }
    }

    for (int i = 0; i < element.nbLines; ++i)
    {
        for (int j = 0; j < element.nbCols; ++j)
        {
            element.item[i].label[j] = createTextTexture(&*renderer, "-----",
                                       "../fonts/Gonsterrat.ttf", sf(15),
                                       element.cols[j].lines[i].clip.x +
                                       (element.cols[j].lines[i].clip.w / 2),
                                       element.cols[j].lines[i].clip.y +
                                       (element.cols[j].lines[i].clip.h / 2),
                                       VCENTER | HCENTER);
        }
    }

    element.color = color;

    element.sortID = 4;

    element.isReverse = 0;

    element.animIn = 1;
    element.animOut = 0;
    element.animFinished = 0;
    element.animActualCol = 0;

    return element;
}

void renderScoreBoard(SDL_Renderer **renderer, scoreBoard board) {

    int animProp = ((double)board.color.a / 255) * 100;

    int barHeight = board.clip.y + sf(30);
    int barWidth =  prop(board.clip.w,
                         animProp);
    int x1 = (board.clip.x + (board.clip.w / 2)) -
             (barWidth / 2);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    SDL_RenderDrawLine(*renderer, x1 , barHeight, x1 + barWidth ,
                       barHeight );

    for (int i = 0; i < board.nbCols; ++i)
    {
        SDL_SetTextureAlphaMod(board.cols[i].label.texture, board.color.a);

        SDL_RenderCopy(*renderer, board.cols[i].label.texture, NULL,
                       &board.cols[i].label.clip);

        if (board.cols[i].sortButton.isSelected)
        {
            roundedBoxColor(*renderer, board.cols[i].sortButton.clip.x, board.cols[i].sortButton.clip.y,
                            board.cols[i].sortButton.clip.x +  board.cols[i].sortButton.clip.w ,
                            board.cols[i].sortButton.clip.y + board.cols[i].sortButton.clip.h, sf(5),
                            SDL_FOURCC(255, 255, 255, 24));
        }
    }

    for (int i = 0; i < board.nbLines; ++i)
    {
        for (int j = 0; j < board.nbCols; ++j)
        {
            SDL_SetTextureAlphaMod(board.item[i].label[j].texture, board.cols[j].lines[i].alpha);

            SDL_RenderCopy(*renderer, board.item[i].label[j].texture, NULL,
                           &board.item[i].label[j].clip);

            if (board.cols[j].lines[i].hovered)
            {
                roundedBoxColor(*renderer, board.cols[j].lines[i].clip.x, board.cols[j].lines[i].clip.y,
                                board.cols[j].lines[i].clip.x +  board.cols[j].lines[i].clip.w ,
                                board.cols[j].lines[i].clip.y + board.cols[j].lines[i].clip.h, sf(5),
                                SDL_FOURCC(255, 255, 255, 24));
            }
        }
    }
}

void updateScrollBarText(SDL_Renderer **renderer, scrollBar *bar) {

    char temp[5];

    sprintf(temp, "%d%%", bar->value);

    SDL_DestroyTexture(bar->bottomText.texture);

    updateTextTexture(&*renderer, &bar->bottomText,
                      temp, "../fonts/Gonsterrat.ttf",
                      sf(10));
}

panel settingsPanelInit(SDL_Renderer **renderer, SDL_Color tabColor) {

    panel element;

    element.clip.x = sf(70);
    element.clip.y = sf(180);
    element.clip.h = sf(720) - sf(140) - sf(80);
    element.clip.w = sf(1280) - (sf(70) * 2);

    int offset = sf(25);

    char tempString[30];

    char temp[6][30];

    strcpy(temp[0], gs("Display"));
    strcpy(temp[1], gs("Interface"));
    strcpy(temp[2], gs("Sounds"));
    strcpy(temp[3], gs("Profile"));

    for (int i = 0; i < 4; ++i)
    {
        element.categoryClip[i].w = (element.clip.w - (3 * offset)) / 4 ;
        element.categoryClip[i].h = element.clip.h - sf(250);
        element.categoryClip[i].y = element.clip.y;
        element.categoryClip[i].x = element.clip.x +
                                    (i * element.categoryClip[i].w) +
                                    (i * offset);

        element.categoryLabel[i] = createTextTexture(&*renderer, temp[i],
                                   FONT_STANDARD, sf(20),
                                   element.categoryClip[i].x +
                                   (element.categoryClip[i].w / 2),
                                   element.categoryClip[i].y + (sf(20) * 2),
                                   HCENTER | BOTTOM);
    }

    //init category 1 : display

    //init displayMode selectBar

    strcpy(temp[0], gs("FullScreen"));
    strcpy(temp[1], gs("FullScreen-Windowed"));
    strcpy(temp[2], gs("Windowed"));

    selectBarItem displayBarItems[3];

    for (int i = 0; i < 3; ++i)
    {
        sprintf(tempString, "../images/selectBar/displayBar/resBar-%d.jpg", i);

        displayBarItems[i].texture = loadTexture(&*renderer, tempString);

        displayBarItems[i].text = createTextTexture(&*renderer, temp[i],
                                  FONT_STANDARD, sf(16), sf(675), sf(340),
                                  LEFT | BOTTOM);
    }

    element.displayBar = createSelectBarRGBA(&*renderer,
                         element.categoryClip[0].x +
                         (element.categoryClip[0].w / 2) - sf(110),
                         element.categoryClip[0].y + sf(80), sf(20), sf(220), 3,
                         displayBarItems, 93, 109, 126, 255);

    setSelectBarValue(&element.displayBar, getUserSettings().displayMode);

    //init displayRes selectBar

    selectBarItem resBarItems[6];

    int nbResItems = 0;

    for (int i = 0; i < 6; ++i)
    {
        if (getScreenWidth() >= getScreenWidth_DisplayRes(i))
        {
            sprintf(tempString, "../images/selectBar/resBar/%dp.jpg",
                    (int)(getScreenWidth_DisplayRes(i) / (16 / (double)9)));

            resBarItems[nbResItems].texture = loadTexture(&*renderer,
                                              tempString );

            sprintf(tempString, "%dx%d", getScreenWidth_DisplayRes(i) ,
                    (int)(getScreenWidth_DisplayRes(i) / (16 / (double)9)));

            resBarItems[nbResItems].text = createTextTexture(&*renderer,
                                           tempString, FONT_STANDARD, sf(16),
                                           sf(675), sf(340), LEFT | BOTTOM);
            nbResItems++;
        }
    }

    element.resBar = createSelectBarRGBA(&*renderer, element.categoryClip[0].x +
                                         (element.categoryClip[0].w / 2) -
                                         sf(110), element.categoryClip[0].y +
                                         sf(130), sf(20), sf(120), nbResItems,
                                         resBarItems, 93, 109, 126, 255);

    setSelectBarValue(&element.resBar, getUserSettings().displayRes);

    //init category 2 : interface

    //init key selector

    element.rotateKeySelector.clip.x = element.categoryClip[1].x + sf(30);
    element.rotateKeySelector.clip.y = element.categoryClip[1].y + sf(70);
    element.rotateKeySelector.clip.h = sf(30);
    element.rotateKeySelector.clip.w = sf(120);

    element.rotateKeySelector.key = createTextTexture(&*renderer,
                                    SDL_GetKeyName(getUserSettings().rotateKey), FONT_STANDARD, sf(15),
                                    element.rotateKeySelector.clip.x + sf(10),
                                    element.rotateKeySelector.clip.y +
                                    (element.rotateKeySelector.clip.h / 2),
                                    LEFT | VCENTER);

    element.rotateKeySelector.isHovered = 0;
    element.rotateKeySelector.isFocus = 0;

    element.rotateKeySelector.color.r = 93;
    element.rotateKeySelector.color.g = 109;
    element.rotateKeySelector.color.b = 126;

    //init langBar

    strcpy(temp[0], gs("English"));
    strcpy(temp[1], gs("French"));
    strcpy(temp[2], gs("Spanish"));
    strcpy(temp[3], gs("Portuguese"));
    strcpy(temp[4], gs("German"));
    strcpy(temp[5], gs("Italian"));

    selectBarItem langBarItems[6];

    for (int i = 0; i < 6; ++i)
    {
        sprintf(tempString, "../images/selectBar/langBar/langBar-%d.jpg", i);

        langBarItems[i].texture = loadTexture(&*renderer, tempString);

        langBarItems[i].text = createTextTexture(&*renderer, temp[i],
                               FONT_STANDARD, sf(16), sf(675), sf(340),
                               LEFT | BOTTOM);
    }

    element.langBar = createSelectBarRGBA(&*renderer,
                                          element.categoryClip[1].x +
                                          (element.categoryClip[1].w / 2) - sf(110),
                                          element.categoryClip[1].y + sf(130), sf(20), sf(220), 6,
                                          langBarItems, 93, 109, 126, 255);

    setSelectBarValue(&element.langBar, getUserSettings().selectedLanguage);

    //init category 3 : sounds

    element.musicBar = createScrollBarRGBA(&*renderer, gs("Music"),
                                           getUserSettings().musicVolume, 1,
                                           sf(200), sf(10), element.categoryClip[2].x + (element.categoryClip[2].w / 2) - sf(200) / 2, element.categoryClip[2].y + sf(100),
                                           sf(8), 93, 109, 126, 0);

    element.effectsBar = createScrollBarRGBA(&*renderer, gs("Effects"),
                         getUserSettings().effectsVolume,
                         2, sf(200), sf(10), element.categoryClip[2].x + (element.categoryClip[2].w / 2) - sf(200) / 2 , element.categoryClip[2].y + sf(165) ,
                         sf(8), 93, 109, 126, 0);

    element.color = tabColor;

    int applyButtonWidth = sf(140);
    int applyButtonHeight = sf(40);

    element.applyButton = mainMenuCreateButtonRGBA(&*renderer, sf(20),
                          gs("Apply"), element.clip.x + element.clip.w
                          - applyButtonWidth,
                          element.clip.y + element.clip.h -
                          applyButtonHeight - sf(150), applyButtonWidth,
                          applyButtonHeight, 3, 46,
                          204, 113, 255);

    return element;
}

void renderSettingsPanel(SDL_Renderer **renderer, panel settingsPanel) {

    int propAlpha = (double)settingsPanel.color.a / 255 * 100;
    int propAnim[4] = {0};
    int j = 4;
    for (int i = 0; i < 4; ++i)
    {
        j--;
        if (propAlpha >= 25 * i && propAlpha <= 100 - 25 * j)
        {
            propAnim[i] = (double)(propAlpha - (i * 25)) / 25 * 100;
        }
        else if (propAlpha >= 25 * i)
        {
            propAnim[i] = 100;
        }
    }

    int barHeight = 0;
    int barWidth = 0;
    int x1 = 0;

    for (int i = 0; i < 4; ++i)
    {
        barHeight = settingsPanel.categoryClip[i].y + sf(50);
        barWidth =  prop(settingsPanel.categoryClip[i].w - sf(50), propAnim[i]);
        x1 = (settingsPanel.categoryClip[i].x + (settingsPanel.categoryClip[i].w / 2)) -
             (barWidth / 2);

        drawRoundedBoxShadow(&*renderer, settingsPanel.categoryClip[i].x, settingsPanel.categoryClip[i].y,
                             settingsPanel.categoryClip[i].x +  settingsPanel.categoryClip[i].w ,
                             settingsPanel.categoryClip[i].y + settingsPanel.categoryClip[i].h, sf(5),
                             12, prop(255, propAnim[i]));

        roundedBoxColor(*renderer, settingsPanel.categoryClip[i].x, settingsPanel.categoryClip[i].y,
                        settingsPanel.categoryClip[i].x +  settingsPanel.categoryClip[i].w ,
                        settingsPanel.categoryClip[i].y + settingsPanel.categoryClip[i].h, sf(5),
                        SDL_FOURCC(settingsPanel.color.r, settingsPanel.color.g, settingsPanel.color.b, prop(255, propAnim[i])));

        SDL_SetTextureAlphaMod(settingsPanel.categoryLabel[i].texture, prop(255, propAnim[i]));

        SDL_RenderCopy(*renderer, settingsPanel.categoryLabel[i].texture, NULL, &settingsPanel.categoryLabel[i].clip);

        SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

        for (int i = 0; i < 2; ++i)
        {
            SDL_RenderDrawLine(*renderer, x1 , barHeight + i, x1 + barWidth ,
                               barHeight + i);
        }
    }

    //render category 1 elements  :

    settingsPanel.resBar.color.a = prop(255, propAnim[0]);
    renderSelectBar(&*renderer, settingsPanel.resBar);

    settingsPanel.displayBar.color.a = prop(255, propAnim[0]);
    renderSelectBar(&*renderer, settingsPanel.displayBar);

    //render category 2 elements :

    settingsPanel.rotateKeySelector.color.a = prop(255, propAnim[1]);

    roundedBoxColor(*renderer, settingsPanel.rotateKeySelector.clip.x,
                    settingsPanel.rotateKeySelector.clip.y,
                    settingsPanel.rotateKeySelector.clip.x +
                    settingsPanel.rotateKeySelector.clip.w,
                    settingsPanel.rotateKeySelector.clip.y +
                    settingsPanel.rotateKeySelector.clip.h, sf(2),
                    SDL_FOURCC(settingsPanel.rotateKeySelector.color.r,
                               settingsPanel.rotateKeySelector.color.g,
                               settingsPanel.rotateKeySelector.color.b,
                               settingsPanel.rotateKeySelector.color.a));

    SDL_SetTextureAlphaMod(settingsPanel.rotateKeySelector.key.texture, settingsPanel.rotateKeySelector.color.a);
    SDL_RenderCopy(*renderer, settingsPanel.rotateKeySelector.key.texture, NULL, &settingsPanel.rotateKeySelector.key.clip);

    settingsPanel.langBar.color.a = prop(255, propAnim[1]);
    renderSelectBar(&*renderer, settingsPanel.langBar);

    //render category 3 elements :

    settingsPanel.musicBar.color.a = prop(255, propAnim[2]);
    scrollBarRender(&*renderer, settingsPanel.musicBar);
    settingsPanel.effectsBar.color.a = prop(255, propAnim[2]);
    scrollBarRender(&*renderer, settingsPanel.effectsBar);


    barHeight = settingsPanel.clip.y + settingsPanel.clip.h - sf(210);
    barWidth =  prop(settingsPanel.clip.w, propAlpha);
    x1 = (settingsPanel.clip.x + (settingsPanel.clip.w / 2)) -
         (barWidth / 2);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    for (int i = 0; i < 2; ++i)
    {
        SDL_RenderDrawLine(*renderer, x1 , barHeight + i, x1 + barWidth ,
                           barHeight + i);
    }

    mainMenuButtonRender(&*renderer, settingsPanel.applyButton, 10);

}

void mainMenuDisplayRefresh(SDL_Renderer **renderer, button newGameButton, SDL_Texture **background,
                            int alpha, mainBar menuBar, form1 newGameForm,
                            logoRect logo, scoreBoard board, panel settingsPanel) {

    SDL_RenderCopy(*renderer, *background, NULL, NULL);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 192);

    SDL_RenderFillRect(*renderer, NULL);

    switch (menuBar.actualTab) {

    case 0:

        if (newGameForm.openAnimation || newGameForm.openned)
        {
            newGameForm.color.a = menuBar.elementsAlpha;
            renderNewGameForm(&*renderer, newGameForm);
        }
        else {

            newGameButton.color.a = menuBar.elementsAlpha;
            mainMenuButtonRender(&*renderer, newGameButton, 12);
        }

        logo.color.a = menuBar.elementsAlpha;
        renderLogoRect(&*renderer, logo);


        break;

    case 1:

        board.color.a = menuBar.elementsAlpha;
        renderScoreBoard(&*renderer, board);

        break;

    case 2:

        settingsPanel.color.a = menuBar.elementsAlpha;

        renderSettingsPanel(&*renderer, settingsPanel);

        break;

    case 3:

        break;

    case 4:

        break;

    }

    renderMenuBar(&*renderer, menuBar);

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);

}

form1 newGameFormInit(SDL_Renderer **renderer, char *name, SDL_Rect startClip,
                      int x, int y, int h, int w, SDL_Color color) {

    SDL_Color elementsColor = {93, 109, 126, 0};

    form1 element;

    element.clip = startClip;

    element.startClip = startClip;

    element.color = color;

    element.finalClip.x = x;
    element.finalClip.y = y;
    element.finalClip.h = h;
    element.finalClip.w = w;

    element.name = createTextTexture(&*renderer, name, FONT_STANDARD, sf(20),
                                     element.finalClip.x +
                                     (element.finalClip.w / 2),
                                     element.finalClip.y + sf(30),
                                     HCENTER | VCENTER);

    int playButtonHeight = sf(30);
    int playButtonWidth = sf(100);

    int playButtonOffset = sf(20);

    element.playButton = mainMenuCreateButtonRGBA(&*renderer, sf(20),
                         gs("Play"), element.finalClip.x + element.finalClip.w
                         - playButtonWidth - playButtonOffset,
                         element.finalClip.y + element.finalClip.h -
                         playButtonHeight - playButtonOffset, playButtonWidth,
                         playButtonHeight, 3, 46,
                         204, 113, 0);

    selectBarItem difficultyBarItems[5];

    difficultyBarItems[0].texture = loadTexture(&*renderer,
                                    "../images/selectBar/displayBar/res-logo3.jpg");
    difficultyBarItems[1].texture = loadTexture(&*renderer,
                                    "../images/selectBar/displayBar/res-logo2.jpg");
    difficultyBarItems[2].texture = loadTexture(&*renderer,
                                    "../images/selectBar/displayBar/res-logo.jpg");
    difficultyBarItems[3].texture = loadTexture(&*renderer,
                                    "../images/selectBar/displayBar/res-logo3.jpg");
    difficultyBarItems[4].texture = loadTexture(&*renderer,
                                    "../images/selectBar/displayBar/res-logo2.jpg");

    difficultyBarItems[0].text = createTextTexture(&*renderer, gs("Customized"),
                                 FONT_STANDARD, sf(16), sf(675), sf(340),
                                 LEFT | BOTTOM);
    difficultyBarItems[1].text = createTextTexture(&*renderer, gs("Easy"),
                                 FONT_STANDARD, sf(16), sf(675), sf(340),
                                 LEFT | BOTTOM);
    difficultyBarItems[2].text = createTextTexture(&*renderer, gs("Medium"),
                                 FONT_STANDARD, sf(16), sf(675), sf(340),
                                 LEFT | BOTTOM);

    difficultyBarItems[3].text = createTextTexture(&*renderer, gs("Difficult"),
                                 FONT_STANDARD, sf(16), sf(675), sf(340),
                                 LEFT | BOTTOM);
    difficultyBarItems[4].text = createTextTexture(&*renderer, gs("Competitive"),
                                 FONT_STANDARD, sf(16), sf(675), sf(340),
                                 LEFT | BOTTOM);

    element.difficultyBar = createSelectBarRGBA(&*renderer, element.finalClip.x
                            + sf(20), element.finalClip.y + sf(80), sf(20),
                            sf(200), 5, difficultyBarItems, elementsColor.r, elementsColor.g, elementsColor.b, elementsColor.a);

    setSelectBarValue(&element.difficultyBar, 0);

    element.smartBar = createScrollBarRGBA(&*renderer, gs("AI intelligence level"),
                                           50, 1, sf(200), sf(10),
                                           element.finalClip.x + sf(20),
                                           element.finalClip.y + sf(170), sf(8),
                                           elementsColor.r, elementsColor.g, elementsColor.b, elementsColor.a);

    element.strokeBar = createGraduateBarRGBA(&*renderer, 1,
                        9, gs("Max number of strokes per round"), element.finalClip.x + sf(20),
                        element.finalClip.y + sf(240), sf(30),
                        sf(100), elementsColor.r, elementsColor.g, elementsColor.b, elementsColor.a);

    element.openned = 0;

    element.animProp = 0;
    element.alphaProp = 0;

    element.prev = 0;

    element.openAnimation = 0;

    return element;
}

void scrollBarAnimation(scrollBar * bar, int actual, int *isChange) {

    if (bar->fadeIn == 1)
    {
        actual = SDL_GetTicks();

        if (bar->circleRadiusRender <= bar->circleRadius)
        {
            if (actual > bar->prev + 10)
            {
                bar->prev = SDL_GetTicks();

                bar->circleRadiusRender = bar->circleRadiusRender + 1;

                if (bar->circleRadiusRender >= bar->circleRadius)
                {
                    bar->circleRadiusRender = bar->circleRadius;
                    bar->fadeIn = 0;
                }

                *isChange = 1;
            }
        }
    }
    else if (bar->fadeOut == 1)
    {
        actual = SDL_GetTicks();

        if (bar->circleRadiusRender > 0)
        {
            if (actual > bar->prev + 10)
            {
                bar->prev = SDL_GetTicks();

                bar->circleRadiusRender = bar->circleRadiusRender - 1;

                if (bar->circleRadiusRender <= 0)
                {
                    bar->circleRadiusRender = 0;
                    bar->fadeOut = 0;

                }

                *isChange = 1;
            }
        }
    }
}

void newGameFormAnimation(form1 * newGameForm, int *isChange, int actual) {

    selectBarAnimation(&newGameForm->difficultyBar, &*isChange, actual);

    scrollBarAnimation(&newGameForm->smartBar, actual, &*isChange);

    if (newGameForm->openAnimation == 1)
    {
        if (newGameForm->animProp < 100)
        {
            if (actual > newGameForm->prev + 10)
            {
                newGameForm->prev = SDL_GetTicks();

                newGameForm->animProp =
                    newGameForm->animProp + 8;

                if (newGameForm->animProp >= 100)
                {
                    newGameForm->animProp = 100;
                }

                *isChange = 1;
            }
        }
        else if (newGameForm->alphaProp < 100)
        {
            if (actual > newGameForm->prev + 10)
            {
                newGameForm->prev = SDL_GetTicks();

                newGameForm->alphaProp =
                    newGameForm->alphaProp + 8;

                if (newGameForm->alphaProp >= 100)
                {
                    newGameForm->alphaProp = 100;
                    newGameForm->openAnimation = 0;
                    newGameForm->openned = 1;
                }

                *isChange = 1;
            }
        }
    }
}

void graduateBarGetState(SDL_Renderer **renderer, graduateBar * bar, SDL_Event event, SDL_Point mousePosition,
                         int *isChange, Mix_Chunk **select, Mix_Chunk **click) {

    int ret = -1;

    char temp[4];

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:

        buttonOnClick(bar->prevButton.clip, mousePosition, &ret,
                      bar->prevButton.id, &*click);

        buttonOnClick(bar->nextButton.clip, mousePosition, &ret,
                      bar->nextButton.id, &*click);

        switch (ret) {

        case 0:

            if (bar->value < bar->maxValue)
            {
                bar->value++;

                sprintf(temp, "%d", bar->value);

                updateTextTexture(&*renderer, &bar->textValue,
                                  temp, FONT_STANDARD, sf(15));

                bar->changedValue = 1;

            }

            ret = -1;

            break;

        case 1:

            if (bar->value > bar->minValue)
            {
                bar->value--;

                sprintf(temp, "%d", bar->value);

                updateTextTexture(&*renderer, &bar->textValue,
                                  temp, FONT_STANDARD, sf(15));

                bar->changedValue = 1;
            }

            ret = -1;

            break;
        }

        break;

    case SDL_MOUSEMOTION:

        buttonGetState(&*renderer, &bar->prevButton, &*select, &*isChange,
                       mousePosition);

        buttonGetState(&*renderer, &bar->nextButton, &*select, &*isChange,
                       mousePosition);

        break;
    }
}

void setSelectBarValue(selectBar * bar, int selected) {

    selectBarItem temp;

    for (int i = 0; i < bar->nbItems; ++i)
    {
        if (bar->item[i].itemId == selected)
        {
            temp = bar->item[i];
            bar->item[i] = bar->item[0];
            bar->item[0] = temp;
        }
    }

    updateSelectbarItems(bar);
}

void setScrollBarValue(SDL_Renderer ** renderer, scrollBar * bar, int value) {

    bar->circleCenterX = ((bar->clip.w / 100) * value) + bar->clip.x;

    bar->value = value;

    updateScrollBarText(&*renderer, bar);
}

void setGraduateBarValue(SDL_Renderer **renderer, graduateBar * bar, int value) {

    char temp[4];

    bar->value = value;

    sprintf(temp, "%d", bar->value);

    updateTextTexture(&*renderer, &bar->textValue,
                      temp, FONT_STANDARD, sf(15));
}

void freeSelectBar(selectBar *bar) {

    for (int i = 0; i < bar->nbItems; ++i)
    {
        SDL_DestroyTexture(bar->item[i].text.texture);
        SDL_DestroyTexture(bar->item[i].texture);
    }

    SDL_DestroyTexture(bar->arrowTexture);
}

void freeScrollBar(scrollBar *bar) {

    SDL_DestroyTexture(bar->circleTexture);
    SDL_DestroyTexture(bar->topText.texture);
    SDL_DestroyTexture(bar->bottomText.texture);
}

void freeTAB0(form1 * newGameForm, button * newGameButton, logoRect * logo) {

    for (int i = 0; i < 4; ++i)
    {
        SDL_DestroyTexture(logo->label[i].texture);
    }

    SDL_DestroyTexture(newGameButton->label.texture);

    SDL_DestroyTexture(newGameForm->name.texture);

    SDL_DestroyTexture(newGameForm->playButton.label.texture);

    freeSelectBar(&newGameForm->difficultyBar);

    freeScrollBar(&newGameForm->smartBar);

    SDL_DestroyTexture(newGameForm->strokeBar.name.texture);
    SDL_DestroyTexture(newGameForm->strokeBar.textValue.texture);
    SDL_DestroyTexture(newGameForm->strokeBar.prevButton.label.texture);
    SDL_DestroyTexture(newGameForm->strokeBar.nextButton.label.texture);
}

void freeTAB1(scoreBoard * board) {

    for (int i = 0; i < board->nbCols; ++i)
    {
        for (int j = 0; j < board->nbLines; ++j)
        {
            SDL_DestroyTexture(board->item[j].label[i].texture);
        }

        SDL_DestroyTexture(board->cols[i].label.texture);
    }
}

void freeTAB2(panel *settingsPanel) {

    freeSelectBar(&settingsPanel->displayBar);
    freeSelectBar(&settingsPanel->resBar);
    freeSelectBar(&settingsPanel->langBar);

    freeScrollBar(&settingsPanel->musicBar);
    freeScrollBar(&settingsPanel->effectsBar);

    for (int i = 0; i < 4; ++i)
    {
        SDL_DestroyTexture(settingsPanel->categoryLabel[i].texture);
    }

    SDL_DestroyTexture(settingsPanel->applyButton.label.texture);

    SDL_DestroyTexture(settingsPanel->rotateKeySelector.key.texture);
}

logoRect logoRectInit(SDL_Renderer **renderer, int x, int y, int h, int w,
                      SDL_Color color) {

    logoRect element;

    element.clip.x = x;
    element.clip.y = y;
    element.clip.h = h;
    element.clip.w = w;

    element.color = color;

    char temp[30] = {0};

    element.label[0] = createTextTexture(&*renderer, gs("The"), FONT_LOGO,
                                         sf(70), element.clip.x +
                                         element.clip.w, element.clip.y +
                                         sf(60), VCENTER | RIGHT);

    element.label[1] = createTextTexture(&*renderer, gs("BattleShip"), FONT_LOGO,
                                         sf(70), element.clip.x +
                                         element.clip.w, element.clip.y +
                                         sf(125), VCENTER | RIGHT);

    element.label[2] = createTextTexture(&*renderer, "V 0.9 Alpha", FONT_LOGO,
                                         sf(20), element.clip.x +
                                         element.clip.w, element.clip.y +
                                         sf(170), VCENTER | RIGHT);

    sprintf(temp, "%s Loic Vanden Bossche", gs("By"));

    element.label[3] = createTextTexture(&*renderer, temp,
                                         FONT_LOGO, sf(15), element.clip.x +
                                         element.clip.w, element.clip.y +
                                         sf(200), VCENTER | RIGHT);

    return element;
}

void scoreBoardAnimation(scoreBoard * board, int *isChange, int actual) {

    int prev = 0;

    int speed = 48;

    if (board->animOut)
    {
        if (actual > prev + 10)
        {
            prev = SDL_GetTicks();

            for (int i = 0; i < board->nbCols; ++i)
            {
                for (int j = 0; j < board->nbLines; ++j)
                {
                    if (board->cols[i].lines[j].alpha > 0)
                    {
                        board->cols[i].lines[j].alpha =
                            board->cols[i].lines[j].alpha - 64;

                        if (board->cols[i].lines[j].alpha <= 0)
                        {
                            board->cols[i].lines[j].alpha = 0;
                            board->animOut = 0;
                        }

                        *isChange = 1;
                    }
                }
            }
        }
    }
    else if (board->animIn)
    {
        if (actual > prev + 10)
        {
            prev = SDL_GetTicks();

            if (board->cols[0].lines[board->animActualCol].alpha == 255 && board->animActualCol < board->nbLines)
            {
                board->animActualCol++;
            }

            for (int i = 0; i < board->nbCols; ++i)
            {
                if (board->cols[i].lines[board->animActualCol].alpha < 255)
                {
                    board->cols[i].lines[board->animActualCol].alpha =
                        board->cols[i].lines[board->animActualCol].alpha + speed;

                    if (board->cols[i].lines[board->animActualCol].alpha >= 255)
                    {
                        board->cols[i].lines[board->animActualCol].alpha = 255;
                    }

                    *isChange = 1;
                }
            }

            if (board->cols[0].lines[board->nbLines - 1].alpha == 255)
            {
                board->animIn = 0;
                board->animActualCol = 0;
            }
        }
    }
}

void scoreBoardGetState(scoreBoard * board,
                        SDL_Point mousePosition, int *isChange) {

    for (int i = 0; i < board->nbCols; ++i)
    {
        for (int j = 0; j < board->nbLines; ++j)
        {
            if (ifPointInSquare(board->cols[i].lines[j].clip, mousePosition))
            {
                if (board->cols[i].lines[j].hovered == 0)
                {
                    board->cols[i].lines[j].hovered = 1;

                    *isChange = 1;
                }
            }
            else if (board->cols[i].lines[j].hovered == 1)
            {
                board->cols[i].lines[j].hovered = 0;

                *isChange = 1;
            }
        }
    }
}

void freeMenuBar(mainBar * bar) {

    for (int i = 0; i < 5; ++i)
    {
        SDL_DestroyTexture(bar->tab[i].name.texture);
    }

    SDL_DestroyTexture(bar->closeButton.label.texture);
}

void setDisplayMode(int displayMode) {

    switch (displayMode) {

    case 0:
        SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN);
        break;
    case 1:
        SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        break;

    case 2:
        SDL_SetWindowFullscreen( window, 0);
        break;
    }
}

int mainMenuDisplay(SDL_Renderer **renderer, SDL_Cursor **cursor,
                    int *mainContinue, int createNewUserAccount) {

    int LocalContinue = 1;
    int isChange = 0;
    int ret = 0;
    SDL_Point mousePosition;
    SDL_Event event;
    int alpha = 255;
    int canQuit = 0;
    int localStart = 1;
    int choice = 0;
    int prev = 0;
    int passed = 0;
    int temp = 0;;
    int isCompetitive = 0;

    setMusicVolume(getUserSettings().musicVolume);

    setChannelVolume(getUserSettings().effectsVolume, 1);

    SDL_Texture *background = loadTexture(&*renderer,
                                          "../images/menu_background.jpg");

    Mix_Chunk *select = Mix_LoadWAV("../sound/effects/select.wav");

    Mix_Chunk *click = Mix_LoadWAV("../sound/effects/clic.wav");

    mainBar menuBar = initMenuBar(&*renderer);

    //TAB-0 HOME --------------------------------------------------------- //

    form1 newGameForm;
    button newGameButton;
    logoRect logo;

    //TAB-1 SCORE -------------------------------------------------------- //

    scoreBoard board;

    int sortRet = -1;

    //TAB-2 SETTINGS ----------------------------------------------------- //

    panel settingsPanel;

    userSettings tempSettings = getUserSettings();

    //TAB-3 STATISTICS --------------------------------------------------- //

    //TAB-4 CREDITS ----------------------------------------------------   //

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    while (!canQuit)
    {
        isChange = 0;

        SDL_Delay(2);

        while (SDL_PollEvent(&event) || menuBar.firstLoad == 1) {

            passed = 0;
            ret = 0;

            menuBarGetSate(event, &menuBar, mousePosition, &isChange);

            if (menuBar.tabChanged != -1 || menuBar.firstLoad == 1)
            {
                menuBar.openAnimation = 1;
                menuBar.elementsAlpha = 0;

                switch (menuBar.tabChanged) {

                //TAB-0 HOME --------------------------------------------- //
                case 0:
                    freeTAB0(&newGameForm, &newGameButton, &logo);
                    break;

                //TAB-1 SCORE -------------------------------------------- //
                case 1:
                    freeTAB1(&board);
                    break;

                //TAB-2 SETTINGS ----------------------------------------- //
                case 2:
                    freeTAB2(&settingsPanel);
                    break;

                //TAB-3 STATISTICS --------------------------------------- //

                case 3:
                    break;

                //TAB-4 CREDITS -----------------------------------------  //

                case 4:
                    break;
                }

                switch (menuBar.actualTab) {

                //TAB-0 HOME --------------------------------------------- //
                case 0:

                    newGameButton = mainMenuCreateButtonRGBA(&*renderer, sf(20),
                                    gs("New game"), sf(50), sf(200), sf(200),
                                    sf(100), 1, 40, 55, 71, 0);

                    newGameForm = newGameFormInit(&*renderer, gs("Create new game"),
                                                  newGameButton.clip,
                                                  newGameButton.clip.x,
                                                  sf(190), sf(400), sf(300),
                                                  newGameButton.color);

                    logo = logoRectInit(&*renderer, sf(780),
                                        newGameForm.finalClip.y,
                                        newGameForm.finalClip.h / 2,
                                        sf(400), newGameButton.color);
                    break;

                //TAB-1 SCORE -------------------------------------------- //
                case 1:

                    board = scoreBoardInit(&*renderer, menuBar.tab[1].color);

                    updateScoreBoardItems(&*renderer, &board);

                    break;

                //TAB-2 SETTINGS ----------------------------------------- //

                case 2:

                    settingsPanel = settingsPanelInit(&*renderer, menuBar.tab[2].color);

                    break;

                //TAB-3 STATISTICS --------------------------------------- //

                case 3:
                    break;

                //TAB-4 CREDITS -----------------------------------------  //

                case 4:
                    break;
                }

                menuBar.tabChanged = -1;

                if (menuBar.firstLoad == 1)
                {
                    menuBar.firstLoad = 0;
                }
            }

            switch (event.type)
            {
            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                buttonGetState(&*renderer, &menuBar.closeButton, &select, &isChange,
                               mousePosition);

                switch (menuBar.actualTab) {

                //TAB-0 HOME --------------------------------------------- //
                case 0:
                    if (newGameForm.openAnimation || newGameForm.openned)
                    {
                        buttonGetState(&*renderer, &newGameForm.playButton,
                                       &select, &isChange, mousePosition);
                    }
                    else {

                        buttonGetState(&*renderer, &newGameButton, &select,
                                       &isChange, mousePosition);
                    }
                    break;

                //TAB-1 SCORE -------------------------------------------- //
                case 1:

                    for (int i = 0; i < board.nbCols; ++i)
                    {
                        buttonGetState(&*renderer, &board.cols[i].sortButton, &select,
                                       &isChange, mousePosition);
                    }

                    scoreBoardGetState(&board, mousePosition, &isChange);

                    break;

                //TAB-2 SETTINGS ----------------------------------------- //
                case 2:

                    if (ifPointInSquare(settingsPanel.rotateKeySelector.clip, mousePosition))
                    {
                        if (!settingsPanel.rotateKeySelector.isHovered)
                        {
                            settingsPanel.rotateKeySelector.isHovered = 1;
                        }
                    }
                    else if (settingsPanel.rotateKeySelector.isHovered) {

                        settingsPanel.rotateKeySelector.isHovered = 0;
                    }

                    if (settingsPanel.settingsChanged)
                    {

                        buttonGetState(&*renderer, &settingsPanel.applyButton, &select,
                                       &isChange, mousePosition);
                    }

                    break;

                //TAB-3 STATISTICS --------------------------------------- //
                case 3:
                    break;


                //TAB-4 CREDITS -----------------------------------------  //
                case 4:
                    break;

                }

                break;

            case SDL_MOUSEBUTTONDOWN:

                buttonOnClick(menuBar.closeButton.clip, mousePosition, &ret,
                              menuBar.closeButton.id, &click);

                switch (menuBar.actualTab) {

                //TAB-0 HOME --------------------------------------------- //
                case 0:

                    if (newGameForm.openAnimation || newGameForm.openned)
                    {
                        buttonOnClick(newGameForm.playButton.clip,
                                      mousePosition, &ret,
                                      newGameForm.playButton.id, &click);
                    }
                    else {

                        buttonOnClick(newGameButton.clip, mousePosition, &ret,
                                      newGameButton.id, &click);
                    }

                    break;


                //TAB-1 SCORE -------------------------------------------- //
                case 1:

                    if (!board.animIn && !board.animOut)
                    {
                        for (int i = 0; i < board.nbCols; ++i)
                        {
                            buttonOnClick(board.cols[i].sortButton.clip, mousePosition, &sortRet,
                                          board.cols[i].sortButton.id, &click);
                        }

                        if (sortRet != -1)
                        {
                            if (sortRet == board.sortID)
                            {
                                if (board.isReverse) {board.isReverse = 0;}
                                else {board.isReverse = 1;}
                            }
                            else {

                                board.sortID = sortRet;
                                board.isReverse = 0;
                            }

                            updateScoreBoardItems(&*renderer, &board);

                            isChange = 1;

                            sortRet = -1;

                            board.animOut = 1;
                            board.animIn = 1;
                        }
                    }

                    break;

                //TAB-2 SETTINGS ----------------------------------------- //
                case 2:

                    if (settingsPanel.rotateKeySelector.isFocus)
                    {
                        settingsPanel.rotateKeySelector.isFocus = 0;

                        updateTextTexture(&*renderer, &settingsPanel.rotateKeySelector.key,
                                          SDL_GetKeyName(tempSettings.rotateKey), FONT_STANDARD, sf(15));

                        isChange = 1;
                    }
                    else {

                        if (settingsPanel.rotateKeySelector.isHovered)
                        {
                            settingsPanel.rotateKeySelector.isFocus = 1;

                            updateTextTexture(&*renderer, &settingsPanel.rotateKeySelector.key,
                                              "____", FONT_STANDARD, sf(15));

                            isChange = 1;
                        }
                    }

                    if (settingsPanel.settingsChanged)
                    {
                        settingsPanel.applyRet = 0;

                        buttonOnClick(settingsPanel.applyButton.clip, mousePosition,
                                      &settingsPanel.applyRet,
                                      settingsPanel.applyButton.id, &click);

                        if (settingsPanel.applyRet)
                        {
                            settingsPanel.needReload = 0;

                            if (tempSettings.displayMode != getUserSettings().displayMode)
                            {
                                setDisplayMode(tempSettings.displayMode);
                            }

                            if (tempSettings.displayRes != getUserSettings().displayRes)
                            {
                                setSizeFactor(getScreenWidth_DisplayRes(tempSettings.displayRes));

                                SDL_SetWindowSize(window, sf(1280), sf(720));

                                setDisplayMode(tempSettings.displayMode);

                                settingsPanel.needReload = 1;
                            }

                            if (tempSettings.selectedLanguage != getUserSettings().selectedLanguage)
                            {
                                setCurrentLang(tempSettings.selectedLanguage);
                                SDL_SetWindowTitle(window, gs("Battleship"));
                                settingsPanel.needReload = 1;
                            }

                            if (tempSettings.musicVolume != getUserSettings().musicVolume)
                            {
                                setMusicVolume(tempSettings.musicVolume);
                            }

                            if (tempSettings.effectsVolume != getUserSettings().effectsVolume)
                            {
                                setChannelVolume(tempSettings.effectsVolume, 1);
                            }

                            setUserSettings(tempSettings);
                            settingsPanel.settingsChanged = 0;
                            settingsPanel.applyRet = 0;
                            settingsPanel.applyButton.isSelected = 0;
                            isChange = 1;

                            if (settingsPanel.needReload)
                            {
                                freeMenuBar(&menuBar);
                                updateMenuBar(&menuBar, &*renderer);
                                menuBar.tabChanged = 2;
                            }
                        }
                    }

                    break;


                //TAB-3 STATISTICS --------------------------------------- //
                case 3:

                    break;


                //TAB-4 CREDITS -----------------------------------------  //
                case 4:

                    break;

                }

                switch (ret) {

                case 1:
                    newGameForm.openAnimation = 1;
                    break;

                case 2:
                    LocalContinue = 0;
                    *mainContinue = 0;
                    break;

                case 3:
                    LocalContinue = 0;
                    choice = 1;
                    break;
                }
                break;

            case SDL_QUIT:
                LocalContinue = 0;
                *mainContinue = 0;
                break;
            }

            switch (menuBar.actualTab) {

            //TAB-0 HOME ------------------------------------------------- //
            case 0:

                if (newGameForm.openned)
                {
                    selectBarGetState(&newGameForm.difficultyBar, event,
                                      mousePosition, &isChange);

                    graduateBarGetState(&*renderer, &newGameForm.strokeBar,
                                        event, mousePosition, &isChange,
                                        &select, &click);

                    if (!newGameForm.difficultyBar.deploy)
                    {
                        scrollBarGetState(&*renderer, &newGameForm.smartBar, event,
                                          mousePosition, &isChange, &*cursor);
                    }

                    if (newGameForm.difficultyBar.item[0].itemId != 0)
                    {
                        if (newGameForm.smartBar.changedValue ||
                                newGameForm.strokeBar.changedValue)
                        {
                            setSelectBarValue(&newGameForm.difficultyBar, 0);
                        }
                    }

                    newGameForm.smartBar.changedValue = 0;
                    newGameForm.strokeBar.changedValue = 0;

                    if (newGameForm.difficultyBar.changedValue == 1)
                    {
                        switch (newGameForm.difficultyBar.item[0].itemId) {

                        case 1:

                            setScrollBarValue(&*renderer, &newGameForm.smartBar, 25);

                            setGraduateBarValue(&*renderer,
                                                &newGameForm.strokeBar, 3);

                            break;

                        case 2:

                            setScrollBarValue(&*renderer, &newGameForm.smartBar, 50);

                            setGraduateBarValue(&*renderer,
                                                &newGameForm.strokeBar, 2);

                            break;

                        case 3:

                            setScrollBarValue(&*renderer, &newGameForm.smartBar, 90);

                            setGraduateBarValue(&*renderer,
                                                &newGameForm.strokeBar, 1);

                            break;

                        case 4:

                            setScrollBarValue(&*renderer, &newGameForm.smartBar, 100);

                            setGraduateBarValue(&*renderer,
                                                &newGameForm.strokeBar, 2);

                            break;
                        }

                        newGameForm.difficultyBar.changedValue = 0;
                    }
                }

                break;


            //TAB-1 SCORE ------------------------------------------------ //

            case 1:

                break;


            //TAB-2 SETTINGS --------------------------------------------- //

            case 2:

                //Process displayBar state

                selectBarGetState(&settingsPanel.displayBar, event,
                                  mousePosition, &isChange);

                if (settingsPanel.displayBar.changedValue == 1)
                {
                    tempSettings.displayMode = settingsPanel.displayBar.item[0].itemId;
                    settingsPanel.displayBar.changedValue = 0;

                    if (settingsPanel.displayBar.item[0].itemId == 1)
                    {
                        for (int i = 0; i < settingsPanel.resBar.nbItems; ++i)
                        {
                            if (getScreenWidth() >= getScreenWidth_DisplayRes(i))
                            {
                                temp = i;
                            }
                        }
                        setSelectBarValue(&settingsPanel.resBar, temp);
                        settingsPanel.resBar.changedValue = 1;
                    }
                }
                if (tempSettings.displayMode != getUserSettings().displayMode)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else if (settingsPanel.settingsChanged)
                {
                    passed ++;
                }

                //Process resBar state

                if (!settingsPanel.displayBar.deploy && settingsPanel.displayBar.item[0].itemId != 1)
                {
                    selectBarGetState(&settingsPanel.resBar, event,
                                      mousePosition, &isChange);
                }

                if (settingsPanel.resBar.changedValue == 1)
                {
                    tempSettings.displayRes = settingsPanel.resBar.item[0].itemId;

                    settingsPanel.resBar.changedValue = 0;
                }
                if (tempSettings.displayRes != getUserSettings().displayRes)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else if (settingsPanel.settingsChanged)
                {
                    passed++;
                }


                //Process keySelector state :

                if (settingsPanel.rotateKeySelector.isFocus)
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        tempSettings.rotateKey = event.key.keysym.sym;

                        updateTextTexture(&*renderer, &settingsPanel.rotateKeySelector.key,
                                          SDL_GetKeyName(event.key.keysym.sym), FONT_STANDARD, sf(15));

                        isChange = 1;

                        settingsPanel.rotateKeySelector.isFocus = 0;
                    }
                }

                if (tempSettings.rotateKey != getUserSettings().rotateKey)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else {

                    passed++;
                }


                //Process langBar state

                selectBarGetState(&settingsPanel.langBar, event,
                                  mousePosition, &isChange);

                if (settingsPanel.langBar.changedValue == 1)
                {
                    tempSettings.selectedLanguage = settingsPanel.langBar.item[0].itemId;

                    settingsPanel.langBar.changedValue = 0;
                }
                if (tempSettings.selectedLanguage != getUserSettings().selectedLanguage)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else if (settingsPanel.settingsChanged)
                {
                    passed++;
                }


                //process musicBar state :

                scrollBarGetState(&*renderer, &settingsPanel.musicBar, event,
                                  mousePosition, &isChange, &*cursor);

                if (settingsPanel.musicBar.changedValue == 1)
                {
                    tempSettings.musicVolume = settingsPanel.musicBar.value;

                    settingsPanel.musicBar.changedValue = 0;
                }
                if (tempSettings.musicVolume != getUserSettings().musicVolume)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else if (settingsPanel.settingsChanged)
                {
                    passed++;
                }

                //process effectsBar state :

                scrollBarGetState(&*renderer, &settingsPanel.effectsBar, event,
                                  mousePosition, &isChange, &*cursor);

                if (settingsPanel.effectsBar.changedValue == 1)
                {
                    tempSettings.effectsVolume = settingsPanel.effectsBar.value;

                    settingsPanel.effectsBar.changedValue = 0;
                }
                if (tempSettings.effectsVolume != getUserSettings().effectsVolume)
                {
                    settingsPanel.settingsChanged = 1;
                }
                else if (settingsPanel.settingsChanged)
                {
                    passed++;
                }

                //Check if setting changed

                if (passed == 6 && settingsPanel.settingsChanged == 1)
                {
                    settingsPanel.settingsChanged = 0;
                }

                break;

            //TAB-3 STATISTICS ------------------------------------------- //

            case 3:

                break;


            //TAB-4 CREDITS ---------------------------------------------  //

            case 4:

                break;
            }
        }

        switch (menuBar.actualTab) {

        //TAB-0 HOME ------------------------------------------------- //

        case 0:

            newGameFormAnimation(&newGameForm, &isChange, SDL_GetTicks());

            break;

        //TAB-1 SCORE ------------------------------------------------ //

        case 1:

            scoreBoardAnimation(&board, &isChange, SDL_GetTicks());

            break;


        //TAB-2 SETTINGS --------------------------------------------- //

        case 2:

            selectBarAnimation(&settingsPanel.displayBar, &isChange, SDL_GetTicks());
            selectBarAnimation(&settingsPanel.resBar, &isChange, SDL_GetTicks());
            selectBarAnimation(&settingsPanel.langBar, &isChange, SDL_GetTicks());

            scrollBarAnimation(&settingsPanel.musicBar, SDL_GetTicks(), &isChange);
            scrollBarAnimation(&settingsPanel.effectsBar, SDL_GetTicks(), &isChange);

            break;


        //TAB-3 STATISTICS ------------------------------------------- //

        case 3:

            break;


        //TAB-4 CREDITS ---------------------------------------------  //

        case 4:

            break;


        }

        menuBarAnimation(&menuBar, &isChange, SDL_GetTicks(), alpha);

        if (LocalContinue == 0) {

            if (alpha < 255 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            if (alpha > 0)
            {
                if (SDL_GetTicks() > prev + 20)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= 0)
                    {
                        alpha = 0;
                        localStart = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (isChange == 1)
        {
            mainMenuDisplayRefresh( &*renderer, newGameButton,
                                    &background, alpha, menuBar, newGameForm,
                                    logo, board, settingsPanel);
        }
    }

    switch (menuBar.actualTab) {

    //TAB-0 HOME --------------------------------------------------------- //

    case 0:
        freeTAB0(&newGameForm, &newGameButton, &logo);
        break;


    //TAB-1 SCORE -------------------------------------------------------- //

    case 1:
        freeTAB1(&board);
        break;


    //TAB-2 SETTINGS ----------------------------------------------------- //

    case 2:
        freeTAB2(&settingsPanel);
        break;


    //TAB-3 STATISTICS --------------------------------------------------  //

    case 3:
        break;


    //TAB-4 CREDITS -----------------------------------------------------  //

    case 4:
        break;

    }

    if (newGameForm.difficultyBar.item[0].itemId == 4)
    {
        isCompetitive = 1;
    }

    int result = (newGameForm.strokeBar.value << 24) |
                 (isCompetitive << 16) |
                 (newGameForm.smartBar.value << 8) |
                 choice;

    SDL_DestroyTexture(background);
    Mix_FreeChunk(select);

    freeMenuBar(&menuBar);

    return result;
}

void colorPicker_UpdateGradiantTexture(SDL_Renderer **renderer, colorPicker *picker) {

    if (picker->gradiantTexture == NULL)
    {
        SDL_DestroyTexture(picker->gradiantTexture);
    }

    SDL_Surface * surface = SDL_CreateRGBSurface(
                                0, picker->gradiantRect.w, picker->gradiantRect.h, 32,
                                0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            HSVColor hsv_color = {
                picker->currentColor.h,
                (double) x / surface->w,
                1.0 - ((double) y / surface->h),
            };

            SDL_Color color = from_RGBColor(hsv_to_rgb(hsv_color));

            ((char*)surface->pixels)[(x + y * surface->w) * 4    ] = color.r;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 1] = color.g;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 2] = color.b;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 3] = color.a;
        }
    }

    picker->gradiantTexture = SDL_CreateTextureFromSurface(*renderer, surface);
    SDL_FreeSurface(surface);
}

void renderTextInput(SDL_Renderer **renderer, textInput input) {

    roundedBoxColor(*renderer, input.clip.x, input.clip.y,
                    input.clip.x + input.clip.w ,
                    input.clip.y + input.clip.h, sf(5),
                    SDL_FOURCC(input.color.r,
                               input.color.g,
                               input.color.b, input.color.a));

    SDL_SetTextureAlphaMod(input.toPrint.texture, input.color.a);
    SDL_RenderCopy(*renderer, input.toPrint.texture, NULL, &input.toPrint.clip);

    if (input.isFocused)
    {
        SDL_SetRenderDrawColor(*renderer, 255, 255, 255, input.color.a);
        SDL_RenderFillRect(*renderer, &input.cursor);
    }

    if (input.printCharacters)
    {
        SDL_SetTextureAlphaMod(input.maxC.texture, input.color.a);
        SDL_RenderCopy(*renderer, input.maxC.texture, NULL, &input.maxC.clip);
    }
}

void renderNewProfileForm(SDL_Renderer **renderer, form2 newProfileForm) {

    SDL_Rect tempRect = {0};

    int outlineNormal = sf(2);
    int outlineLarge = sf(5);

    drawRoundedBoxShadow(&*renderer, newProfileForm.overview.x, newProfileForm.overview.y,
                         newProfileForm.overview.x + newProfileForm.overview.w ,
                         newProfileForm.overview.y + newProfileForm.overview.h, sf(5), 12,
                         newProfileForm.color.a);

    roundedBoxColor(*renderer, newProfileForm.overview.x, newProfileForm.overview.y,
                    newProfileForm.overview.x + newProfileForm.overview.w ,
                    newProfileForm.overview.y + newProfileForm.overview.h, sf(5),
                    SDL_FOURCC(newProfileForm.category[0].color.r,
                               newProfileForm.category[0].color.g,
                               newProfileForm.category[0].color.b, newProfileForm.color.a));

    int offset = outlineLarge - outlineNormal;

    for (int i = offset; i > 0 ; --i)
    {
        SDL_SetTextureAlphaMod(newProfileForm.circleTexture, prop(255, (newProfileForm.color.a / (double)255) * 100) - (prop(255, (newProfileForm.color.a / (double)255) * 100) / offset * i));

        SDL_SetTextureColorMod(newProfileForm.circleTexture, prop(newProfileForm.selectedColor.r, 80), prop(newProfileForm.selectedColor.g, 80), prop(newProfileForm.selectedColor.b, 80));

        tempRect.x = newProfileForm.overviewPpRect.x - (outlineLarge - offset + i);
        tempRect.y = newProfileForm.overviewPpRect.y - (outlineLarge - offset + i );
        tempRect.h = newProfileForm.overviewPpRect.h + ((outlineLarge - offset + i) * 2);
        tempRect.w = tempRect.h;

        SDL_RenderCopy(*renderer, newProfileForm.circleTexture, NULL, &tempRect);
    }

    SDL_SetTextureAlphaMod(newProfileForm.circleTexture, newProfileForm.color.a);
    SDL_SetTextureColorMod(newProfileForm.circleTexture, newProfileForm.selectedColor.r, newProfileForm.selectedColor.g, newProfileForm.selectedColor.b);

    tempRect.x = newProfileForm.overviewPpRect.x - outlineNormal;
    tempRect.y = newProfileForm.overviewPpRect.y - outlineNormal;
    tempRect.h = newProfileForm.overviewPpRect.h + outlineNormal * 2;
    tempRect.w = tempRect.h;

    SDL_RenderCopy(*renderer, newProfileForm.circleTexture, NULL, &tempRect);

    SDL_SetTextureAlphaMod(newProfileForm.pp[newProfileForm.ppID], newProfileForm.color.a);
    SDL_RenderCopy(*renderer, newProfileForm.pp[newProfileForm.ppID], NULL, &newProfileForm.overviewPpRect);

    if (newProfileForm.selectedFlagTexture != NULL)
    {
        SDL_SetTextureAlphaMod(newProfileForm.selectedFlagTexture, newProfileForm.color.a);
        SDL_RenderCopy(*renderer, newProfileForm.selectedFlagTexture, NULL, &newProfileForm.selectedFlagRect);
    }

    SDL_SetTextureAlphaMod(newProfileForm.userName.texture, newProfileForm.color.a);
    SDL_RenderCopy(*renderer, newProfileForm.userName.texture, NULL, &newProfileForm.userName.clip);

    SDL_SetTextureAlphaMod(newProfileForm.level.texture, newProfileForm.color.a);
    SDL_RenderCopy(*renderer, newProfileForm.level.texture, NULL, &newProfileForm.level.clip);

    SDL_SetTextureAlphaMod(newProfileForm.country.texture, newProfileForm.color.a);
    SDL_RenderCopy(*renderer, newProfileForm.country.texture, NULL, &newProfileForm.country.clip);

    for (int i = 0; i < 3; ++i)
    {
        drawRoundedBoxShadow(&*renderer, newProfileForm.category[i].clip.x, newProfileForm.category[i].clip.y,
                             newProfileForm.category[i].clip.x + newProfileForm.category[i].clip.w ,
                             newProfileForm.category[i].clip.y + newProfileForm.category[i].clip.h, sf(5), 12,
                             newProfileForm.category[i].color.a);

        roundedBoxColor(*renderer, newProfileForm.category[i].clip.x, newProfileForm.category[i].clip.y,
                        newProfileForm.category[i].clip.x + newProfileForm.category[i].clip.w ,
                        newProfileForm.category[i].clip.y + newProfileForm.category[i].clip.h, sf(5),
                        SDL_FOURCC(newProfileForm.category[i].color.r,
                                   newProfileForm.category[i].color.g,
                                   newProfileForm.category[i].color.b, newProfileForm.category[i].color.a));


        newProfileForm.category[i].nextButton.color.a = newProfileForm.category[i].color.a;
        mainMenuButtonRender(&*renderer, newProfileForm.category[i].nextButton, 5);

        SDL_SetTextureAlphaMod(newProfileForm.category[i].label.texture, newProfileForm.category[i].color.a);
        SDL_RenderCopy(*renderer, newProfileForm.category[i].label.texture, NULL, &newProfileForm.category[i].label.clip);

        int barHeight = newProfileForm.category[i].clip.y + sf(55);
        int barWidth =  prop(newProfileForm.category[i].clip.w - sf(50),
                             newProfileForm.category[i].animProp);

        int x1 = (newProfileForm.category[i].clip.x + (newProfileForm.category[i].clip.w / 2)) -
                 (barWidth / 2);

        SDL_SetRenderDrawColor(*renderer, 255, 255, 255, newProfileForm.category[i].color.a);

        for (int i = 0; i < 2; ++i)
        {
            SDL_RenderDrawLine(*renderer, x1 , barHeight + i, x1 + barWidth ,
                               barHeight + i);
        }
    }

    int outlineSmallSize = sf(3);

    SDL_SetTextureAlphaMod(newProfileForm.circleTexture, prop(192, (newProfileForm.category[0].color.a / (double)255) * 100));
    SDL_SetTextureColorMod(newProfileForm.circleTexture, 255, 255, 255);

    for (int i = 0; i < 9; ++i)
    {
        if (newProfileForm.ppID == i || newProfileForm.ppIsHovered[i])
        {
            tempRect.x = newProfileForm.ppRect[i].x - outlineSmallSize;
            tempRect.y = newProfileForm.ppRect[i].y - outlineSmallSize;
            tempRect.h = newProfileForm.ppRect[i].h + outlineSmallSize * 2;
            tempRect.w = tempRect.h;

            SDL_RenderCopy(*renderer, newProfileForm.circleTexture, NULL, &tempRect);
        }

        SDL_SetTextureAlphaMod(newProfileForm.pp[i], newProfileForm.category[0].color.a);
        SDL_RenderCopy(*renderer, newProfileForm.pp[i], NULL, &newProfileForm.ppRect[i]);
    }

    SDL_SetTextureAlphaMod(newProfileForm.picker.hueTexture, newProfileForm.category[1].color.a);
    SDL_RenderCopy(*renderer, newProfileForm.picker.hueTexture, NULL, &newProfileForm.picker.hueRect);

    SDL_SetTextureAlphaMod(newProfileForm.picker.gradiantTexture, newProfileForm.category[1].color.a);
    SDL_RenderCopy(*renderer, newProfileForm.picker.gradiantTexture, NULL, &newProfileForm.picker.gradiantRect);

    tempRect.x = ((newProfileForm.picker.currentColor.h / 360.0) * newProfileForm.picker.hueRect.w) - (sf(4) / 2) + newProfileForm.picker.hueRect.x;
    tempRect.y = newProfileForm.picker.gradiantRect.y + newProfileForm.picker.gradiantRect.h;
    tempRect.w = sf(4);
    tempRect.h = newProfileForm.picker.hueRect.h;

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, prop(170, (newProfileForm.category[1].color.a / (double)255) * 100));

    SDL_RenderFillRect(*renderer, &tempRect);

    SDL_SetTextureColorMod(newProfileForm.circleTexture, 255, 255, 255);
    SDL_SetTextureAlphaMod(newProfileForm.circleTexture, prop(170, (newProfileForm.category[1].color.a / (double)255) * 100));

    tempRect.x = newProfileForm.picker.gradiantRect.x + (newProfileForm.picker.currentColor.s  * newProfileForm.picker.gradiantRect.w - 3);
    tempRect.y = newProfileForm.picker.gradiantRect.y + ((1.0 - newProfileForm.picker.currentColor.v) * newProfileForm.picker.gradiantRect.w - 3);
    tempRect.w = sf(5);
    tempRect.h = sf(5);

    SDL_RenderCopy(*renderer, newProfileForm.circleTexture, NULL, &tempRect);

    newProfileForm.nickInput.color.a = newProfileForm.category[2].color.a;
    renderTextInput(&*renderer, newProfileForm.nickInput);

    newProfileForm.countryInput.color.a = newProfileForm.category[2].color.a;
    renderTextInput(&*renderer, newProfileForm.countryInput);

    for (int i = 0; i < 6; ++i)
    {
        if (newProfileForm.flagTexture[i] != NULL)
        {
            if (newProfileForm.flagButton[i].isSelected)
            {
                roundedBoxColor(*renderer, newProfileForm.flagButton[i].clip.x, newProfileForm.flagButton[i].clip.y,
                                newProfileForm.flagButton[i].clip.x + newProfileForm.flagButton[i].clip.w ,
                                newProfileForm.flagButton[i].clip.y + newProfileForm.flagButton[i].clip.h, sf(5),
                                SDL_FOURCC(255, 255, 255, prop(128, (newProfileForm.category[1].color.a / (double)255) * 100)));
            }

            SDL_SetTextureAlphaMod(newProfileForm.flagTexture[i], newProfileForm.category[2].color.a);
            SDL_RenderCopy(*renderer, newProfileForm.flagTexture[i], NULL, &newProfileForm.flagRect[i]);
        }
    }

    if (newProfileForm.flagInfo.texture != NULL)
    {
        SDL_SetTextureAlphaMod(newProfileForm.flagInfo.texture, newProfileForm.category[2].color.a);
        SDL_RenderCopy(*renderer, newProfileForm.flagInfo.texture, NULL, &newProfileForm.flagInfo.clip);
    }

    SDL_SetTextureAlphaMod(newProfileForm.infoLabel[0].texture, newProfileForm.category[2].color.a);
    SDL_RenderCopy(*renderer, newProfileForm.infoLabel[0].texture, NULL, &newProfileForm.infoLabel[0].clip);

    SDL_SetTextureAlphaMod(newProfileForm.infoLabel[1].texture, newProfileForm.category[2].color.a);
    SDL_RenderCopy(*renderer, newProfileForm.infoLabel[1].texture, NULL, &newProfileForm.infoLabel[1].clip);

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

textInput createTextInput(SDL_Renderer **renderer, int x, int y, int w, int h, int printCharacters) {

    char temp[50];

    textInput element;

    element.clip.w = w;
    element.clip.h = h;
    element.clip.x = x;
    element.clip.y = y;

    element.maxCharacters = 20;

    element.toPrint = createTextTexture(&*renderer, "", "../fonts/Gonsterrat.ttf",
                                        sf(20), element.clip.x + sf(10),
                                        element.clip.y + element.clip.h - sf(10),
                                        LEFT | BOTTOM);

    strcpy(element.text , "");

    if (printCharacters)
    {
        sprintf(temp, "%s 0 / %d", gs("Number of character"), element.maxCharacters);

        element.maxC = createTextTexture(&*renderer, temp, "../fonts/Gonsterrat.ttf",
                                         sf(15), element.clip.x + sf(10),
                                         element.clip.y + element.clip.h + sf(20),
                                         LEFT | BOTTOM);
    }

    element.cursor.x = element.clip.x + sf(10);

    element.cursor.h = sf(20);

    element.cursor.y = element.clip.y + (element.clip.h / 2) - element.cursor.h / 2;

    element.cursor.w = sf(2);

    element.printCharacters = printCharacters;
    element.textChanged = 0;
    element.isFocused = 0;
    element.isHovered = 0;

    return element;
}

form2 initNewProfileForm(SDL_Renderer **renderer) {

    form2 element;

    element.clip.x = sf(70);
    element.clip.y = sf(60);
    element.clip.h = sf(720) - (element.clip.y * 2);
    element.clip.w = sf(1280) - (element.clip.x * 2);

    element.overview.x = element.clip.x;
    element.overview.y = element.clip.y;
    element.overview.h = sf(100);
    element.overview.w = element.clip.w / 2;

    element.overviewPpRect.h = element.overview.h * 0.75;
    element.overviewPpRect.w = element.overviewPpRect.h;
    element.overviewPpRect.y = element.overview.y + (element.overview.h / 2) -
                               (element.overviewPpRect.h / 2);
    element.overviewPpRect.x = element.overview.x + element.overviewPpRect.y -
                               element.overview.y;

    element.selectedFlagTexture = NULL;

    element.selectedFlagRect.h = sf(30);
    element.selectedFlagRect.x = element.overviewPpRect.x + sf(40);
    element.selectedFlagRect.y = element.overviewPpRect.y + element.overviewPpRect.y - sf(20);

    element.userName = createTextTexture(&*renderer, "______", FONT_STANDARD,
                                         sf(25), element.overviewPpRect.x + element.overviewPpRect.w + sf(20),
                                         element.overview.y + sf(45),
                                         LEFT | BOTTOM);

    element.level = createTextTexture(&*renderer, "LVL 0", FONT_STANDARD,
                                      sf(10), element.overviewPpRect.x + element.overviewPpRect.w + sf(20),
                                      element.overview.y + sf(54),
                                      LEFT | BOTTOM);

    element.country = createTextTexture(&*renderer, "___", FONT_STANDARD,
                                        sf(15), element.overviewPpRect.x + element.overviewPpRect.w + sf(20),
                                        element.overview.y + sf(80),
                                        LEFT | BOTTOM);

    element.circleTexture = loadTexture(&*renderer, "../images/circle.png");

    int offset = sf(65);

    char temp[6][30] = {0};

    strcpy(temp[0], gs("Next"));
    strcpy(temp[1], gs("Next"));
    strcpy(temp[2], gs("Finish"));

    strcpy(temp[3], gs("Picture"));
    strcpy(temp[4], gs("Color"));
    strcpy(temp[5], gs("Infos"));

    int nextButtonWidth = sf(100);
    int nextButtonHeight = sf(40);

    for (int i = 0; i < 3; ++i)
    {
        element.category[i].clip.w = (element.clip.w - (2 * offset)) / 3 ;
        element.category[i].clip.h = element.clip.h - element.overview.h - offset;
        element.category[i].clip.y = element.clip.y + element.overview.h + offset;
        element.category[i].clip.x = element.clip.x +
                                     (i * element.category[i].clip.w) +
                                     (i * offset);

        element.category[i].nextButton = mainMenuCreateButtonRGBA(&*renderer, sf(20),
                                         temp[i], element.category[i].clip.x + element.category[i].clip.w
                                         - nextButtonWidth - sf(20),
                                         element.category[i].clip.y + element.category[i].clip.h -
                                         nextButtonHeight - sf(20), nextButtonWidth,
                                         nextButtonHeight, i, 46,
                                         204, 113, 255);

        element.category[i].label = createTextTexture(&*renderer, temp[i + 3], FONT_STANDARD,
                                    sf(30), element.category[i].clip.x + (element.category[i].clip.w / 2),
                                    element.category[i].clip.y + sf(35),
                                    HCENTER | VCENTER);

        element.category[i].animProp = 0;
    }

    for (int i = 0; i < 9; ++i)
    {
        sprintf(temp[0], "../images/pp/profile-%d.png", i + 1);

        element.pp[i] = loadTexture(&*renderer, temp[0]);

        element.ppRect[i].h = sf(60);
        element.ppRect[i].w = element.ppRect[i].h;

        element.ppRect[i].x = element.category[0].clip.x +
                              ((element.category[0].clip.w) / 4) * ((i - ((i / 3) * 3)) + 1) -
                              (element.ppRect[i].w / 2);

        element.ppRect[i].y = element.category[0].clip.y + ((element.ppRect[1].x -
                              element.ppRect[0].x ) * (i / 3)) + sf(130) -
                              (element.ppRect[i].h / 2);

        element.ppIsHovered[i] = 0;

    }

    element.picker.currentColor.h = 180.0;
    element.picker.currentColor.s = 0.0;
    element.picker.currentColor.v = 1.0;

    element.selectedColor.r = 255;
    element.selectedColor.g = 255;
    element.selectedColor.b = 255;
    element.selectedColor.a = 255;

    //updating gradiant

    element.picker.gradiantRect.h = sf(200);
    element.picker.gradiantRect.w = sf(200);

    element.picker.gradiantRect.x = element.category[1].clip.x + sf(40);
    element.picker.gradiantRect.y = element.category[1].clip.y + sf(100);

    element.picker.gradiantTexture = NULL;

    colorPicker_UpdateGradiantTexture(&*renderer, &element.picker);


    //building hue selector

    element.picker.hueRect.x = element.picker.gradiantRect.x;
    element.picker.hueRect.y = element.picker.gradiantRect.y + element.picker.gradiantRect.h;
    element.picker.hueRect.w = element.picker.gradiantRect.w;
    element.picker.hueRect.h = sf(20);

    SDL_Surface * surface = SDL_CreateRGBSurface(
                                0, element.picker.hueRect.w, element.picker.hueRect.h , 32,
                                0x000000ff, 0x0000ff00,
                                0x00ff0000, 0xff000000);

    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            HSVColor hsv_color = {
                (double) x / surface->w * 360,
                1.0, 1.0,
            };

            SDL_Color color = from_RGBColor(hsv_to_rgb(hsv_color));

            ((char*)surface->pixels)[(x + y * surface->w) * 4    ] = color.r;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 1] = color.g;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 2] = color.b;
            ((char*)surface->pixels)[(x + y * surface->w) * 4 + 3] = color.a;
        }
    }

    element.picker.hueTexture = SDL_CreateTextureFromSurface(*renderer, surface);
    SDL_FreeSurface(surface);

    element.nickInput = createTextInput(&*renderer, element.category[2].clip.x + (element.category[2].clip.w / 2) - sf(300) / 2,
                                        element.category[2].clip.y + sf(130),
                                        sf(300), sf(40), 1);
    element.nickInput.color.r = 46;
    element.nickInput.color.g = 64;
    element.nickInput.color.b = 83;
    element.nickInput.color.a = 255;


    element.countryInput = createTextInput(&*renderer, element.nickInput.clip.x,
                                           element.category[2].clip.y + sf(250),
                                           sf(300), sf(40), 0);
    element.countryInput.color.r = 46;
    element.countryInput.color.g = 64;
    element.countryInput.color.b = 83;
    element.countryInput.color.a = 255;

    for (int i = 0; i < 6; ++i)
    {
        element.flagTexture[i] = NULL;

        element.flagButton[i].isSelected = 0;
        element.flagButton[i].id = i;
    }

    element.flagInfo = createTextTexture(&*renderer, "___", "../fonts/Gonsterrat.ttf",
                                         sf(10), element.countryInput.clip.x + sf(10),
                                         element.countryInput.clip.y + sf(85),
                                         LEFT | TOP);

    element.flagInfo.texture = NULL;

    element.infoLabel[0] = createTextTexture(&*renderer, gs("Nickname"), FONT_STANDARD,
                           sf(25), element.nickInput.clip.x + sf(10),
                           element.category[2].clip.y + sf(110),
                           LEFT | VCENTER);

    element.infoLabel[1] = createTextTexture(&*renderer, gs("Flag"), FONT_STANDARD,
                           sf(25), element.countryInput.clip.x + sf(10),
                           element.category[2].clip.y + sf(230),
                           LEFT | VCENTER);


    element.picker.selectionID = 0;

    element.ppID = 0;

    element.category[0].color.r = 46;
    element.category[0].color.g = 64;
    element.category[0].color.b = 83;
    element.category[0].color.a = 0;

    element.category[1].color.r = 52;
    element.category[1].color.g = 73;
    element.category[1].color.b = 94;
    element.category[1].color.a = 0;

    element.category[2].color.r = 93;
    element.category[2].color.g = 109;
    element.category[2].color.b = 126;
    element.category[2].color.a = 0;

    element.categoryOpenned = 0;

    element.color.a = 0;

    element.animProp = 0;

    element.categoryOpenned = 0;

    strcpy(element.nick, "");
    strcpy(element.countryName, "");

    return element;
}

void renderError(SDL_Renderer **renderer, error textError) {

    if (textError.isSet)
    {
        SDL_SetTextureColorMod(textError.label.texture, textError.color.r, textError.color.g, textError.color.b);
        SDL_SetTextureAlphaMod(textError.label.texture, textError.color.a);

        SDL_RenderCopy(*renderer, textError.label.texture, NULL, &textError.label.clip);
    }
}


void createProfileRefresh(SDL_Renderer **renderer, int alpha, SDL_Texture *background, form2 newProfileForm, error *textErrors) {

    SDL_RenderCopy(*renderer, background, NULL, NULL);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 192);
    SDL_RenderFillRect(*renderer, NULL);

    renderNewProfileForm(&*renderer, newProfileForm);

    int numberOfErrors = 0;
    int errorsStartY = newProfileForm.category[2].clip.y + newProfileForm.category[2].clip.h - sf(10);
    int largestStringW = 0;
    int blackBoxOffset = sf(10);

    for (int i = 0; i < 3; ++i)
    {
        if (textErrors[i].isSet)
        {
            if (textErrors[i].label.clip.w > largestStringW)
            {
                largestStringW = textErrors[i].label.clip.w;
            }

            textErrors[i].label.clip.y = errorsStartY + (numberOfErrors * sf(18));
            numberOfErrors++;
        }
    }

    if (numberOfErrors != 0)
    {
        roundedBoxColor(*renderer, textErrors[0].label.clip.x - blackBoxOffset / 2, errorsStartY - blackBoxOffset / 2,
                        textErrors[0].label.clip.x + largestStringW + blackBoxOffset / 2,
                        errorsStartY + (numberOfErrors * sf(18)) + blackBoxOffset / 2, sf(5),
                        SDL_FOURCC(0, 0, 0, 144));
    }

    for (int i = 0; i < 3; ++i)
    {
        renderError(&*renderer, textErrors[i]);
    }

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);
}

void updateTextInput(SDL_Renderer **renderer, textInput *input) {

    char *temp = malloc(sizeof(char) * input->maxCharacters + 5);

    if (input->toPrint.texture != NULL)
    {
        SDL_DestroyTexture(input->toPrint.texture);
    }

    updateTextTexture(&*renderer, &input->toPrint,
                      input->text, "../fonts/Gonsterrat.ttf", sf(20));

    if (input->printCharacters)
    {
        sprintf(temp, "%s %d / %d", gs("Number of character"), (int)strlen(input->text), input->maxCharacters);

        if (input->maxC.texture != NULL)
        {
            SDL_DestroyTexture(input->maxC.texture);
        }

        updateTextTexture(&*renderer, &input->maxC,
                          temp, "../fonts/Gonsterrat.ttf", sf(15));
    }

    input->cursor.x = input->toPrint.clip.x + input->toPrint.clip.w + sf(2);

    free(temp);
}

void textInputGetSate(SDL_Renderer **renderer, SDL_Point mousePosition, textInput *input, SDL_Event event, int *isChange) {

    if (input->textChanged)
    {
        input->textChanged = 0;
    }

    switch (event.type)
    {
    case SDL_MOUSEMOTION:

        if (ifPointInSquare(input->clip, mousePosition))
        {
            if (!input->isHovered)
            {
                input->isHovered = 1;
            }
        }
        else if (input->isHovered)
        {
            input->isHovered = 0;
        }

        break;

    case SDL_MOUSEBUTTONDOWN:

        if (input->isHovered)
        {
            input->isFocused = 1;
            *isChange = 1;

            SDL_StartTextInput();
        }
        else if (input->isFocused)
        {
            input->isFocused = 0;
            *isChange = 1;

            SDL_StopTextInput();
        }

        break;

    case SDL_TEXTINPUT:

        if (input->isFocused)
        {
            if (strlen(input->text) < input->maxCharacters)
            {
                strcat(input->text, event.text.text);

                updateTextInput(&*renderer, input);

                input->textChanged = 1;

                *isChange = 1;
            }
        }

        break;

    case SDL_KEYDOWN:

        if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
            if (input->isFocused)
            {
                if (strlen(input->text) > 0) {

                    input->text[strlen(input->text) - 1] = '\0';
                    updateTextInput(&*renderer, input);

                    input->textChanged = 1;

                    if (strlen(input->text) == 0)
                    {
                        input->cursor.x = input->clip.x + sf(10);
                    }
                }

                *isChange = 1;
            }
        }

        break;
    }
}

void freeTextInput(textInput *input) {

    SDL_DestroyTexture(input->maxC.texture);
    SDL_DestroyTexture(input->toPrint.texture);
}

void getInitialsSettingsRefresh(SDL_Renderer **renderer,
                                int alpha, SDL_Texture *background,
                                SDL_Rect panelClip, selectBar langBar,
                                int elementsAlpha, button nextButton,
                                text label) {

    SDL_RenderCopy(*renderer, background, NULL, NULL);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 192);
    SDL_RenderFillRect(*renderer, NULL);

    drawRoundedBoxShadow(&*renderer, panelClip.x, panelClip.y,
                         panelClip.x + panelClip.w ,
                         panelClip.y + panelClip.h, sf(5),
                         12, elementsAlpha);

    roundedBoxColor(*renderer, panelClip.x, panelClip.y,
                    panelClip.x + panelClip.w ,
                    panelClip.y + panelClip.h, sf(5),
                    SDL_FOURCC( 52, 73, 94, elementsAlpha));

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    SDL_SetTextureAlphaMod(label.texture, elementsAlpha);
    SDL_RenderCopy(*renderer, label.texture, NULL, &label.clip);

    nextButton.color.a = elementsAlpha;
    mainMenuButtonRender(&*renderer, nextButton, 5);

    langBar.color.a = elementsAlpha;
    renderSelectBar(&*renderer, langBar);

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);
}

void getInitialLang(SDL_Renderer **renderer,
                    int *mainContinue, SDL_Cursor **cursor,
                    int *menuContinue) {

    int isChange = 1;
    int LocalContinue = 1;
    int localStart = 1;
    int prev = 0;
    int actual = 0;
    int alpha = 255;
    int canQuit = 0;
    int normalQuit = 1;

    SDL_Event event;

    SDL_Texture *background = loadTexture(&*renderer,
                                          "../images/menu_background.jpg");

    Mix_Chunk *select = Mix_LoadWAV("../sound/effects/select.wav");

    Mix_Chunk *click = Mix_LoadWAV("../sound/effects/clic.wav");

    SDL_Point mousePosition;

    SDL_Rect panelClip = {sf(100), (sf(720) / 2) - sf(300), sf(400), sf(200)};

    char temp[6][30] = {0};
    char tempString[50] = {0};

    selectBarItem langBarItems[6];
    selectBar langBar;

    int elementsAlpha = 0;

    text label;
    button nextButton;
    int nextButtonHeight = sf(30);
    int nextButtonWidth = sf(130);

    int stringsNeedReload = 0;
    int loadStrings = 1;

    userSettings tempSettings;

    int ret = 0;

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    while (!canQuit)
    {
        isChange = 0;
        SDL_Delay(2);

        if (stringsNeedReload || loadStrings)
        {
            if (stringsNeedReload)
            {
                freeSelectBar(&langBar);
                SDL_DestroyTexture(label.texture);
                SDL_DestroyTexture(nextButton.label.texture);
            }

            label = createTextTexture(&*renderer, gs("Choose a lang to continue"), "../fonts/Gonsterrat.ttf",
                                      sf(15), panelClip.x + sf(30),
                                      panelClip.y + sf(30),
                                      LEFT | VCENTER);

            nextButton = mainMenuCreateButtonRGBA(&*renderer, sf(18),
                                                  gs("Continue"), panelClip.x + panelClip.w
                                                  - nextButtonWidth - sf(30),
                                                  panelClip.y + panelClip.h -
                                                  nextButtonHeight - sf(30), nextButtonWidth,
                                                  nextButtonHeight, 1, 46,
                                                  204, 113, 255);

            strcpy(temp[0], gs("English"));
            strcpy(temp[1], gs("French"));
            strcpy(temp[2], gs("Spanish"));
            strcpy(temp[3], gs("Portuguese"));
            strcpy(temp[4], gs("German"));
            strcpy(temp[5], gs("Italian"));

            for (int i = 0; i < 6; ++i)
            {
                sprintf(tempString, "../images/selectBar/langBar/langBar-%d.jpg", i);

                langBarItems[i].texture = loadTexture(&*renderer, tempString);

                langBarItems[i].text = createTextTexture(&*renderer, temp[i],
                                       FONT_STANDARD, sf(16), sf(675), sf(340),
                                       LEFT | BOTTOM);
            }

            langBar = createSelectBarRGBA(&*renderer,
                                          panelClip.x + sf(30),
                                          panelClip.y + sf(70), sf(20), sf(220), 6,
                                          langBarItems, 93, 109, 126, 255);

            setSelectBarValue(&langBar, getUserSettings().selectedLanguage);

            isChange = 1;

            stringsNeedReload = 0;
            loadStrings = 0;
        }

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;
                normalQuit = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                if (!langBar.deploy)
                {
                    buttonGetState(&*renderer, &nextButton, &select, &isChange,
                                   mousePosition);
                }

                break;

            case SDL_MOUSEBUTTONDOWN:

                if (!langBar.deploy)
                {
                    ret = 0;

                    buttonOnClick(nextButton.clip, mousePosition, &ret,
                                  nextButton.id, &click);

                    if (ret != 0)
                    {
                        LocalContinue = 0;
                    }
                }

                break;
            }

            selectBarGetState(&langBar, event, mousePosition, &isChange);

            if (langBar.changedValue == 1)
            {
                tempSettings = getUserSettings();
                tempSettings.selectedLanguage = langBar.item[0].itemId;
                setUserSettings(tempSettings);

                setCurrentLang(getUserSettings().selectedLanguage);

                stringsNeedReload = 1;

                langBar.changedValue = 0;
            }
        }

        selectBarAnimation(&langBar, &isChange, SDL_GetTicks());

        if (LocalContinue == 0) {

            actual = SDL_GetTicks();

            if (elementsAlpha > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    elementsAlpha = elementsAlpha - 8;

                    if (elementsAlpha <= 0)
                    {
                        elementsAlpha = 0;
                    }

                    isChange = 1;
                }
            }
            else if (alpha < 255 )
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            actual = SDL_GetTicks();

            if (alpha > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= 0)
                    {
                        alpha = 0;
                    }

                    isChange = 1;
                }
            }
            else if (elementsAlpha < 255)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    elementsAlpha = elementsAlpha + 8;

                    if (elementsAlpha >= 255)
                    {
                        elementsAlpha = 255;
                        localStart = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (isChange == 1)
        {
            getInitialsSettingsRefresh(&*renderer, alpha, background, panelClip, langBar, elementsAlpha, nextButton, label);
        }
    }

    freeSelectBar(&langBar);
    SDL_DestroyTexture(label.texture);
    SDL_DestroyTexture(nextButton.label.texture);

    if (normalQuit)
    {
        playVideoDisplay(&*renderer, &*cursor, &*mainContinue, &*menuContinue,
                         "../images/animations/welcome_anim", gs("Welcome/aboard !"), 190, 15);
        if (!*mainContinue || !*menuContinue)
            canQuit = 1;
    }

    setSettingsInitialized();
}

void saveClientInfo();
void setClientProfile(profile tempProfile);

void createProfile(SDL_Renderer **renderer,
                   int *mainContinue, SDL_Cursor **cursor,
                   int *menuContinue) {

    int isChange = 1;
    int LocalContinue = 1;
    int localStart = 1;
    int prev = 0;
    int actual = 0;
    int alpha = 255;
    int canQuit = 0;

    int animRet = -1;

    char tab[6][40];
    char temp[50];
    int flagRet = -1;
    int flagSelected = 0;
    int tempW; int tempH;

    profile tempProfile = initBlankProfile();

    SDL_Event event;

    SDL_Texture *background = loadTexture(&*renderer,
                                          "../images/menu_background.jpg");

    Mix_Chunk *select = Mix_LoadWAV("../sound/effects/select.wav");

    Mix_Chunk *click = Mix_LoadWAV("../sound/effects/clic.wav");

    form2 newProfileForm = initNewProfileForm(&*renderer);

    error errors[3];

    char tempErrorsStrings[3][100] = {0};

    strcpy(tempErrorsStrings[0], gs("Your username must be at least 3 characters long"));
    strcpy(tempErrorsStrings[1], gs("Your country must be valid"));
    strcpy(tempErrorsStrings[2], gs("Failed, please check the status of your internet connection"));

    for (int i = 0; i < 3; ++i)
    {
        errors[i].label = createTextTexture(&*renderer, tempErrorsStrings[i],
                                            "../fonts/Gonsterrat.ttf", sf(12), newProfileForm.category[2].clip.x + sf(20), 0,
                                            LEFT | BOTTOM);
        errors[i].id = i;

        errors[i].isSet = 0;
        errors[i].color.r = 255;
        errors[i].color.g = 0;
        errors[i].color.b = 0;
        errors[i].color.a = 255;
    }

    SDL_Point mousePosition;

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    while (!canQuit)
    {
        isChange = 0;
        SDL_Delay(2);

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                for (int i = 0; i < 3; ++i)
                {
                    if (newProfileForm.categoryOpenned == i)
                    {
                        buttonGetState(&*renderer, &newProfileForm.category[i].nextButton, &select,
                                       &isChange, mousePosition);
                    }
                }

                if (strcmp(newProfileForm.countryInput.text, "") != 0)
                {
                    for (int i = 0; i < 6; ++i)
                    {
                        if (newProfileForm.flagTexture[i] != NULL)
                        {
                            if ( ifPointInSquare(newProfileForm.flagButton[i].clip, mousePosition) == 1 && newProfileForm.flagButton[i].isSelected != 1) {

                                newProfileForm.flagButton[i].isSelected = 1;

                                audioEffect(select, 10);

                                if (newProfileForm.flagInfo.texture != NULL)
                                {
                                    SDL_DestroyTexture(newProfileForm.flagInfo.texture);
                                    newProfileForm.flagInfo.texture = NULL;
                                }

                                updateTextTexture(&*renderer, &newProfileForm.flagInfo,
                                                  gcs(tab[i]), "../fonts/Gonsterrat.ttf",
                                                  sf(12));

                                isChange = 1;

                            }
                            else if (ifPointInSquare(newProfileForm.flagButton[i].clip, mousePosition) == 0 && newProfileForm.flagButton[i].isSelected == 1) {

                                newProfileForm.flagButton[i].isSelected = 0;

                                isChange = 1;
                            }
                        }
                    }

                    flagSelected = 0;

                    for (int i = 0; i < 6; ++i)
                    {
                        if (newProfileForm.flagButton[i].isSelected)
                        {
                            flagSelected = 1;
                        }
                    }

                    if (!flagSelected)
                    {
                        if (newProfileForm.flagInfo.texture != NULL)
                        {
                            SDL_DestroyTexture(newProfileForm.flagInfo.texture);
                            newProfileForm.flagInfo.texture = NULL;
                        }

                        newProfileForm.flagInfo.texture = NULL;
                    }
                }

                for (int i = 0; i < 9; ++i)
                {
                    if (ifPointInSquare(newProfileForm.ppRect[i], mousePosition))
                    {
                        if (!newProfileForm.ppIsHovered[i])
                        {
                            newProfileForm.ppIsHovered[i] = 1;
                            isChange = 1;
                        }
                    }
                    else if (newProfileForm.ppIsHovered[i]) {

                        newProfileForm.ppIsHovered[i] = 0;
                        isChange = 1;
                    }
                }

                break;

            case SDL_MOUSEBUTTONDOWN:

                animRet = -1;

                for (int i = 0; i < 3; ++i)
                {
                    if (newProfileForm.categoryOpenned == i)
                    {
                        buttonOnClick(newProfileForm.category[i].nextButton.clip, mousePosition, &animRet,
                                      newProfileForm.category[i].nextButton.id, &click);
                    }
                }
                if (animRet != -1)
                {
                    switch (animRet) {

                    case 0:

                        newProfileForm.categoryOpenned = 1;
                        newProfileForm.category[1].animIn = 1;
                        newProfileForm.category[0].nextButton.isSelected = 0;

                        break;

                    case 1:

                        newProfileForm.categoryOpenned = 2;
                        newProfileForm.category[2].animIn = 1;
                        newProfileForm.category[1].nextButton.isSelected = 0;

                        break;

                    case 2:

                        if (strlen(newProfileForm.nick) < 3)
                        {
                            errors[0].isSet = 1;
                            isChange = 1;
                        }
                        else {
                            errors[0].isSet = 0;
                            isChange = 1;
                        }

                        if (strcmp(newProfileForm.countryName, "") == 0)
                        {
                            errors[1].isSet = 1;
                            isChange = 1;
                        }
                        else {
                            errors[1].isSet = 0;
                            isChange = 1;
                        }

                        if (!errors[0].isSet && !errors[1].isSet)
                        {
                            if (!getThreadStatut())
                            {
                                tempProfile.id = getClientProfile().id;
                                strcpy(tempProfile.nick, newProfileForm.nick);
                                strcpy(tempProfile.country, newProfileForm.countryName);
                                tempProfile.color = newProfileForm.selectedColor;
                                tempProfile.imgID = newProfileForm.ppID;

                                netPacket packet;
                                packet.client = tempProfile;
                                packet = sandPackedRequest("createProfile", &packet);

                                if (packet.received)
                                {
                                    setClientProfile(packet.client);
                                    saveClientInfo();
                                    LocalContinue = 0;
                                    errors[2].isSet = 0;
                                    isChange = 1;
                                }
                                else {
                                    errors[2].isSet = 1;
                                    isChange = 1;
                                }
                            }

                            break;
                        }
                    }
                }

                if (newProfileForm.categoryOpenned == 2)
                {
                    if (strcmp(newProfileForm.countryInput.text, "") != 0)
                    {
                        flagRet = -1;

                        for (int i = 0; i < 6; ++i)
                        {
                            if (newProfileForm.flagTexture[i] != NULL)
                            {
                                buttonOnClick(newProfileForm.flagButton[i].clip, mousePosition, &flagRet,
                                              newProfileForm.flagButton[i].id, &click);
                            }
                        }

                        if (flagRet != -1)
                        {
                            strcpy(newProfileForm.countryName, tab[flagRet]);

                            if (newProfileForm.country.texture != NULL)
                            {
                                SDL_DestroyTexture(newProfileForm.country.texture);
                                newProfileForm.country.texture = NULL;
                            }

                            updateTextTexture(&*renderer, &newProfileForm.country,
                                              gcs(newProfileForm.countryName), FONT_STANDARD,
                                              sf(15));

                            if (newProfileForm.selectedFlagTexture != NULL)
                            {
                                SDL_DestroyTexture(newProfileForm.selectedFlagTexture);
                                newProfileForm.selectedFlagTexture = NULL;
                            }

                            sprintf(temp, "../images/flags/%s.png", newProfileForm.countryName);

                            newProfileForm.selectedFlagTexture = loadTexture(&*renderer, temp);

                            SDL_QueryTexture(newProfileForm.selectedFlagTexture, NULL, NULL, &tempW, &tempH);

                            newProfileForm.selectedFlagRect.w = newProfileForm.selectedFlagRect.h * (tempW / (double)tempH);

                            isChange = 1;
                        }
                    }
                }

                if (newProfileForm.categoryOpenned >= 1)
                {
                    if (ifPointInSquare(newProfileForm.picker.gradiantRect, mousePosition))
                    {
                        if (newProfileForm.picker.selectionID == 0)
                        {
                            newProfileForm.picker.selectionID = 1;

                        }
                    }
                    else if (ifPointInSquare(newProfileForm.picker.hueRect, mousePosition))
                    {
                        if (newProfileForm.picker.selectionID == 0)
                        {
                            newProfileForm.picker.selectionID = 2;
                        }
                    }
                }

                for (int i = 0; i < 9; ++i)
                {
                    if (newProfileForm.ppIsHovered[i])
                    {
                        newProfileForm.ppID = i;
                        isChange = 1;
                    }
                }

                break;

            case SDL_MOUSEBUTTONUP:

                if (newProfileForm.categoryOpenned >= 1)
                {
                    if (newProfileForm.picker.selectionID != 0)
                    {
                        newProfileForm.picker.selectionID = 0;
                    }
                }

                break;
            }

            if (newProfileForm.categoryOpenned >= 1)
            {
                if (newProfileForm.picker.selectionID == 1)
                {
                    newProfileForm.picker.currentColor.s = ((double) CLAMP(0, newProfileForm.picker.gradiantRect.w, mousePosition.x - newProfileForm.picker.gradiantRect.x) / newProfileForm.picker.gradiantRect.w);
                    newProfileForm.picker.currentColor.v = 1.0 - ((double) CLAMP(0, newProfileForm.picker.gradiantRect.w, mousePosition.y - newProfileForm.picker.gradiantRect.y) / newProfileForm.picker.gradiantRect.w);

                    newProfileForm.selectedColor = from_RGBColor(hsv_to_rgb(newProfileForm.picker.currentColor));

                    isChange = 1;
                }
                else if (newProfileForm.picker.selectionID == 2)
                {
                    newProfileForm.picker.currentColor.h = ((double) CLAMP(0, newProfileForm.picker.hueRect.w, mousePosition.x - newProfileForm.picker.hueRect.x) / newProfileForm.picker.hueRect.w) * 360.0;
                    colorPicker_UpdateGradiantTexture(&*renderer, &newProfileForm.picker);

                    newProfileForm.selectedColor = from_RGBColor(hsv_to_rgb(newProfileForm.picker.currentColor));

                    isChange = 1;
                }
            }

            if (newProfileForm.categoryOpenned == 2)
            {
                textInputGetSate(&*renderer, mousePosition, &newProfileForm.nickInput, event, &isChange);

                textInputGetSate(&*renderer, mousePosition, &newProfileForm.countryInput, event, &isChange);

                if (newProfileForm.nickInput.textChanged)
                {
                    strcpy(newProfileForm.nick, newProfileForm.nickInput.text);

                    if (newProfileForm.userName.texture != NULL)
                    {
                        SDL_DestroyTexture(newProfileForm.userName.texture);
                        newProfileForm.userName.texture = NULL;
                    }

                    if (strlen(newProfileForm.nickInput.text) != 0)
                    {
                        updateTextTexture(&*renderer, &newProfileForm.userName,
                                          newProfileForm.nickInput.text, FONT_STANDARD, sf(25));
                    }
                    else {

                        updateTextTexture(&*renderer, &newProfileForm.userName,
                                          "______", FONT_STANDARD, sf(25));
                    }
                }

                if (newProfileForm.countryInput.textChanged)
                {
                    getCountryTab(newProfileForm.countryInput.text, tab, 6);

                    for (int i = 0; i < 6; ++i)
                    {
                        if (strcmp(tab[i], "") != 0 && strcmp(newProfileForm.countryInput.text, "") != 0)
                        {
                            int flagsHeight = sf(30);
                            sprintf(temp, "../images/flags/%s.png", tab[i]);

                            if (newProfileForm.flagTexture[i] != NULL)
                            {
                                SDL_DestroyTexture(newProfileForm.flagTexture[i]);
                            }

                            newProfileForm.flagTexture[i] = loadTexture(&*renderer, temp);

                            SDL_QueryTexture(newProfileForm.flagTexture[i], NULL, NULL, &tempW, &tempH);

                            if (!i)
                            {
                                newProfileForm.flagRect[i].x = newProfileForm.category[2].clip.x + (newProfileForm.category[2].clip.w / 2) - (newProfileForm.countryInput.clip.w / 2) + sf(10) ;
                            }
                            else {
                                newProfileForm.flagRect[i].x = newProfileForm.flagRect[i - 1].x  + newProfileForm.flagRect[i - 1].w;
                            }

                            newProfileForm.flagRect[i].y = newProfileForm.countryInput.clip.y + newProfileForm.countryInput.clip.h + sf(10);
                            newProfileForm.flagRect[i].h = flagsHeight;
                            newProfileForm.flagRect[i].w = flagsHeight * (tempW / (double)tempH);

                            newProfileForm.flagButton[i].clip = newProfileForm.flagRect[i];
                        }
                        else if ( newProfileForm.flagTexture[i] != NULL) {

                            SDL_DestroyTexture(newProfileForm.flagTexture[i]);

                            newProfileForm.flagTexture[i] = NULL;
                        }
                    }
                }
            }
        }

        if (LocalContinue == 0) {

            actual = SDL_GetTicks();

            if (newProfileForm.category[2].animProp > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.category[2].animProp = newProfileForm.category[2].animProp - 8;

                    if (newProfileForm.category[2].animProp <= 0)
                    {
                        newProfileForm.category[2].animProp = 0;
                    }

                    newProfileForm.category[2].color.a = prop(255, newProfileForm.category[2].animProp);

                    isChange = 1;
                }
            }
            else if (newProfileForm.category[1].animProp > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.category[1].animProp = newProfileForm.category[1].animProp - 8;

                    if (newProfileForm.category[1].animProp <= 0)
                    {
                        newProfileForm.category[1].animProp = 0;
                    }

                    newProfileForm.category[1].color.a = prop(255, newProfileForm.category[1].animProp);

                    isChange = 1;
                }
            }

            else if (newProfileForm.category[0].animProp > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.category[0].animProp = newProfileForm.category[0].animProp - 8;

                    if (newProfileForm.category[0].animProp <= 0)
                    {
                        newProfileForm.category[0].animProp = 0;
                    }

                    newProfileForm.category[0].color.a = prop(255, newProfileForm.category[0].animProp);

                    isChange = 1;
                }
            }
            else if (newProfileForm.animProp > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.animProp = newProfileForm.animProp - 8;

                    if (newProfileForm.animProp <= 0)
                    {
                        newProfileForm.animProp = 0;
                    }

                    newProfileForm.color.a = prop(255, newProfileForm.animProp);

                    isChange = 1;
                }
            }
            else if (alpha < 255 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            actual = SDL_GetTicks();

            if (alpha > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= 0)
                    {
                        alpha = 0;

                    }

                    isChange = 1;
                }
            }
            else if (newProfileForm.animProp < 100)
            {
                if (actual > prev + 20)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.animProp = newProfileForm.animProp + 8;

                    if (newProfileForm.animProp >= 100)
                    {
                        newProfileForm.animProp = 100;
                    }

                    newProfileForm.color.a = prop(255, newProfileForm.animProp);

                    isChange = 1;
                }
            }
            else if (newProfileForm.category[0].animProp < 100)
            {
                if (actual > prev + 20)
                {
                    prev = SDL_GetTicks();

                    newProfileForm.category[0].animProp = newProfileForm.category[0].animProp + 8;

                    if (newProfileForm.category[0].animProp >= 100)
                    {
                        newProfileForm.category[0].animProp = 100;
                        localStart = 0;
                    }

                    newProfileForm.category[0].color.a = prop(255, newProfileForm.category[0].animProp);

                    isChange = 1;
                }
            }
        }

        for (int i = 1; i <= 2; ++i)
        {
            if (newProfileForm.category[i].animIn)
            {
                actual = SDL_GetTicks();

                if (newProfileForm.category[i].animProp < 100)
                {
                    if (actual > prev + 20)
                    {
                        prev = SDL_GetTicks();

                        newProfileForm.category[i].animProp = newProfileForm.category[i].animProp + 8;

                        if (newProfileForm.category[i].animProp >= 100)
                        {
                            newProfileForm.category[i].animProp = 100;
                            newProfileForm.category[i].animIn = 0;
                        }

                        newProfileForm.category[i].color.a = prop(255, newProfileForm.category[i].animProp);

                        isChange = 1;
                    }
                }
            }
        }

        if (isChange == 1)
        {
            createProfileRefresh(&*renderer, alpha, background, newProfileForm, errors);
        }
    }

    SDL_DestroyTexture(newProfileForm.circleTexture);

    SDL_DestroyTexture(newProfileForm.selectedFlagTexture);

    for (int i = 0; i < 9; ++i)
    {
        SDL_DestroyTexture(newProfileForm.pp[i]);
    }

    for (int i = 0; i < 3; ++i)
    {
        SDL_DestroyTexture(newProfileForm.category[i].label.texture);
        SDL_DestroyTexture(newProfileForm.category[i].nextButton.label.texture);
    }

    freeTextInput(&newProfileForm.nickInput);
    freeTextInput(&newProfileForm.countryInput);

    SDL_DestroyTexture(newProfileForm.picker.hueTexture);
    SDL_DestroyTexture(newProfileForm.picker.gradiantTexture);

    SDL_DestroyTexture(newProfileForm.userName.texture);
    SDL_DestroyTexture(newProfileForm.country.texture);
    SDL_DestroyTexture(newProfileForm.level.texture);

    for (int i = 0; i < 6; ++i)
    {
        if (newProfileForm.flagTexture[i] != NULL)
        {
            SDL_DestroyTexture(newProfileForm.flagTexture[i]);
        }
    }
}

button inGameMenu_createButton(SDL_Renderer **renderer, char fontLocation[],
                               int fontSize, const char buttonText[],
                               int x, int y, int id) {

    button btn;

    btn.label = createTextTexture(&*renderer, buttonText, fontLocation,
                                  fontSize, x, y, LEFT | VCENTER);

    btn.clip.x = x;
    btn.clip.y = y;
    btn.clip.w = btn.label.clip.w;
    btn.clip.h = btn.label.clip.h;
    btn.isSelected = 0;

    btn.label.clip.x = x;
    btn.label.clip.y = y;

    btn.activated = 1;
    btn.alpha = 0;

    btn.id = id;

    return btn;
}

void inGameMenu_buttonRender(SDL_Renderer **renderer, button btn) {

    if (btn.isSelected == 1)
    {
        btn.label.clip.h = btn.label.clip.h * 1.1;
        btn.label.clip.w = btn.label.clip.w * 1.1;
        btn.label.clip.y = btn.label.clip.y - 5;
        SDL_SetTextureColorMod(btn.label.texture, 248, 196, 113);
    }
    else
    {
        SDL_SetTextureColorMod(btn.label.texture, 255, 255, 255);
    }

    SDL_SetTextureAlphaMod(btn.label.texture, btn.alpha);

    SDL_RenderCopy(*renderer, btn.label.texture, NULL, &btn.label.clip);

}

void renderMsgBox(SDL_Renderer **renderer, msgBox msg) {

    int colorOffset = 40;

    for (int i = 0; i < 12; ++i)
    {

        filledTrigonColor(*renderer, msg.trigonPoint[0].x + i,
                          msg.trigonPoint[0].y + i , msg.trigonPoint[1].x + i,
                          msg.trigonPoint[1].y + i, msg.trigonPoint[2].x + i,
                          msg.trigonPoint[2].y + i, SDL_FOURCC(0, 0, 0, 21));
    }

    drawRoundedBoxShadow(&*renderer, msg.clip.x, msg.clip.y,
                         msg.clip.x + sf(21), msg.clip.y + sf(21), sf(3), 12,
                         255);

    drawRoundedBoxShadow(&*renderer, msg.clip.x, msg.clip.y,
                         msg.clip.x + prop(msg.clip.w, msg.propValue),
                         msg.clip.y + prop(msg.clip.h, msg.propValue), sf(5),
                         12, 255);

    filledTrigonColor(*renderer, msg.trigonPoint[0].x, msg.trigonPoint[0].y ,
                      msg.trigonPoint[1].x, msg.trigonPoint[1].y,
                      msg.trigonPoint[2].x, msg.trigonPoint[2].y,
                      SDL_FOURCC(prop(msg.color.r, (colorOffset * msg.hovered)
                                      + 100 - colorOffset),
                                 prop(msg.color.g, (colorOffset * msg.hovered)
                                      + 100 - colorOffset),
                                 prop(msg.color.b, (colorOffset * msg.hovered)
                                      + 100 - colorOffset), 255));

    if (msg.activated == 1 || msg.fadeOut == 1 || msg.fadeIn == 1)
    {
        roundedBoxColor(*renderer, msg.clip.x, msg.clip.y,
                        msg.clip.x + prop(msg.clip.w, msg.propValue) ,
                        msg.clip.y + prop(msg.clip.h, msg.propValue), sf(5),
                        SDL_FOURCC(prop(msg.color.r, (colorOffset * msg.hovered)
                                        + 100 - colorOffset),
                                   prop(msg.color.g, (colorOffset * msg.hovered)
                                        + 100 - colorOffset),
                                   prop(msg.color.b, (colorOffset * msg.hovered)
                                        + 100 - colorOffset), 255));
    }

    roundedBoxColor(*renderer, msg.clip.x, msg.clip.y, msg.clip.x + sf(21) ,
                    msg.clip.y + sf(21), sf(3),
                    SDL_FOURCC(prop(msg.color.r, (colorOffset * msg.hovered)
                                    + 100 - colorOffset),
                               prop(msg.color.g, (colorOffset * msg.hovered)
                                    + 100 - colorOffset),
                               prop(msg.color.b, (colorOffset * msg.hovered)
                                    + 100 - colorOffset), 255));

    SDL_RenderCopyEx(*renderer, msg.arrowText.texture, NULL,
                     &msg.arrowText.clip, prop(90, msg.propValue), NULL,
                     SDL_FLIP_NONE);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    if (msg.clip.x + prop(msg.clip.w, msg.propValue) - sf(13) > msg.clip.x) {

        for (int i = 0; i < 2; ++i)
        {
            SDL_RenderDrawLine(*renderer, msg.clip.x + sf(13), msg.clip.y +
                               sf(28) + i, msg.clip.x +
                               prop(msg.clip.w, msg.propValue)
                               - sf(13), msg.clip.y + i + sf(28));
        }
    }

    SDL_SetTextureAlphaMod(msg.title.texture, msg.textAlpha);

    SDL_RenderCopy(*renderer, msg.title.texture, NULL, &msg.title.clip);

    for (int i = 0; i < msg.nbLines; ++i)
    {
        SDL_SetTextureAlphaMod(msg.line[i].texture, msg.textAlpha);
        SDL_RenderCopy(*renderer, msg.line[i].texture, NULL, &msg.line[i].clip);
    }

    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
}

void inGameMenuRefresh(SDL_Renderer **renderer, SDL_Texture * sshot,
                       SDL_Rect menuRect, button btn1, button btn2, button btn3,
                       button btn4,
                       scrollBar * musicBar, scrollBar * effectsBar,
                       msgBox msg) {

    SDL_RenderCopy(*renderer, sshot, NULL, NULL);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 220);

    SDL_RenderFillRect(*renderer, &menuRect);

    for (int i = 0; i < 220; ++i)
    {
        SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 220 - i);
        SDL_RenderDrawLine(*renderer, menuRect.w + i, 0, menuRect.w + i,
                           sf(720));
    }

    inGameMenu_buttonRender(&*renderer, btn1);
    inGameMenu_buttonRender(&*renderer, btn2);
    inGameMenu_buttonRender(&*renderer, btn3);
    inGameMenu_buttonRender(&*renderer, btn4);

    scrollBarRender(&*renderer, *musicBar);
    scrollBarRender(&*renderer, *effectsBar);

    setChannelVolume(effectsBar->value, 1);
    setMusicVolume(musicBar->value);

    renderMsgBox(&*renderer, msg);

    SDL_RenderPresent(*renderer);
}

msgBox createMsgBoxRGBA(SDL_Renderer **renderer, char *title, char *text, int x,
                        int y, SDL_Color color, int arrow, char *fontLocation) {

    int tempW;
    int tempH;

    msgBox element;

    element.clip.x = x;
    element.clip.y = y;
    element.clip.w = -1;

    element.color = color;

    element.propValue = 0;
    element.activated = 0;
    element.hovered = 0;

    switch (arrow) {

    case TOP:

        element.trigonPoint[0].x = element.clip.x;
        element.trigonPoint[0].y = element.clip.y + sf(2);

        element.trigonPoint[1].x = element.clip.x + sf(21);
        element.trigonPoint[1].y = element.clip.y + sf(2);

        element.trigonPoint[2].x = element.clip.x + sf(11);
        element.trigonPoint[2].y = element.clip.y - sf(15);

        break;

    case LEFT:

        element.trigonPoint[0].x = element.clip.x + sf(2);
        element.trigonPoint[0].y = element.clip.y;

        element.trigonPoint[1].x = element.clip.x - sf(15);
        element.trigonPoint[1].y = element.clip.y + sf(11);

        element.trigonPoint[2].x = element.clip.x + sf(2);
        element.trigonPoint[2].y = element.clip.y + sf(21);

        break;

    case RIGHT:

        element.trigonPoint[0].x = element.clip.x + sf(21) - sf(2);
        element.trigonPoint[0].y = element.clip.y;

        element.trigonPoint[1].x = element.clip.x + sf(21) + sf(15);
        element.trigonPoint[1].y = element.clip.y + sf(11);

        element.trigonPoint[2].x = element.clip.x + sf(21) - sf(2);
        element.trigonPoint[2].y = element.clip.y + sf(21);

        break;

    case BOTTOM:

        element.trigonPoint[0].x = element.clip.x;
        element.trigonPoint[0].y = element.clip.y + sf(21) - sf(2);

        element.trigonPoint[1].x = element.clip.x + sf(11);
        element.trigonPoint[1].y = element.clip.y + sf(21) + sf(15);

        element.trigonPoint[2].x = element.clip.x + sf(21);
        element.trigonPoint[2].y = element.clip.y + sf(21) - sf(2);

        break;
    }

    char* token = strtok(text, "/");

    element.nbLines = 0;

    while (token != NULL && element.nbLines < 10) {

        element.line[element.nbLines] = createTextTexture(&*renderer, token,
                                        fontLocation, sf(15),
                                        element.clip.x + sf(26),
                                        element.clip.y + sf(46) +
                                        element.nbLines * sf(18),
                                        LEFT | VCENTER);

        SDL_QueryTexture(element.line[element.nbLines].texture, NULL, NULL,
                         &tempW, &tempH);

        if (tempW > element.clip.w)
        {
            element.clip.w = tempW + sf(53);
        }

        token = strtok(NULL, "/");

        element.nbLines++;
    }

    element.clip.h = sf(65) + element.nbLines * sf(18);

    element.arrowText = createTextTexture(&*renderer, "y", "../fonts/msg.ttf",
                                          sf(23), element.clip.x + sf(21) / 2,
                                          element.clip.y + sf(21) / 2,
                                          HCENTER | VCENTER);

    SDL_QueryTexture(element.arrowText.texture, NULL, NULL, &tempW, &tempH);


    element.title = createTextTexture(&*renderer, title, fontLocation, sf(20),
                                      element.clip.x + sf(26),
                                      element.clip.y + sf(13), LEFT | VCENTER);

    element.textAlpha = 0;

    return element;
}

SDL_Color randomColor() {

    SDL_Color colorTab[5];

    colorTab[0].r = 231;
    colorTab[0].g = 76;
    colorTab[0].b = 60;

    colorTab[1].r = 241;
    colorTab[1].g = 196;
    colorTab[1].b = 15;

    colorTab[2].r = 46;
    colorTab[2].g = 204;
    colorTab[2].b = 113;

    colorTab[3].r = 52;
    colorTab[3].g = 152;
    colorTab[3].b = 129;

    return colorTab[getRandoms(0, 3)];

}

SDL_Rect msgBoxGetRect(msgBox msg) {

    SDL_Rect tempRect = msg.clip;

    if (msg.activated == 1)
    {
        tempRect.w = prop(msg.clip.w, msg.propValue);
        tempRect.h = prop(msg.clip.h, msg.propValue);
    }
    else {

        tempRect.w = sf(30);
        tempRect.h = sf(30);
    }

    return tempRect;
}

void msgBoxOnClick(msgBox * msg, SDL_Point mousePosition, Mix_Chunk * click) {

    if (ifPointInSquare(msgBoxGetRect(*msg), mousePosition) == 1 ) {

        audioEffect(click, 50);

        if (msg->activated == 1)
        {
            msg->fadeOut = 1;
            msg->fadeIn = 0;
        }
        else {

            msg->fadeOut = 0;
            msg->fadeIn = 1;
        }
    }
}

void msgBoxAnimation(msgBox * msg, int *prev, int actual, int *isChange) {

    if (msg->fadeIn == 1)
    {
        actual = SDL_GetTicks();

        if (msg->propValue < 100)
        {
            if (actual > *prev + 10)
            {
                *prev = SDL_GetTicks();

                msg->propValue = msg->propValue + 8;

                if (msg->propValue >= 100)
                {
                    msg->propValue = 100;
                }

                *isChange = 1;
            }
        }
        else if (msg->textAlpha < 255)
        {
            if (actual > *prev + 10)
            {
                *prev = SDL_GetTicks();

                msg->textAlpha = msg->textAlpha + 32;

                if (msg->textAlpha >= 255)
                {
                    msg->textAlpha = 255;
                    msg->activated = 1;
                    msg->fadeIn = 0;
                }

                *isChange = 1;
            }
        }
    }
    else if (msg->fadeOut == 1) {

        actual = SDL_GetTicks();

        if (msg->textAlpha > 0)
        {
            if (actual > *prev + 10)
            {
                *prev = SDL_GetTicks();

                msg->textAlpha = msg->textAlpha - 32;

                if (msg->textAlpha <= 0)
                {
                    msg->textAlpha = 0;
                }

                *isChange = 1;
            }
        }
        else if (msg->propValue > 0)
        {
            if (actual > *prev + 0)
            {
                *prev = SDL_GetTicks();

                msg->propValue = msg->propValue - 8;

                if (msg->propValue <= 0)
                {
                    msg->propValue = 0;
                    msg->activated = 0;
                    msg->fadeOut = 0;
                }

                *isChange = 1;
            }
        }
    }
}

void msgBoxGetState(SDL_Point mousePosition, int *isChange, msgBox * msg) {

    if ( ifPointInSquare(msgBoxGetRect(*msg), mousePosition) == 1 &&
            msg->hovered != 1) {

        msg->hovered = 1;

        *isChange = 1;

    }
    else if (ifPointInSquare(msgBoxGetRect(*msg), mousePosition) == 0 &&
             msg->hovered == 1) {

        msg->hovered = 0;

        *isChange = 1;
    }
}

void inGameMenu(SDL_Renderer **renderer, int *subLocalContinue,
                int *mainContinue, SDL_Texture * sshot, SDL_Cursor **cursor,
                int *menuContinue, int *langChanged) {

    int isChange = 1;
    int LocalContinue = 1;
    int localStart = 1;
    int ret = 0;
    int panelWidth = sf(450);
    int prev = 0;
    int actual = 0;
    int canQuit = 0;
    int settingsOpened = 0;
    int settingsFadeIn = 0;
    int settingsFadeOut = 0;

    char msgBoxText[500] = {"Ceci est le plateau d'attaque/"
                            "cliquer sur une case pour attaquer votre advairsaire/"
                            "De rien"
                           };

    msgBox msg = createMsgBoxRGBA(&*renderer, "Plateau de l'IA", msgBoxText,
                                  sf(800), sf(400), randomColor(), LEFT,
                                  "../fonts/Gonsterrat.ttf");

    SDL_Event event;

    Mix_Chunk *select = Mix_LoadWAV("../sound/effects/select.wav");

    Mix_Chunk *click = Mix_LoadWAV("../sound/effects/clic.wav");

    button btn[4];

    btn[0] = inGameMenu_createButton(&*renderer, "../fonts/Gonsterrat.ttf",
                                     sf(40), "Resume",
                                     sf(30), sf(40), 1);
    btn[1] = inGameMenu_createButton(&*renderer, "../fonts/Gonsterrat.ttf",
                                     sf(40), "Settings",
                                     sf(30), sf(110), 2);
    btn[2] = inGameMenu_createButton(&*renderer, "../fonts/Gonsterrat.ttf",
                                     sf(40), "Return to menu",
                                     sf(30), sf(180), 3);
    btn[3] = inGameMenu_createButton(&*renderer, "../fonts/Gonsterrat.ttf",
                                     sf(40), "Return to desktop",
                                     sf(30), sf(250), 4);

    scrollBar musicBar = createScrollBarRGBA(&*renderer, "Musique",
                         getUserSettings().musicVolume, 1,
                         sf(200), sf(10), sf(45), sf(200),
                         sf(8), 93, 173, 226, 0);

    scrollBar effectsBar = createScrollBarRGBA(&*renderer, "Effets",
                           getUserSettings().effectsVolume,
                           2, sf(200), sf(10), sf(45), sf(250),
                           sf(8), 93, 173, 226, 0);

    SDL_Rect menuRect = {0, 0, 0, sf(720)};

    SDL_Point mousePosition;

    SDL_RenderClear(*renderer);

    while (!canQuit)
    {
        SDL_Delay(2);

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *subLocalContinue = 0;
                *mainContinue = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                for (int i = 0; i < 4; ++i)
                {
                    buttonGetState(&*renderer, &btn[i], &select,
                                   &isChange, mousePosition);
                }

                msgBoxGetState(mousePosition, &isChange, &msg);

                break;

            case SDL_MOUSEBUTTONDOWN:

                for (int i = 0; i < 4; ++i)
                {
                    buttonOnClick(btn[i].clip, mousePosition, &ret, btn[i].id, &click);
                }

                msgBoxOnClick(&msg, mousePosition, click);

                switch (ret)
                {
                case 1:

                    LocalContinue = 0;
                    break;

                case 2:

                    if (settingsOpened == 1)
                    {
                        settingsFadeIn = 0;
                        settingsFadeOut = 1;
                    }
                    else {

                        settingsFadeIn = 1;
                        settingsFadeOut = 0;
                    }
                    break;

                case 3:

                    LocalContinue = 0;
                    *subLocalContinue = 0;
                    *menuContinue = 0;
                    break;

                case 4:

                    LocalContinue = 0;
                    *subLocalContinue = 0;
                    *mainContinue = 0;
                    *menuContinue = 0;
                    break;

                }

            case SDL_KEYDOWN:

                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    LocalContinue = 0;

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                    SDL_SetCursor(*cursor);
                }

            }

            if (settingsOpened == 1)
            {
                scrollBarGetState(&*renderer, &musicBar, event, mousePosition, &isChange,
                                  &*cursor);
                scrollBarGetState(&*renderer, &effectsBar, event, mousePosition, &isChange,
                                  &*cursor);
            }
        }

        msgBoxAnimation(&msg, &prev, SDL_GetTicks(), &isChange);

        scrollBarAnimation(&musicBar, SDL_GetTicks(), &isChange);
        scrollBarAnimation(&effectsBar, SDL_GetTicks(), &isChange);

        if (LocalContinue == 0) {

            actual = SDL_GetTicks();

            if (effectsBar.color.a > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    effectsBar.color.a = effectsBar.color.a - 16;

                    if (effectsBar.color.a <= 0 + 16)
                    {
                        effectsBar.color.a = 0;
                    }

                    isChange = 1;
                }
            }
            else if (musicBar.color.a > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    musicBar.color.a = musicBar.color.a - 16;

                    if (musicBar.color.a <= 0 + 16)
                    {
                        musicBar.color.a = 0;
                    }

                    isChange = 1;
                }
            }
            else if (btn[2].clip.y > sf(180))
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    btn[2].clip.y = btn[2].clip.y - 16;
                    btn[2].label.clip.y = btn[2].label.clip.y - 16;
                    btn[3].clip.y = btn[3].clip.y - 16;
                    btn[3].label.clip.y = btn[3].label.clip.y - 16;

                    if (btn[2].clip.y <= sf(180))
                    {
                        settingsFadeOut = 0;
                    }

                    isChange = 1;
                }
            }
            else if (btn[0].alpha > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    if (btn[3].alpha > 0)
                    {
                        btn[3].alpha = btn[3].alpha - 16;

                        if (btn[3].alpha <= 0)
                        {
                            btn[3].alpha = 0;
                        }
                    }

                    if (btn[3].alpha <= 128)
                    {
                        if (btn[2].alpha > 0)
                        {
                            btn[2].alpha = btn[2].alpha - 16;

                            if (btn[2].alpha <= 0)
                            {
                                btn[2].alpha = 0;
                            }
                        }

                        if (btn[2].alpha <= 128)
                        {
                            if (btn[1].alpha > 0)
                            {
                                btn[1].alpha = btn[1].alpha - 16;

                                if (btn[1].alpha <= 0)
                                {
                                    btn[1].alpha = 0;
                                }
                            }

                            if (btn[1].alpha <= 128)
                            {
                                if (btn[0].alpha > 0)
                                {
                                    btn[0].alpha = btn[0].alpha - 16;

                                    if (btn[0].alpha <= 0)
                                    {
                                        btn[0].alpha = 0;
                                    }
                                }
                            }
                        }
                    }

                    isChange = 1;
                }
            }
            else if (menuRect.w > 0)
            {
                if (actual > prev + 5)
                {
                    prev = SDL_GetTicks();

                    menuRect.w = menuRect.w - 32;

                    if (menuRect.w <= 0)
                    {
                        menuRect.w = 0;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            actual = SDL_GetTicks();

            if (menuRect.w < panelWidth)
            {
                if (actual > prev + 5)
                {
                    prev = SDL_GetTicks();

                    menuRect.w = menuRect.w + 32;

                    if (menuRect.w >= panelWidth)
                    {
                        menuRect.w = panelWidth;
                    }

                    isChange = 1;
                }
            }
            else if (btn[3].alpha < 255)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    if (btn[0].alpha < 255)
                    {
                        btn[0].alpha = btn[0].alpha + 16;

                        if (btn[0].alpha >= 255)
                        {
                            btn[0].alpha = 255;
                        }
                    }

                    if (btn[0].alpha >= 128)
                    {
                        if (btn[1].alpha < 255)
                        {
                            btn[1].alpha = btn[1].alpha + 16;

                            if (btn[1].alpha >= 255)
                            {
                                btn[1].alpha = 255;
                            }
                        }

                        if (btn[1].alpha >= 128)
                        {
                            if (btn[2].alpha < 255)
                            {
                                btn[2].alpha = btn[2].alpha + 16;

                                if (btn[2].alpha >= 255)
                                {
                                    btn[2].alpha = 255;
                                }
                            }

                            if (btn[2].alpha >= 128)
                            {
                                if (btn[3].alpha < 255)
                                {
                                    btn[3].alpha = btn[3].alpha + 16;

                                    if (btn[3].alpha >= 255)
                                    {
                                        btn[3].alpha = 255;
                                        localStart = 0;

                                    }
                                }
                            }
                        }
                    }

                    isChange = 1;
                }
            }
        }

        if (settingsFadeIn == 1)
        {
            actual = SDL_GetTicks();

            if (btn[2].clip.y < sf(300))
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    btn[2].clip.y = btn[2].clip.y + 16;
                    btn[2].label.clip.y = btn[2].label.clip.y + 16;
                    btn[3].clip.y = btn[3].clip.y + 16;
                    btn[3].label.clip.y = btn[3].label.clip.y + 16;

                    isChange = 1;
                }
            }
            else if (musicBar.color.a < 255)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    musicBar.color.a = musicBar.color.a + 16;

                    if (musicBar.color.a >= 255 - 16)
                    {
                        musicBar.color.a = 255;
                    }

                    isChange = 1;
                }
            }
            else if (effectsBar.color.a < 255)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    effectsBar.color.a = effectsBar.color.a + 16;

                    if (effectsBar.color.a >= 255 - 16)
                    {
                        effectsBar.color.a = 255;
                        settingsFadeIn = 0;
                        settingsOpened = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (settingsFadeOut == 1) {

            actual = SDL_GetTicks();

            if (effectsBar.color.a > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    effectsBar.color.a = effectsBar.color.a - 16;

                    if (effectsBar.color.a <= 0 + 16)
                    {
                        effectsBar.color.a = 0;
                    }

                    isChange = 1;
                }
            }
            else if (musicBar.color.a > 0)
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    musicBar.color.a = musicBar.color.a - 16;

                    if (musicBar.color.a <= 0 + 16)
                    {
                        musicBar.color.a = 0;
                    }

                    isChange = 1;
                }
            }
            else if (btn[2].clip.y > sf(180))
            {
                if (actual > prev + 10)
                {
                    prev = SDL_GetTicks();

                    btn[2].clip.y = btn[2].clip.y - 16;
                    btn[2].label.clip.y = btn[2].label.clip.y - 16;
                    btn[3].clip.y = btn[3].clip.y - 16;
                    btn[3].label.clip.y = btn[3].label.clip.y - 16;

                    if (btn[2].clip.y <= sf(180))
                    {
                        settingsFadeOut = 0;
                        settingsOpened = 0;
                    }

                    isChange = 1;
                }
            }
        }
        if (isChange == 1)
        {
            inGameMenuRefresh(&*renderer, sshot, menuRect, btn[0], btn[1],
                              btn[2], btn[3], &musicBar, &effectsBar, msg);

            isChange = 0;
            ret = 0;
        }
    }

    userSettings tempSettings;

    tempSettings = getUserSettings();

    tempSettings.musicVolume = musicBar.value;
    tempSettings.effectsVolume = effectsBar.value;

    setUserSettings(tempSettings);

    for (int i = 0; i < 4; ++i)
    {
        SDL_DestroyTexture(btn[i].label.texture);
    }

    Mix_FreeChunk(click);
}

void playVideoRefresh(SDL_Renderer **renderer, SDL_Texture **frame, int alpha, text lines[], int textAlpha[], int nbLines) {

    SDL_RenderCopy(*renderer, *frame, NULL, NULL);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(*renderer, NULL);

    for (int i = 0; i < nbLines; ++i)
    {
        SDL_SetTextureAlphaMod(lines[i].texture, textAlpha[i]);
        SDL_RenderCopy(*renderer, lines[i].texture, NULL, &lines[i].clip);
    }

    SDL_RenderPresent(*renderer);
}

void playVideoDisplay(SDL_Renderer **renderer, SDL_Cursor **cursor,
                      int *mainContinue, int *menuContinue, char location[], char tempString[],
                      int nbFrames, int fps) {

    int nbLines = 0;

    text *lines;
    int *textPrev;
    int *textAlpha;

    int i = 0;

    if (strcmp(tempString, "") != 0)
    {
        char textString[50];

        char *token[2];

        strcpy(textString, tempString);
        token[0] = strtok(textString, "/");

        while (token[0] != NULL) {

            token[0] = strtok(NULL, "/");

            nbLines++;
        }

        strcpy(textString, tempString);
        token[1] = strtok(textString, "/");

        lines = malloc(nbLines * (sizeof(text)));
        textAlpha = malloc(nbLines * (sizeof(int)));
        textPrev = malloc(nbLines * (sizeof(int)));

        int offset = sf(50);
        int textSize = sf(80);
        int textPlcmt = (sf(720) / 2) - sf(160);

        while (token[1] != NULL) {

            lines[i] = createTextTexture(&*renderer, token[1],
                                         FONT_STANDARD, textSize,
                                         sf(1280) / 2,
                                         textPlcmt + ((textSize + offset) * i) ,
                                         HCENTER | VCENTER);
            textPrev[i] = 0;
            textAlpha[i] = 0;

            i++;

            token[1] = strtok(NULL, "/");
        }
    }

    int LocalContinue = 1;
    int canQuit = 0;
    int localStart = 1;
    int prev = 0;

    char *temp = malloc(30 * sizeof(char));

    SDL_Event event;

    SDL_Texture *frame = NULL;

    int startTime = 0;
    int endTime = 0;
    int delta = 0;
    int timePerFrame = ((1 / (double)fps) * 1000);
    int stat = 128;
    int alpha = 255;
    int langChanged = 0;

    i = 1;

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    while (!canQuit)
    {
        SDL_Delay(2);

        if (!startTime) {
            startTime = SDL_GetTicks();
        } else {
            delta = endTime - startTime;
        }

        if (delta < timePerFrame) {
            SDL_Delay(timePerFrame - delta);
        }

        if (delta > timePerFrame) {
            fps = 1000 / delta;
        }

        startTime = endTime;
        endTime = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;
                *menuContinue = 0;

                break;

            case SDL_KEYDOWN:

                switch (event.key.keysym.sym ) {

                case SDLK_ESCAPE:

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

                    SDL_SetCursor(*cursor);

                    SDL_Rect windowClip = {0, 0, sf(1280), sf(720)};

                    SDL_Texture *sshot = takeScreenShot(&*renderer, windowClip);

                    inGameMenu(&*renderer, &LocalContinue, &*mainContinue,
                               sshot, &*cursor, &*menuContinue, &langChanged);

                    break;
                }
            }
        }

        if (i >= nbFrames)
        {
            LocalContinue = 0;
        }

        if (LocalContinue == 1 )
        {
            i++;

            if (i <= nbFrames)
            {
                sprintf (temp, "%s/%d.jpg", location, i);

                if (frame != NULL )
                {
                    SDL_DestroyTexture(frame);
                }

                frame = loadTexture(&*renderer, temp);
            }
        }

        if (LocalContinue == 0) {

            if (textAlpha[0] > 0)
            {
                if (SDL_GetTicks() > textPrev[0] + 10)
                {
                    textPrev[0] = SDL_GetTicks();

                    for (int i = 0; i < nbLines; ++i)
                    {
                        textAlpha[i] = textAlpha[i] - 16;

                        if (textAlpha[i] <= 0)
                        {
                            textAlpha[i] = 0;
                        }
                    }
                }
            }
            if (alpha < 255)
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }
                }
            }
        }
        else if (localStart == 1) {

            if (alpha > stat)
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= stat)
                    {
                        alpha = stat;
                    }
                }
            }
            if (textAlpha[nbLines - 1] < 255)
            {
                if (textAlpha[0] < 255)
                {
                    if (SDL_GetTicks() > textPrev[0] + 10)
                    {
                        textPrev[0] = SDL_GetTicks();

                        textAlpha[0] = textAlpha[0] + 16;

                        if (textAlpha[0] >= 255)
                        {
                            textAlpha[0] = 255;
                        }
                    }
                }

                for (int i = 1; i < nbLines; ++i)
                {
                    if (textAlpha[i - 1] == 255)
                    {
                        if (textAlpha[i] < 255)
                        {
                            if (SDL_GetTicks() > textPrev[i] + 10)
                            {
                                textPrev[i] = SDL_GetTicks();

                                textAlpha[i] = textAlpha[i] + 16;

                                if (textAlpha[i] >= 255)
                                {
                                    textAlpha[i] = 255;
                                }
                            }
                        }
                    }
                }

                if (textAlpha[nbLines - 1] == 255)
                {
                    localStart = 0;
                }
            }
        }

        playVideoRefresh(&*renderer, &frame, alpha, lines, textAlpha, nbLines);
    }

    for (int i = 0; i < nbLines; ++i)
    {
        SDL_DestroyTexture(lines[i].texture);
    }

    free(temp);
    free(lines);
    free(textAlpha);
    free(textPrev);

    SDL_DestroyTexture(frame);
}

void playRes(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue,
             int *menuContinue, int isHit) {

    video videos[7];
    int rand;
    int rand2;

    char msg[20];

    char msg2[20];

    strcpy(videos[0].location, "../images/animations/anim17" );
    videos[0].nbFrames = 42;
    videos[0].fps = 18;

    strcpy(videos[1].location, "../images/animations/anim18" );
    videos[1].nbFrames = 31;
    videos[1].fps = 18;

    strcpy(videos[2].location, "../images/animations/anim21" );
    videos[2].nbFrames = 103;
    videos[2].fps = 30;

    rand = getRandoms(0, 2);

    switch (isHit) {

    case 0:

        strcpy(msg, gs("Missed !") );

        break;

    case 1:

        strcpy(msg, gs("Hit !") );

        break;

    case 2:

        strcpy(videos[3].location, "../images/animations/anim3" );
        videos[3].nbFrames = 72;
        videos[3].fps = 18;

        strcpy(videos[4].location, "../images/animations/anim6" );
        videos[4].nbFrames = 61;
        videos[4].fps = 18;

        strcpy(videos[5].location, "../images/animations/anim9" );
        videos[5].nbFrames = 65;
        videos[5].fps = 18;

        strcpy(videos[6].location, "../images/animations/anim20" );
        videos[6].nbFrames = 124;
        videos[6].fps = 30;

        strcpy(msg, gs("Hit !") );

        strcpy(msg2, gs("Sunk !") );

        rand2 = getRandoms(3, 6);

        break;
    }

    playVideoDisplay(&*renderer, &*cursor, &*mainContinue, &*menuContinue,
                     videos[rand].location, msg, videos[rand].nbFrames, videos[rand].fps);

    if (isHit == 2)
    {
        playVideoDisplay(&*renderer, &*cursor, &*mainContinue, &*menuContinue,
                         videos[rand2].location, msg2, videos[rand2].nbFrames, videos[rand2].fps);
    }
}

void initDraggableItems(SDL_Renderer **renderer, draggableItem boats[]) {

    boats[0] = createDraggableItem(&*renderer, gs("Aircraft Carrier"), 1,
                                   "../images/boats/Porte_Avion.png", sf(805),
                                   sf(440), sf(333), sf(65), 5);

    boats[1] = createDraggableItem(&*renderer, gs("Submarine"), 2,
                                   "../images/boats/Sous_marin.png", sf(805),
                                   sf(320), sf(198), sf(65), 3);

    boats[2] = createDraggableItem(&*renderer, gs("Cruiser") , 3,
                                   "../images/boats/Croiseur.png", sf(805),
                                   sf(200), sf(264), sf(65), 4);

    boats[3] = createDraggableItem(&*renderer, gs("Torpedo boat"), 4,
                                   "../images/boats/Torpilleur.png", sf(1050),
                                   sf(80), sf(132), sf(65), 2);

    boats[4] = createDraggableItem(&*renderer, gs("Counter torpedo boat"), 5,
                                   "../images/boats/Contre-Torpilleur.png",
                                   sf(805), sf(80), sf(198), sf(65), 3);
}

void placementRefresh(SDL_Renderer **renderer, SDL_Texture **background,
                      SDL_Texture **grid_backgroundTexture,
                      draggableItem boats[], gridCase * plateau,
                      button * btn1, int alpha) {

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 128);

    SDL_RenderCopy(*renderer, *background, NULL, NULL);

    SDL_RenderFillRect(*renderer, NULL);

    SDL_Rect gridRect = getLargeGridRect();
    SDL_RenderCopy(*renderer, *grid_backgroundTexture, NULL, &gridRect);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
    drawLargeGrid(&*renderer);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 128);
    highlightLargeCase(&*renderer, plateau);

    buttonRender(&*renderer, &*btn1);

    rendererAllItems(&*renderer, boats);

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);
}

void plateauPlacement(SDL_Renderer **renderer, SDL_Cursor** cursor,
                      int *mainContinue, int playerGrid[], int *menuContinue,
                      boat boatsPlayer[]) {

    int LocalContinue = 1;
    int isChange = 0;
    int cursorMode = 0;
    int ret = 0;
    int canQuit = 0;
    int localStart = 1;
    int langChanged = 0;
    int alpha = 255;
    int prev = 0;

    gridCase plateau[100];

    draggableItem boats[5];

    SDL_Event event;
    SDL_Point mousePosition;

    playVideoDisplay(&*renderer, &*cursor, &*mainContinue, &*menuContinue,
                     "../images/animations/anim19", gs("Phase 1/The placement"), 91, 15);
    if (!*mainContinue || !*menuContinue)
        canQuit = 1;

    SDL_Texture *background = loadTexture(&*renderer, "../images/fond.jpg");

    SDL_Texture *grid_backgroundTexture = loadTexture(&*renderer,
                                          "../images/background_grid.png");

    initDraggableItems(&*renderer, boats);

    button btn1 = createButton(&*renderer, FONT_MILITARY , sf(50), gs("Validate"), sf(820),
                               sf(589), sf(347), sf(106));

    btn1.activated = 0;

    Mix_Chunk *select = Mix_LoadWAV("../sound/effects/select.wav");
    Mix_Chunk *click = Mix_LoadWAV("../sound/effects/clic.wav");

    initPlateau(plateau);

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    while (!canQuit)
    {
        isChange = 0;
        ret = 0;

        SDL_Delay(2);

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;
                *menuContinue = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                cursorSwitchPlacement(&*cursor, mousePosition, &cursorMode,
                                      boats);

                if (btn1.activated == 1)
                {

                    buttonGetState(&*renderer, &btn1, &select, &isChange,
                                   mousePosition);
                }

                break;

            case SDL_MOUSEBUTTONDOWN:

                if (btn1.activated == 1)
                {
                    buttonOnClick(btn1.clip, mousePosition, &ret, 1, &click);
                }

                break;

            case SDL_KEYDOWN:

                switch (event.key.keysym.sym ) {

                case SDLK_ESCAPE:

                    btn1.isSelected = 0;

                    placementRefresh(&*renderer, &background,
                                     &grid_backgroundTexture, boats, plateau,
                                     &btn1, alpha);

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

                    SDL_SetCursor(*cursor);

                    cursorMode = 0;

                    SDL_Rect windowClip = {0, 0, sf(1280), sf(720)};

                    SDL_Texture *sshot = takeScreenShot(&*renderer, windowClip);

                    inGameMenu(&*renderer, &LocalContinue, &*mainContinue,
                               sshot, &*cursor, &*menuContinue, &langChanged);

                    isChange = 1;

                    break;
                }
            }

            ItemDragGlobal(&*renderer, event, mousePosition, &isChange, boats, getUserSettings().rotateKey);

            if (boats[0].placed == 1 && boats[1].placed == 1 &&
                    boats[2].placed == 1 && boats[3].placed == 1 &&
                    boats[4].placed == 1 && btn1.activated == 0)
            {
                btn1.activated = 1;
                isChange = 1;

            }
            else if (btn1.activated == 1 && (boats[0].placed == 0 ||
                                             boats[1].placed == 0 ||
                                             boats[2].placed == 0 ||
                                             boats[3].placed == 0 ||
                                             boats[4].placed == 0))
            {
                btn1.activated = 0;
                isChange = 1;
            }

            if (ret == 1)
            {
                LocalContinue = 0;
                for (int i = 0; i < 100; ++i)
                {
                    playerGrid[i] = plateau[i].occuped;
                }
            }
        }

        if (LocalContinue == 0) {

            if (alpha < 255 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            if (alpha > 0)
            {
                if (SDL_GetTicks() > prev + 20)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= 0)
                    {
                        alpha = 0;
                        localStart = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (isChange == 1)
        {
            getGridState(plateau, boats);

            placementRefresh(&*renderer, &background, &grid_backgroundTexture,
                             boats, plateau, &btn1, alpha);
        }
    }

    setBoatState(boats, boatsPlayer);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(grid_backgroundTexture);

    SDL_DestroyTexture(btn1.label.texture);
}

void attackRefresh(SDL_Renderer **renderer, SDL_Texture **background,
                   SDL_Texture **grid_backgroundTexture, SDL_Texture **expl1,
                   gridCase * plateau, int gridIA[], int gridPlayer[],
                   int alpha, boat boats[], int alpha2, text textInfoIA_1,
                   text textInfoIA_2) {

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 128);

    SDL_RenderCopy(*renderer, *background, NULL, NULL);

    SDL_RenderFillRect(*renderer, NULL);

    SDL_Rect grid_LargeBackgroundRect = getLargeGridRect();
    SDL_RenderCopy(*renderer, *grid_backgroundTexture, NULL,
                   &grid_LargeBackgroundRect);
    drawLargeGrid(&*renderer);

    SDL_Rect grid_SmallBackgroundRect = getSmallGridRect();
    SDL_RenderCopy(*renderer, *grid_backgroundTexture, NULL,
                   &grid_SmallBackgroundRect);
    drawSmallGrid(&*renderer);

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 128);

    highlightLargeCase(&*renderer, plateau);
    highlightLargeStateCase(&*renderer, plateau, gridIA, &*expl1);

    highlightSmallStateCase(&*renderer, gridPlayer);
    renderSmallItem(&*renderer, boats);
    highlightSmallStateCase2(&*renderer, gridPlayer, &*expl1);

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, alpha2);

    if (alpha2 > 0)
    {
        SDL_SetTextureAlphaMod(textInfoIA_1.texture, alpha2 + 75);
        SDL_SetTextureAlphaMod(textInfoIA_2.texture, alpha2 + 75);
    }
    else if (alpha2 == 0)
    {
        SDL_SetTextureAlphaMod(textInfoIA_1.texture, alpha2);
        SDL_SetTextureAlphaMod(textInfoIA_2.texture, alpha2);
    }

    SDL_RenderFillRect(*renderer, &grid_SmallBackgroundRect);

    SDL_RenderCopy(*renderer, textInfoIA_1.texture, NULL, &textInfoIA_1.clip);
    SDL_RenderCopy(*renderer, textInfoIA_2.texture, NULL, &textInfoIA_2.clip);

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);
}

int plateauAttack(SDL_Renderer **renderer, SDL_Cursor** cursor,
                  int *mainContinue, int *menuContinue, int isStart,
                  int gridIA[], int gridPlayer[], boat boats[],
                  int IANbStrokes) {

    int LocalContinue = 1;
    int isChange = 0;
    int ret = -1;
    char temp[10];
    int alpha = 255;
    int alpha2 = 0;
    int canQuit = 0;
    int langChanged = 0;
    int localStart = 1;
    int prev = 0;
    int prev2 = 0;

    gridCase plateau[100];

    SDL_Event event;
    SDL_Point mousePosition;

    SDL_Texture *background = loadTexture(&*renderer, "../images/fond.jpg");

    SDL_Texture *grid_backgroundTexture = loadTexture(&*renderer,
                                          "../images/background_grid.png");

    SDL_Texture *expl1 = loadTexture(&*renderer, "../images/expln1.png");

    sprintf (temp, "%d %s", IANbStrokes, gs("times"));

    text textInfoIA_1 = createTextTexture(&*renderer, gs("The AI played :"),
                                          FONT_MILITARY, sf(40), sf(918),
                                          sf(500), HCENTER | VCENTER);

    text textInfoIA_2 = createTextTexture(&*renderer, temp, FONT_MILITARY,
                                          sf(40), sf(918), sf(550),
                                          HCENTER | VCENTER);

    initPlateau(plateau);

    if (IANbStrokes != 0)
    {
        alpha2 = 180;
    }

    if (isStart == 1)
    {
        playVideoDisplay(&*renderer, &*cursor, &*mainContinue, &*menuContinue,
                         "../images/animations/anim2", gs("Phase 2/The fight"), 169, 30);
        if (!*mainContinue || !*menuContinue)
            canQuit = 1;
    }

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    int initalTime = SDL_GetTicks();

    while (!canQuit)
    {

        SDL_Delay(2);

        while (SDL_PollEvent(&event)) {

            isChange = 0;

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;
                *menuContinue = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                getCaseHovered(plateau, mousePosition, &isChange, gridIA);

                break;

            case SDL_MOUSEBUTTONDOWN:

                ret = getCaseToReturn(plateau);

                if (ret != -1)
                {
                    LocalContinue = 0;
                }

                break;

            case SDL_KEYDOWN:

                switch (event.key.keysym.sym ) {

                case SDLK_ESCAPE:

                    attackRefresh(&*renderer, &background,
                                  &grid_backgroundTexture, &expl1, plateau,
                                  gridIA, gridPlayer, (int)alpha, boats,
                                  (int)alpha2, textInfoIA_1, textInfoIA_2);

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

                    SDL_SetCursor(*cursor);

                    SDL_Rect windowClip = {0, 0, sf(1280), sf(720)};

                    SDL_Texture *sshot = takeScreenShot(&*renderer, windowClip);

                    inGameMenu(&*renderer, &LocalContinue, &*mainContinue,
                               sshot, &*cursor, &*menuContinue, &langChanged);

                    isChange = 1;

                    break;
                }
            }
        }

        if (SDL_GetTicks() - initalTime > 2000) {

            if ( alpha2 > 0) {

                if (SDL_GetTicks() > prev2 + 10)
                {
                    alpha2 = alpha2 - 8;
                    prev2 = SDL_GetTicks();

                    if (alpha2 <= 0)
                    {
                        alpha2 = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (LocalContinue == 0) {

            if (alpha < 255 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            if (alpha > 0)
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 8;

                    if (alpha <= 0)
                    {
                        alpha = 0;
                        localStart = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (isChange == 1 )
        {
            attackRefresh(&*renderer, &background, &grid_backgroundTexture,
                          &expl1, plateau, gridIA, gridPlayer, alpha, boats,
                          alpha2, textInfoIA_1, textInfoIA_2);
        }
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(grid_backgroundTexture);

    return ret;
}

void winRefresh(SDL_Renderer **renderer, int alpha, int alpha2,
                SDL_Texture * frame, text timer, text textTimer, text text1,
                text text2, SDL_Rect rectWinner, SDL_Rect rectLooser,
                text text3, text text4, text winnerText, text looserText) {

    SDL_RenderCopy(*renderer, frame, NULL, NULL);

    setAlpha(&*renderer, alpha2);

    SDL_RenderCopy(*renderer, timer.texture, NULL, &timer.clip);
    SDL_RenderCopy(*renderer, textTimer.texture, NULL, &textTimer.clip);
    SDL_RenderCopy(*renderer, text1.texture, NULL, &text1.clip);
    SDL_RenderCopy(*renderer, text2.texture, NULL, &text2.clip);
    SDL_RenderCopy(*renderer, text3.texture, NULL, &text3.clip);
    SDL_RenderCopy(*renderer, text4.texture, NULL, &text4.clip);

    SDL_SetRenderDrawColor(*renderer, 241, 196, 15 , 150);
    SDL_RenderFillRect(*renderer, &rectWinner);
    SDL_SetRenderDrawColor(*renderer,  231, 76, 60 , 150);
    SDL_RenderFillRect(*renderer, &rectLooser);

    SDL_RenderCopy(*renderer, winnerText.texture, NULL, &winnerText.clip);
    SDL_RenderCopy(*renderer, looserText.texture, NULL, &looserText.clip);

    setAlpha(&*renderer, alpha);

    SDL_RenderPresent(*renderer);
}

char *getClientNick();



void displayWin(SDL_Renderer **renderer, SDL_Cursor** cursor,
                int *mainContinue, int *menuContinue, timer mainTimer,
                int scoreIA, int scorePlayer) {

    int isChange = 0;
    int canQuit = 0;
    int localStart = 1;
    int langChanged = 0;
    char temp[50] = {0};
    int i = 1;

    text timer = createTextTexture(&*renderer, "../fonts/digital.ttf", FONT_STANDARD, sf(40),
                                   sf(30), sf(45), LEFT | VCENTER);

    text textTimer = createTextTexture(&*renderer, gs("Total time"), FONT_STANDARD,
                                       sf(100), sf(640), sf(250),
                                       HCENTER | VCENTER);

    text text1 = createTextTexture(&*renderer, gs("You"), FONT_STANDARD,
                                   sf(100), sf(640), sf(250),
                                   HCENTER | VCENTER);

    text winnerText;
    winnerText.texture = NULL;

    text looserText;
    looserText.texture = NULL;

    SDL_SetTextureAlphaMod(textTimer.texture, 0);
    SDL_SetTextureAlphaMod(winnerText.texture, 0);
    SDL_SetTextureAlphaMod(looserText.texture, 0);

    SDL_Event event;
    SDL_Point mousePosition;

    SDL_Texture *frame = NULL;

    int alpha2 = 0;
    int timerAlpha = 255;
    int alpha = 255;
    double propValue = 0;
    int speedAdd = 0;
    int alphaText = 0;

    int time1 = SDL_GetTicks();
    int time2 = SDL_GetTicks();
    int time3 = 0;

    SDL_RenderClear(*renderer);
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);

    int ready = 0;
    int ready2 = 0;
    int prev = 0;
    int prev2 = 0;

    int LocalContinue = 1;
    SDL_Rect rectWinner = {sf(330), sf(400), sf(200), 0};
    SDL_Rect rectLooser = {sf(790), sf(400), sf(200), 0};

    int scoreWinner = 0;
    int scoreLooser = 0;
    char winner[20] = {0};
    char looser[20] = {0};
    char res[20] = {0};

    if (scoreIA < scorePlayer)
    {
        scoreWinner = scorePlayer;
        scoreLooser = scoreIA;
        strcpy(winner, getClientProfile().nick);
        strcpy(looser, gs("AI"));
        strcpy(res, gs("won"));

    }
    else {

        scoreWinner = scoreIA;
        scoreLooser = scorePlayer;
        strcpy(winner, gs("AI"));
        strcpy(looser, getClientProfile().nick);
        strcpy(res, gs("loose"));
    }

    text text2 = createTextTexture(&*renderer, res, FONT_STANDARD, sf(100),
                                   sf(640), sf(370), HCENTER | VCENTER);

    text text4 = createTextTexture(&*renderer, looser, FONT_STANDARD, sf(40),
                                   rectLooser.x + (rectLooser.w / 2), sf(550),
                                   HCENTER | VCENTER);

    SDL_SetTextureAlphaMod(text4.texture, 0);

    text text3 = createTextTexture(&*renderer, winner, FONT_STANDARD, sf(40),
                                   rectWinner.x + (rectWinner.w / 2), sf(550),
                                   HCENTER | VCENTER);

    SDL_SetTextureAlphaMod(text3.texture, 0);

    while (!canQuit)
    {
        isChange = 0;

        SDL_Delay(2);

        while (SDL_PollEvent(&event)) {

            switch (event.type)
            {
            case SDL_QUIT:

                LocalContinue = 0;
                *mainContinue = 0;
                *menuContinue = 0;

                break;

            case SDL_MOUSEMOTION:

                SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

                break;

            case SDL_MOUSEBUTTONDOWN:

                break;

            case SDL_KEYDOWN:

                switch (event.key.keysym.sym ) {

                case SDLK_ESCAPE:

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

                    SDL_SetCursor(*cursor);

                    SDL_Rect test = {0, 0, sf(1280), sf(720)};

                    SDL_Texture *sshot = takeScreenShot(&*renderer, test);

                    inGameMenu(&*renderer, &LocalContinue, &*mainContinue,
                               sshot, &*cursor, &*menuContinue, &langChanged);

                    isChange = 1;

                    break;
                }
            }
        }

        if (LocalContinue == 0) {

            if (alpha < 255 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha + 8;

                    if (alpha >= 255)
                    {
                        alpha = 255;
                        canQuit = 1;
                    }

                    isChange = 1;
                }
            }
        }
        else if (localStart == 1) {

            if (alpha > 0)
            {
                if (SDL_GetTicks() > prev + 30)
                {
                    prev = SDL_GetTicks();

                    alpha = alpha - 4;

                    if (alpha <= 0)
                    {
                        alpha = 0;
                        localStart = 0;
                    }

                    isChange = 1;
                }
            }
        }

        if (i < 105)
        {
            if (SDL_GetTicks() - time1 > 50)
            {
                time1 = SDL_GetTicks();

                i++;
                sprintf (temp, "%s/%d.jpg", "../images/animations/anim11", i);

                if (frame != NULL)
                {
                    SDL_DestroyTexture(frame);
                }

                frame = loadTexture(&*renderer, temp);
                isChange = 1;
            }
        }
        if (i == 105 )
        {
            if (alpha2 < 180 )
            {
                if (SDL_GetTicks() > prev + 20)
                {
                    prev = SDL_GetTicks();

                    alpha2 = alpha2 + 4;

                    if (alpha2 >= 180)
                    {
                        alpha2 = 180;
                    }

                    isChange = 1;
                }
            }

            if (timerAlpha > 0 )
            {
                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    timerAlpha = timerAlpha - 8;

                    if (timerAlpha <= 0)
                    {
                        timerAlpha = 0;
                    }

                    isChange = 1;

                    SDL_SetTextureAlphaMod(text1.texture, timerAlpha);
                    SDL_SetTextureAlphaMod(text2.texture, timerAlpha);
                }
            }

            if (timerAlpha == 0 && alpha2 >= 180)
            {
                mainTimer.running = 0;
                i++;
            }
        }

        if (SDL_GetTicks() - time2 > 50)
        {
            if (mainTimer.running == 1)
            {
                getTimerTime(&mainTimer);

                time2 = SDL_GetTicks();

                sprintf (temp, "%d : %02d : %02d : %03d", mainTimer.time.hours,
                         mainTimer.time.min, mainTimer.time.sec, mainTimer.time.mili);

                SDL_DestroyTexture(timer.texture);

                updateTextTexture(&*renderer, &timer,
                                  temp, "../fonts/digital.ttf", sf(40));

                SDL_SetTextureAlphaMod(timer.texture, timerAlpha);

            }
            else {

                if (mainTimer.endTime == 0)
                {
                    mainTimer.endTime = SDL_GetTicks() - mainTimer.initialTime;
                }

                if (ready == 0)
                {
                    if (timerAlpha < 255)
                    {
                        if (SDL_GetTicks() > prev + 20)
                        {
                            prev = SDL_GetTicks();

                            timerAlpha = timerAlpha + 4;

                            if (timerAlpha >= 255)
                            {
                                timerAlpha = 255;
                                ready = 1;
                                time3 = SDL_GetTicks();
                            }
                        }

                        SDL_DestroyTexture(timer.texture);

                        timer = createTextTexture(&*renderer, temp,
                                                  "../fonts/digital.ttf",
                                                  sf(80), sf(640), sf(400),
                                                  HCENTER | VCENTER);

                        SDL_SetTextureAlphaMod(timer.texture, timerAlpha);
                        SDL_SetTextureAlphaMod(textTimer.texture, timerAlpha);

                        isChange = 1;
                    }
                }
            }
        }

        if (ready == 1)
        {
            if (SDL_GetTicks() - time3 > 3000)
            {
                if (timerAlpha > 0 )
                {
                    if (SDL_GetTicks() > prev + 20)
                    {
                        prev = SDL_GetTicks();

                        timerAlpha = timerAlpha - 8;

                        if (timerAlpha <= 0)
                        {
                            timerAlpha = 0;
                            ready2 = 1;
                            text2 = createTextTexture(&*renderer, "Score",
                                                      FONT_STANDARD, sf(100),
                                                      sf(640), sf(200),
                                                      HCENTER | VCENTER);
                        }

                        SDL_SetTextureAlphaMod(timer.texture, timerAlpha);
                        SDL_SetTextureAlphaMod(textTimer.texture, timerAlpha);

                        isChange = 1;
                    }
                }
            }
        }

        if (ready2 == 1)
        {
            if (propValue < 100)
            {
                if (SDL_GetTicks() > prev2 + 20)
                {
                    prev2 = SDL_GetTicks();

                    alphaText = alphaText + 8;

                    if (alphaText >= 255)
                    {
                        alphaText = 255;
                    }

                    SDL_SetTextureAlphaMod(text3.texture, (int)alphaText);
                    SDL_SetTextureAlphaMod(text4.texture, (int)alphaText);
                    SDL_SetTextureAlphaMod(text2.texture, (int)alphaText);

                    isChange = 1;
                }

                if (SDL_GetTicks() > prev + 10)
                {
                    prev = SDL_GetTicks();

                    propValue = propValue + 1;

                    prev = prev + speedAdd;

                    speedAdd++;

                    if (propValue >= 99)
                    {
                        propValue = 100;

                        ready2 = 0;
                    }

                    rectWinner.h = (propValue / 100) *
                                   (int)(((float)scoreWinner / 1700) * sf(200));

                    rectLooser.h = (propValue / 100) *
                                   (int)(((float)scoreLooser / 1700) * sf(200));

                    rectWinner.y = sf(500) - rectWinner.h;
                    rectLooser.y = sf(500) - rectLooser.h;

                    sprintf(temp, "%d", (int)((propValue / 100)*scoreWinner));

                    if (winnerText.texture != NULL)
                    {
                        SDL_DestroyTexture(winnerText.texture);
                    }

                    winnerText = createTextTexture(&*renderer, temp,
                                                   FONT_STANDARD,
                                                   (propValue / 100) *
                                                   (int)(((float)scoreWinner /
                                                          1700) * sf(50)),
                                                   rectWinner.x +
                                                   (rectWinner.w / 2), sf(500) -
                                                   (rectWinner.h / 2),
                                                   HCENTER | VCENTER);

                    sprintf(temp, "%d", (int)((propValue / 100)*scoreLooser));

                    if (looserText.texture != NULL)
                    {
                        SDL_DestroyTexture(looserText.texture);
                    }

                    looserText = createTextTexture(&*renderer, temp,
                                                   FONT_STANDARD,
                                                   (propValue / 100) *
                                                   (int)(((float)scoreLooser /
                                                          1700) * sf(50)),
                                                   rectLooser.x + (rectLooser.w
                                                           / 2), sf(500) -
                                                   (rectLooser.h / 2),
                                                   HCENTER | VCENTER);
                    isChange = 1;
                }
            }
        }
        else if (propValue == 100)
        {
            LocalContinue = 0;
        }
        if (isChange == 1)
        {
            winRefresh(&*renderer, (int)alpha, (int)alpha2, frame, timer,
                       textTimer, text1, text2, rectWinner, rectLooser, text3,
                       text4, winnerText, looserText);
        }
    }

    if (!getThreadStatut())
    {
        netPacket packet;

        packet.infos = ((scorePlayer / 10) << 8) | (scoreIA / 10);

        packet.client.bestTime = mainTimer.endTime;

        packet = sandPackedRequest("updateProfile", &packet);
    }
}
