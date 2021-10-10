#ifndef IHM_H
#define IHM_H

#include "../headers/SDL2/SDL.h"
#include "../headers/SDL2/SDL_mixer.h"
#include "../headers/SDL2/SDL_image.h"
#include "../headers/SDL2/SDL_net.h"
#include "../headers/SDL2/SDL2_gfxPrimitives.h"
#include "entreprise-header.h"

typedef struct {
	double r;       
	double g;       
	double b;       
} RGBColor;

typedef struct {
	double h;       
	double s;       
	double v;    
} HSVColor;

typedef struct
{
	SDL_Texture* texture;
	SDL_Rect clip;
	int plcmtMode;

} text;

typedef struct
{
	SDL_Rect clip;
	text label;
	SDL_Color color;
	int isSelected;
	int activated;
	int id;
	int alpha;

} button;

typedef struct
{
	SDL_Rect clip;
	int activated;
	text text;
	char displayName[20];

} draggableItem_placeHolder;

typedef struct
{
	SDL_Rect clip;
	SDL_Rect clipRender;
	SDL_Texture *texture;
	SDL_Point relativePos;
	SDL_Point positionReference;
	SDL_Point firstPoint;
	SDL_Rect actualCase;
	draggableItem_placeHolder placeHolder;
	int placed;
	int placable;
	int flipped;
	int dragging;
	int nbCase;
	int mouseOver;
	int itemId;

} draggableItem;

typedef struct
{
	SDL_Rect clipRender;
	SDL_Rect clip;
	SDL_Color color;
	int circleRadius;
	int circleRadiusRender;
	int circleCenterX;
	int circleCenterY;
	SDL_Texture *circleTexture;
	int changedValue;
	int drag;
	int mouseOver;
	int fadeOut;
	int fadeIn;
	int value;
	int id;
	int prev;
	text topText;
	text bottomText;

} scrollBar;

typedef struct
{
	SDL_Rect clip;
	int occuped;
	int highlighted;

} gridCase;

typedef struct
{
	SDL_Texture *texture;
	SDL_Rect textureClip;
	text text;
	int itemId;
	int hovered;
	SDL_Rect clip;

} selectBarItem;

typedef struct
{
	SDL_Rect clip;
	SDL_Color color;
	SDL_Texture *arrowTexture;
	SDL_Rect arrowClip;
	int nbItems;
	selectBarItem item[6];
	int changedValue;
	int deployAnim;
	int retractAnim;
	int deploy;
	int hovered;
	int prev;
	int animProp;

} selectBar;

typedef struct
{
	char location[30];
	int nbFrames;
	int fps;

} video;

typedef struct
{
	int id;
	text label;
	int isSet;
	SDL_Color color;

} error;

typedef struct
{
	int musicVolume;
	int effectsVolume;
	int selectedLanguage;
	int displayMode;
	int displayRes;
	int rotateKey;

} userSettings;

typedef struct
{
	SDL_Rect clip;
	SDL_Color color;
	int propValue;
	int fadeIn;
	int fadeOut;
	int activated;
	int hovered;
	SDL_Point trigonPoint[3];
	text title;
	text line[10];
	int nbLines;
	text arrowText;
	int textAlpha;

} msgBox;

typedef struct
{
	SDL_Color color;
	SDL_Rect clip;
	text name;
	int isHovered;
	int colorProp;
	int alphaProp;
	int textAlpha;
	int colorAnimIn;
	int colorAnimOut;

} mainBarTab;

typedef struct
{
	button closeButton;
	SDL_Rect barRect;
	int actualTab;
	int openAnimation;
	int barAlpha;
	int elementsAlpha;
	int animStart;
	int prev;
	int prev2;
	int tabChanged;
	int firstLoad;
	text barName;
	mainBarTab tab[5];

} mainBar;

typedef struct
{
	SDL_Rect clip;
	SDL_Color color;
	text name;
	text textValue;
	button prevButton;
	button nextButton;
	int changedValue;
	int minValue;
	int maxValue;
	int value;

} graduateBar;

typedef struct
{
	SDL_Rect clip;
	SDL_Rect startClip;
	SDL_Rect finalClip;
	SDL_Color color;
	int openAnimation;
	int openned;
	int prev;
	int animProp;
	int alphaProp;
	text name;
	button playButton;
	selectBar difficultyBar;
	scrollBar smartBar;
	graduateBar strokeBar;

} form1;

typedef struct
{
	SDL_Rect clip;
	SDL_Color color;
	text label[4];
	SDL_Texture *logo;

} logoRect;

typedef struct
{
	text label[6];

} scoreBoardItem;

typedef struct
{
	SDL_Rect clip;
	int alpha;
	int hovered;

} scoreBoardLine;

typedef struct
{
	text label;
	button sortButton;
	SDL_Rect clip;
	scoreBoardLine lines[11];

} scoreBoardCols;

typedef struct
{
	text name;
	SDL_Texture *arrowTexture;
	scoreBoardItem item[11];
	scoreBoardCols cols[6];
	int nbCols;
	int nbLines;
	SDL_Rect clip;
	SDL_Color color;
	int sortID;
	int isReverse;
	int animIn;
	int animOut;
	int animFinished;
	int animActualCol;

} scoreBoard;

typedef struct
{
	int id;
    char nick[30];
    char country[40];
    SDL_Color color;
    int imgID;
    int bestTime;
    int averageTime;
    int firstLog;
    int defeats;
    int wins;

} profile;

typedef struct
{
	int isHovered;
	int isFocus;
	SDL_Color color;
	SDL_Rect clip;
	text key;

} keySelector;

typedef struct
{

	button applyButton;
	int applyRet;
	int settingsChanged;
	SDL_Rect clip;
	SDL_Rect categoryClip[4];
	text categoryLabel[4];
	SDL_Color color;
	scrollBar musicBar;
	scrollBar effectsBar;
	selectBar langBar;
	selectBar displayBar;
	selectBar resBar;
	keySelector rotateKeySelector;
	button profileButton[2];
	int needReload;

} panel;

typedef struct
{
	int selectionID;
	int colorChanged;
	SDL_Rect sliderRect;
	SDL_Rect hueRect;
	SDL_Rect gradiantRect;
	HSVColor currentColor;
	SDL_Texture *hueTexture;
	SDL_Texture *gradiantTexture;

} colorPicker;

typedef struct
{
	int isFocused;
	int isHovered;
	SDL_Color color;
	char text[100];
	text toPrint;
	int textChanged;
	int printCharacters;
	int maxCharacters;
	SDL_Rect cursor;
	SDL_Rect clip;
	text maxC;

} textInput;

typedef struct
{
	SDL_Rect clip;
	SDL_Color color;
	int animIn;
	int animProp;
	text label;
	button nextButton;

} form2_category;

typedef struct
{
	SDL_Rect clip;
	colorPicker picker;
	textInput nickInput;
	textInput countryInput;
	SDL_Texture *circleTexture;
	SDL_Rect outlineSmall;
	form2_category category[3];
	SDL_Texture *pp[9];
	SDL_Rect ppRect[9];
	int ppIsHovered[9];
	SDL_Rect overview;
	SDL_Rect overviewPpRect;
	SDL_Color color;
	int animProp;
	int categoryOpenned;
	text userName;
	text country;
	text level;
	text infoLabel[2];
	SDL_Texture *selectedFlagTexture;
	SDL_Rect selectedFlagRect;
	SDL_Texture *flagTexture[6];
	SDL_Rect flagRect[6];
	button flagButton[6];
	text flagInfo;
	SDL_Color selectedColor;
	char nick[30];
	char countryName[40];
	int ppID;

} form2;

typedef struct
{
    char instruction[20];
    int ret;
    profile client;
    profile bestClients[11];
    int received;
    int infos;
    int canSend;

} netPacket;

#define FONT_STANDARD "../fonts/standard.ttf"
#define FONT_MILITARY "../fonts/gunplay.ttf"
#define FONT_LOGO "../fonts/logo.ttf"

#define LEFT 0x1
#define HCENTER 0x2
#define RIGHT 0x4

#define TOP 0x10
#define VCENTER 0x20
#define BOTTOM 0x40

#define INFT 2147483647

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define CLAMP(INF,SUP,N) ((N)>(SUP)?(SUP):((N<INF)?(INF):(N)))

void getGlobalSizeFactor();

int sf(int value);

void initSettings();

void saveSettings();

void SDL_start(SDL_Renderer **renderer, Mix_Music *music);

void SDL_stop(SDL_Renderer **renderer, Mix_Music **music);

void mainMenuDisplayRefresh(SDL_Renderer **renderer, button newGameButton,
                            SDL_Texture **background, int alpha,
                            mainBar menuBar, form1 newGameForm, logoRect logo, scoreBoard board, panel settingsPanel);

int mainMenuDisplay(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int createNewUserAccount);

selectBar createSelectBarRGBA(SDL_Renderer **renderer, int x, int y, int h, int w, int nbItems, selectBarItem *items, int r, int g, int b, int a);

void inGameMenu_text_Init(SDL_Renderer **renderer, text *textMusic, text *textEffects, text *textSettings, text *textVolume, text *textLang);

void inGameMenu_button_Init(SDL_Renderer **renderer, button *btn1, button *btn2, button *btn3);

void inGameMenuRefresh(SDL_Renderer **renderer, SDL_Texture *sshot, SDL_Rect menuRect, button btn1, button btn2, button btn3, button btn4, scrollBar *musicBar, scrollBar *effectsBar, msgBox msg);

void inGameMenu(SDL_Renderer **renderer, int *subLocalContinue, int *mainContinue, SDL_Texture *sshot, SDL_Cursor **cursor, int *menuContinue, int *langChanged);

void playVideoDisplay(SDL_Renderer **renderer, SDL_Cursor **cursor,int *mainContinue,int *menuContinue,
                     char location[], char tempText[], int frames, int fps);

void playRes(SDL_Renderer **renderer, SDL_Cursor **cursor, int *mainContinue, int *menuContinue, int isHit);

void initDraggableItems(SDL_Renderer **renderer, draggableItem boats[]);

void placementRefresh(SDL_Renderer **renderer, SDL_Texture **background, SDL_Texture **grid_backgroundTexture, draggableItem boats[], gridCase *plateau, button *btn1, int alpha); void plateauPlacement(SDL_Renderer **renderer, SDL_Cursor** cursor, int *mainContinue, int playerGrid[], int *menuContinue, boat boatsPlayer[]);

void attackRefresh(SDL_Renderer **renderer, SDL_Texture **background, SDL_Texture **grid_backgroundTexture, SDL_Texture **expl1, gridCase *plateau, int gridIA[], int gridPlayer[], int alpha, boat boats[], int alpha2, text textInfoIA_1, text textInfoIA_2);

int plateauAttack(SDL_Renderer **renderer, SDL_Cursor** cursor, int *mainContinue, int *menuContinue, int isStart, int gridIA[], int gridPlayer[], boat boats[], int IANbStrokes);

void winRefresh(SDL_Renderer **renderer, int alpha, int alpha2, SDL_Texture *frame, text timer, text textTimer, text text1, text text2, SDL_Rect rectWinner, SDL_Rect rectLooser, text text3, text text4, text winnerText, text looserText);

void playVideoPhase_text_Init(SDL_Renderer **renderer, text *text1, text *text2, int phaseNumber);

void setMusicVolume(int volume);

void setChannelVolume(int volume, int channel);

timer startTimer();

void getTimerTime(timer *element);

void printTime(timer element);

void renderSelectBar(SDL_Renderer **renderer, selectBar bar);

void setSelectBarValue(selectBar *bar, int selected);

void selectBarGetState(selectBar *bar, SDL_Event event, SDL_Point mousePosition, int *isChange);

text createTextTexture(SDL_Renderer **renderer, const char textToPrint[], char fontLocation[], int fontSize, int centerX, int centerY, int plcmtMode);

void setAlpha(SDL_Renderer **renderer, int alpha);

void audioEffect(Mix_Chunk *sample, int volume);

int ifPointInSquare(SDL_Rect clip, SDL_Point pos);

button createButton(SDL_Renderer **renderer, char buttonFontLocation[], int fontSize, const char buttonText[], int x, int y, int w, int h);

void buttonGetState(SDL_Renderer **renderer, button *btn, Mix_Chunk **select, int *isChange, SDL_Point mousePosition);

void buttonOnClick(SDL_Rect clip, SDL_Point mousePosition , int *ret, int retAsk, Mix_Chunk **click);

void drawThickRect(SDL_Renderer **renderer, SDL_Rect clip, int thickness, int r, int g , int b, int a);

void buttonRender(SDL_Renderer **renderer, button *btn);

SDL_Texture *loadTexture(SDL_Renderer **renderer, char imgLocation[]);

SDL_Rect getLargeCaseRect(int caseX, int caseY);

SDL_Rect getSmallCaseRect(int caseX, int caseY);

void ItemDrag(SDL_Renderer **renderer, draggableItem *item, SDL_Event event, SDL_Point mousePosition, int *isChange, int key);

draggableItem createDraggableItem(SDL_Renderer **renderer, char text[], int itemId, char img[], int x, int y, int w, int h, int nbCase);

void highlightLargeCase(SDL_Renderer **renderer, gridCase *plateau);

void highlightSmallStateCase(SDL_Renderer **renderer, int gridPlayer[]);

void highlightSmallStateCase2(SDL_Renderer **renderer, int gridPlayer[], SDL_Texture **expl);

void highlightLargeStateCase(SDL_Renderer **renderer, gridCase plateau[], int grid[], SDL_Texture **expl);

void getCaseState(gridCase *plateau, draggableItem *item, int state);

void clearCaseToHighLight(gridCase *plateau);

void initPlateau(gridCase *plateau);

void renderLargeItem(SDL_Renderer **renderer, draggableItem *item, int state);

void rendererAllItems(SDL_Renderer **renderer, draggableItem boats[]);

SDL_Rect getLargeGridRect();

SDL_Rect getSmallGridRect();

void drawLargeGrid(SDL_Renderer **renderer);

void drawSmallGrid(SDL_Renderer **renderer);

void renderSmallItem(SDL_Renderer **renderer, boat boats[]);

void ifEnteredInSquare(SDL_Rect clip, int *mouseOver, SDL_Point mousePosition);

void cursorSwitchItem(SDL_Cursor** cursor, SDL_Point mousePosition, draggableItem *item, int  *cursorMode);

void getGridState(gridCase *plateau, draggableItem boats[]);

void ItemDragGlobal(SDL_Renderer **renderer, SDL_Event event, SDL_Point mousePosition, int *isChange, draggableItem boats[], int key);

void cursorSwitchPlacement(SDL_Cursor **cursor, SDL_Point mousePosition, int *cursorMode, draggableItem boats[]);

scrollBar createScrollBarRGBA(SDL_Renderer **renderer, char *name, int value, int barId, int barLenght, int barHeight, int barX, int barY, int circleRadius, int r, int g, int b, int a);

SDL_Texture *takeScreenShot(SDL_Renderer **renderer, SDL_Rect clip);

void scrollBarGetState(SDL_Renderer **renderer, scrollBar *bar, SDL_Event event, SDL_Point mousePosition, int *isChange, SDL_Cursor **cursor);

void scrollBarRender(SDL_Renderer **renderer, scrollBar bar);

void renderRect(SDL_Renderer **renderer, int x, int y, int h, int w);

void setBoatState(draggableItem boats[], boat boatsPlayer[]);

void getCaseHovered(gridCase *plateau, SDL_Point mousePosition, int *isChange, int grid[]);

int getCaseToReturn(gridCase *plateau);

#endif