
#include <stdio.h>
#include <stdlib.h>
#include "../headers/ihm-header.h"

void updateScrollBarText(SDL_Renderer **renderer, scrollBar *bar);

void setMusicVolume(int volume) {

    volume = (volume * MIX_MAX_VOLUME) / 100;

    Mix_VolumeMusic(volume);

}

void setChannelVolume(int volume, int channel) {

    volume = (volume * MIX_MAX_VOLUME) / 100;

    Mix_Volume(channel, volume);

}

timer startTimer() {

    timer element;

    element.endTime = 0;
    element.initialTime = SDL_GetTicks();
    element.time.mili = 0;
    element.time.sec = 0;
    element.time.min = 0;
    element.time.hours = 0;
    element.running = 1;

    return element;
}

void getTimerTime(timer *element) {

    int time = SDL_GetTicks() - element->initialTime;

    int seconds = time / 1000;

    element->time.mili = time - (seconds * 1000);

    int min = seconds / 60;

    element->time.sec = seconds - min * 60;

    element->time.hours = min / 60;

    element->time.min = min - element->time.hours * 60;

}

void printTime(timer element) {

    getTimerTime(&element);

    printf("%dh : %dmin : %dsec : %dmili\n", element.time.hours, element.time.min,
           element.time.sec, element.time.mili );
}

text createTextTexture(SDL_Renderer **renderer, const char textToPrint[],
                       char fontLocation[], int fontSize, int centerX, int centerY, int plcmtMode) {

    text element;

    SDL_Color fontColor = {255, 255, 255};
    TTF_Font *police = TTF_OpenFont(fontLocation, fontSize);

    SDL_Surface *surface = TTF_RenderUTF8_Blended(police, textToPrint,
                           fontColor);

    element.texture = SDL_CreateTextureFromSurface(*renderer, surface);

    SDL_GetClipRect(surface, &element.clip);

    element.plcmtMode = plcmtMode;

    switch (0xf & plcmtMode) {
    case LEFT:

        element.clip.x = centerX;
        break;

    case HCENTER:

        element.clip.x = centerX - (element.clip.w / 2);
        break;

    case RIGHT:

        element.clip.x = centerX - element.clip.w;
        break;
    }

    switch (0xf0 & plcmtMode) {
    case TOP:

        element.clip.y = centerY;
        break;

    case VCENTER:

        element.clip.y = centerY - (element.clip.h / 2);
        break;

    case BOTTOM:

        element.clip.y = centerY - element.clip.h;
        break;
    }

    TTF_CloseFont(police);
    SDL_FreeSurface(surface);

    return element;
}

void setAlpha(SDL_Renderer **renderer, int alpha) {

    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, alpha);

    SDL_RenderFillRect(*renderer, NULL);

}

void audioEffect(Mix_Chunk *sample, int volume) {

    volume = (volume * 128) / 100;
    Mix_VolumeChunk(sample, volume);

    Mix_PlayChannel(1, sample, 0 );
}

int ifPointInSquare(SDL_Rect clip, SDL_Point pos) {

    if (pos.y >= clip.y && pos.y <= clip.y + clip.h && pos.x >= clip.x &&
            pos.x <= clip.x + clip.w) {

        return 1;
    }
    else {
        return 0;
    }
}

button createButton(SDL_Renderer **renderer, char buttonFontLocation[],
                    int fontSize, const char buttonText[], int x, int y, int w, int h) {

    button element;

    element.label = createTextTexture(&*renderer, buttonText, buttonFontLocation,
                                      fontSize, x + (w / 2),
                                      y + (h / 2),
                                      HCENTER | VCENTER);

    element.clip.x = x;
    element.clip.y = y;
    element.clip.w = w;
    element.clip.h = h;
    element.isSelected = 0;

    element.activated = 1;

    return element;
}

void buttonGetState(SDL_Renderer **renderer, button *btn, Mix_Chunk **select,
                    int *isChange, SDL_Point mousePosition) {

    if ( ifPointInSquare(btn->clip, mousePosition) == 1 && btn->isSelected != 1) {

        btn->isSelected = 1;

        audioEffect(*select, 10);

        *isChange = 1;

    }
    else if (ifPointInSquare(btn->clip, mousePosition) == 0 && btn->isSelected == 1) {

        btn->isSelected = 0;

        *isChange = 1;
    }
}

void buttonOnClick(SDL_Rect clip, SDL_Point mousePosition , int *ret, int retAsk, Mix_Chunk **click) {

    if (ifPointInSquare(clip, mousePosition) == 1 ) {

        audioEffect(*click, 50);

        *ret = retAsk;
    }
}

void drawThickRect(SDL_Renderer **renderer, SDL_Rect clip, int thickness, int r,
                   int g , int b, int a) {

    SDL_SetRenderDrawColor(*renderer, r, g, b, a);

    for (int i = 0; i < thickness; ++i)
    {
        SDL_RenderDrawRect(*renderer, &clip);
        clip.x = clip.x + 1;
        clip.y = clip.y + 1;
        clip.h = clip.h - 2;
        clip.w = clip.w - 2;
    }

}

void buttonRender(SDL_Renderer **renderer, button *btn) {

    int alpha;

    if (btn->isSelected == 1)
    {
        SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 64);
        SDL_RenderFillRect(*renderer, &btn->clip);
    }

    if (btn->activated == 1)
    {
        alpha = 255;

        SDL_SetTextureAlphaMod(btn->label.texture, alpha);

    }
    else {
        alpha = 100;

        SDL_SetTextureAlphaMod(btn->label.texture, alpha);
    }

    SDL_RenderCopy(*renderer, btn->label.texture, NULL, &btn->label.clip);

    drawThickRect(&*renderer, btn->clip, sf(5), 255, 255 , 255, alpha);

}

SDL_Texture *loadTexture(SDL_Renderer **renderer, char imgLocation[]) {

    SDL_Surface *surface = IMG_Load(imgLocation);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(*renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

SDL_Rect getLargeCaseRect(int caseX, int caseY) {

    SDL_Rect caseRect;

    int totaSize = 0;
    int spaceB = 0;
    int caseSize = sf(65);
    int spaceBT = sf(2);

    totaSize = (10 * caseSize) + (spaceBT * 10);
    spaceB = (sf(720) - totaSize) / 2;

    caseRect.x = (caseX * caseSize) + (spaceBT * caseX) + spaceB;
    caseRect.y = (caseY * caseSize) + (spaceBT * caseY) + spaceB;
    caseRect.h = caseSize;
    caseRect.w = caseSize;

    return caseRect;
}

SDL_Rect getSmallCaseRect(int caseX, int caseY) {

    SDL_Rect caseRect;

    int caseSize = sf(33);
    int spaceBT = sf(1);

    int spaceH = sf(360);
    int spaceW = sf(752);

    caseRect.x = (caseX * caseSize) + (spaceBT * caseX) + spaceW - (caseX / 2);
    caseRect.y = (caseY * caseSize) + (spaceBT * caseY) + spaceH - (caseY / 2);
    caseRect.h = caseSize;
    caseRect.w = caseSize;

    return caseRect;
}

void ItemDrag(SDL_Renderer **renderer, draggableItem *item, SDL_Event event,
              SDL_Point mousePosition, int *isChange, int key) {

    int temp;

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:

        if (ifPointInSquare(item->clip, mousePosition) == 1 ) {

            item->dragging = 1;
            item->placed = 0;

            item->relativePos.x = event.button.x - item->clip.x;
            item->relativePos.y = event.button.y - item->clip.y;

            item->clip.x = event.button.x - item->relativePos.x;
            item->clip.y = event.button.y - item->relativePos.y;

            item->clipRender.x = event.button.x - item->relativePos.x;
            item->clipRender.y = event.button.y - item->relativePos.y;

            *isChange = 1;
        }

        break;

    case SDL_MOUSEMOTION:

        if (item->dragging == 1) {

            item->clip.x = event.motion.x - item->relativePos.x;
            item->clip.y = event.motion.y - item->relativePos.y;

            item->clipRender.x = event.button.x - item->relativePos.x;
            item->clipRender.y = event.button.y - item->relativePos.y;

            item->positionReference.x = item->clip.x + sf(32);
            item->positionReference.y = item->clip.y + sf(32);

            *isChange = 1;
        }

        break;

    case SDL_MOUSEBUTTONUP:

        if (item->dragging == 1)
        {
            if (item->placable == 0)
            {
                item->clip.x = item->firstPoint.x;
                item->clip.y = item->firstPoint.y;
                item->clipRender.x = item->firstPoint.x;
                item->clipRender.y = item->firstPoint.y;

                if (item->flipped == 1)
                {
                    item->flipped = 0;
                    temp = item->clip.h;
                    item->clip.h = item->clip.w;
                    item->clip.w = temp;
                }
            }
            else {

                item->clip.x = item->actualCase.x;
                item->clip.y = item->actualCase.y;

                item->clipRender.x = item->actualCase.x;
                item->clipRender.y = item->actualCase.y;

                item->placed = 1;
            }

            *isChange = 1;
            item->dragging = 0;

        }

        break;

    case SDL_KEYDOWN:

        if (item->dragging == 1)
        {
            if (event.key.keysym.sym == key)
            {
                if (item->flipped == 0)
                {
                    item->flipped = 1;
                    temp = item->clip.h;
                    item->clip.h = item->clip.w;
                    item->clip.w = temp;
                }
                else {

                    item->flipped = 0;

                    temp = item->clip.h;
                    item->clip.h = item->clip.w;
                    item->clip.w = temp;
                }

                *isChange = 1;
            }
        }
    }
}

draggableItem createDraggableItem(SDL_Renderer **renderer, char text[],
                                  int itemId, char img[], int x, int y, int w, int h, int nbCase) {

    draggableItem item;

    SDL_Surface *surface = IMG_Load(img);
    item.texture = SDL_CreateTextureFromSurface(*renderer, surface);
    SDL_FreeSurface(surface);

    int offset = 10;

    item.clip.x = x;
    item.clip.y = y;
    item.clip.w = w;
    item.clip.h = h;

    item.firstPoint.x = x;
    item.firstPoint.y = y;

    item.clipRender.x = x;
    item.clipRender.y = y;
    item.clipRender.w = w;
    item.clipRender.h = h;

    item.itemId = itemId;

    item.nbCase = nbCase;

    item.dragging = 0;

    item.flipped = 0;

    item.mouseOver = 0;

    item.placable = 0;

    item.placed = 0;

    item.placeHolder.clip.x = item.clip.x - (offset / 2);
    item.placeHolder.clip.y = item.clip.y - (offset / 2);
    item.placeHolder.clip.h = item.clip.h + offset;
    item.placeHolder.clip.w = item.clip.w + offset;

    item.placeHolder.text = createTextTexture(&*renderer, text, FONT_STANDARD,
                            16, item.placeHolder.clip.x,
                            item.placeHolder.clip.y + item.placeHolder.clip.h + 10,
                            LEFT | VCENTER);

    return item;

}

void highlightLargeCase(SDL_Renderer **renderer, gridCase *plateau) {

    for (int i = 0; i < 100; ++i)
    {
        if (plateau[i].highlighted == 1)
        {
            SDL_RenderFillRect(*renderer, &plateau[i].clip);
        }
    }
}

void highlightSmallStateCase(SDL_Renderer **renderer, int gridPlayer[]) {

    int a = 0;
    SDL_Rect caseRect;

    for (int j = 0; j < 10; ++j)
    {
        for (int i = 0; i < 10; ++i)
        {
            if (gridPlayer[a] > 0) {

                SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 128);
                caseRect = getSmallCaseRect(i, j);
                SDL_RenderFillRect(*renderer, &caseRect);
            }
            else if (gridPlayer[a] == -10) {

                SDL_SetRenderDrawColor(*renderer, 255, 0, 0, 128);
                caseRect = getSmallCaseRect(i, j);
                SDL_RenderFillRect(*renderer, &caseRect);
            }

            a++;
        }
    }
}

void highlightSmallStateCase2(SDL_Renderer **renderer, int gridPlayer[],
                              SDL_Texture **expl) {

    int a = 0;
    SDL_Rect caseRect;

    for (int j = 0; j < 10; ++j)
    {
        for (int i = 0; i < 10; ++i)
        {
            if (gridPlayer[a] != 0 && gridPlayer[a] != -10 &&
                    gridPlayer[a] < 0) {

                caseRect = getSmallCaseRect(i, j);
                SDL_RenderCopy(*renderer, *expl, NULL, &caseRect);
            }
            a++;
        }
    }
}

void highlightLargeStateCase(SDL_Renderer **renderer, gridCase plateau[],
                             int grid[], SDL_Texture **expl) {

    for (int i = 0; i < 100; ++i)
    {
        if (grid[i] <= -1 && grid[i] >= -10)
        {
            if (grid[i] == -10)
            {
                SDL_SetRenderDrawColor(*renderer, 236, 112, 99, 128);
                SDL_RenderFillRect(*renderer, &plateau[i].clip);
            }
            else
            {
                SDL_SetRenderDrawColor(*renderer, 130, 224, 170, 128);
                SDL_RenderFillRect(*renderer, &plateau[i].clip);
                SDL_RenderCopy(*renderer, *expl, NULL, &plateau[i].clip);
            }
        }
    }
}

void getCaseState(gridCase *plateau, draggableItem *item, int state) {

    if (item->dragging == state)
    {

        SDL_Point positionEnd;

        int mult = 1;
        int pixelLength = sf((item->nbCase - 1) * 66);
        int overlap;

        positionEnd.y = item->positionReference.y;
        positionEnd.x = item->positionReference.x + pixelLength;

        item->placable = 0;

        if (item->flipped == 1)
        {
            mult = 10;

            positionEnd.y = item->positionReference.y + pixelLength;
            positionEnd.x = item->positionReference.x;
        }

        for (int i = 0; i < 100; ++i)
        {
            if ( ifPointInSquare(plateau[i].clip , item->positionReference) == 1 &&
                    ((item->flipped == 1 && positionEnd.y <= sf(685)) ||
                     (item->flipped == 0 && positionEnd.x <= sf(685)))) {

                overlap = 0;

                if (plateau[i].highlighted == 1)
                {
                    overlap = 1;
                }

                for (int z = 1; z < item->nbCase; ++z)
                {
                    if (plateau[i + (z * mult)].highlighted == 1)
                    {
                        overlap = 1;
                    }
                }

                if (overlap == 0)
                {
                    plateau[i].highlighted = 1;
                    plateau[i].occuped = item->itemId ;

                    item->actualCase = plateau[i].clip;

                    for (int z = 1; z < item->nbCase; ++z)
                    {
                        plateau[i + (z * mult)].highlighted = 1;
                        plateau[i + (z * mult)].occuped = item->itemId ;
                    }

                    item->placable = 1;
                }
            }
        }
    }
}

void clearCaseToHighLight(gridCase *plateau) {

    for (int i = 0; i < 100; ++i)
    {
        plateau[i].highlighted = 0;
        plateau[i].occuped = 0;
    }
}

void initPlateau(gridCase *plateau) {

    int a = 0;

    for (int j = 0; j < 10; ++j)
    {
        for (int i = 0; i < 10; ++i)
        {
            plateau[a].occuped = 0;
            plateau[a].highlighted = 0;
            plateau[a].clip = getLargeCaseRect(i, j);
            a++;
        }
    }
}

void renderLargeItem(SDL_Renderer **renderer, draggableItem *item, int state) {

    if (item->dragging == state)
    {

        int alpha;

        SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 30);

        SDL_RenderFillRect(*renderer, &item->placeHolder.clip);

        if (item->dragging == 1 || item->placed == 1)
        {
            alpha = 100;

        }
        else
        {
            alpha = 255;
        }

        SDL_SetTextureAlphaMod(item->placeHolder.text.texture, alpha);

        drawThickRect(&*renderer, item->placeHolder.clip, sf(2), 255, 255 , 255,
                      alpha);

        SDL_RenderCopy(*renderer, item->placeHolder.text.texture, NULL,
                       &item->placeHolder.text.clip);

        SDL_Point center;
        center.x = sf(32);
        center.y = sf(32);

        SDL_RenderCopyEx(*renderer, item->texture, NULL, &item->clipRender,
                         item->flipped * 90, &center, SDL_FLIP_NONE);

    }
}

void rendererAllItems(SDL_Renderer **renderer, draggableItem boats[]) {

    renderLargeItem(&*renderer, &boats[0], 0);
    renderLargeItem(&*renderer, &boats[1], 0);
    renderLargeItem(&*renderer, &boats[2], 0);
    renderLargeItem(&*renderer, &boats[3], 0);
    renderLargeItem(&*renderer, &boats[4], 0);

    renderLargeItem(&*renderer, &boats[0], 1);
    renderLargeItem(&*renderer, &boats[1], 1);
    renderLargeItem(&*renderer, &boats[2], 1);
    renderLargeItem(&*renderer, &boats[3], 1);
    renderLargeItem(&*renderer, &boats[4], 1);

}

SDL_Rect getLargeGridRect() {

    int caseSize = sf(65);
    int spaceBT = sf(2);

    int totaSize = (10 * caseSize) + (spaceBT * 9);
    int spaceB = (sf(720) - totaSize) / 2;

    SDL_Rect gridRect;
    gridRect.x = spaceB;
    gridRect.y = spaceB;
    gridRect.h = totaSize;
    gridRect.w = totaSize;

    return gridRect;
}

SDL_Rect getSmallGridRect() {

    int caseSize = sf(33);
    int spaceBT = sf(1);

    int totaSize = (10 * caseSize) + (spaceBT * 9);
    int spaceH = sf(360);
    int spaceW = sf(752);

    SDL_Rect gridRect;
    gridRect.x = spaceW;
    gridRect.y = spaceH;
    gridRect.h = totaSize;
    gridRect.w = totaSize;

    return gridRect;
}

void drawLargeGrid(SDL_Renderer **renderer) {

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);

    SDL_Rect caseRect;

    SDL_Point firstPoint;
    SDL_Point secondPoint;

    SDL_Rect gridRect = getLargeGridRect();

    gridRect.x = gridRect.x - sf(5);
    gridRect.y = gridRect.y - sf(5);
    gridRect.h = gridRect.h + (sf(5) * 2);
    gridRect.w = gridRect.w + (sf(5) * 2);

    int spaceBT = sf(2);

    for (int i = 1; i <= 9; ++i)
    {
        caseRect = getLargeCaseRect(i, 0);

        firstPoint.x = caseRect.x - 1;
        firstPoint.y = caseRect.y;

        caseRect = getLargeCaseRect(i, 10);

        secondPoint.x = caseRect.x - 1;
        secondPoint.y = caseRect.y;

        for (int i = 0; i < spaceBT; ++i)
        {
            SDL_RenderDrawLine(*renderer, firstPoint.x - i, firstPoint.y,
                               secondPoint.x - i, secondPoint.y);
        }

        caseRect = getLargeCaseRect(0, i);

        firstPoint.x = caseRect.x;
        firstPoint.y = caseRect.y - 1;

        caseRect = getLargeCaseRect(10, i);

        secondPoint.x = caseRect.x;
        secondPoint.y = caseRect.y - 1;

        for (int i = 0; i < spaceBT; ++i)
        {
            SDL_RenderDrawLine(*renderer, firstPoint.x, firstPoint.y - i,
                               secondPoint.x, secondPoint.y - i);
        }

        drawThickRect(&*renderer, gridRect, sf(5), 255, 255 , 255, 255);
    }
}

void drawSmallGrid(SDL_Renderer **renderer) {

    SDL_Rect caseRect;

    SDL_Point firstPoint;
    SDL_Point secondPoint;

    SDL_Rect gridRect = getSmallGridRect();

    gridRect.x = gridRect.x - sf(2);
    gridRect.y = gridRect.y - sf(2);
    gridRect.h = gridRect.h + (sf(2) * 2);
    gridRect.w = gridRect.w + (sf(2) * 2);

    int spaceBT = sf(1);

    for (int i = 1; i <= 9; ++i)
    {
        caseRect = getSmallCaseRect(i, 0);

        firstPoint.x = caseRect.x - 1;
        firstPoint.y = caseRect.y;

        caseRect = getSmallCaseRect(i, 10);

        secondPoint.x = caseRect.x - 1;
        secondPoint.y = caseRect.y;

        for (int i = 0; i < spaceBT; ++i)
        {
            SDL_RenderDrawLine(*renderer, firstPoint.x - i, firstPoint.y,
                               secondPoint.x - i, secondPoint.y);
        }

        caseRect = getSmallCaseRect(0, i);

        firstPoint.x = caseRect.x;
        firstPoint.y = caseRect.y - 1;

        caseRect = getSmallCaseRect(10, i);

        secondPoint.x = caseRect.x;
        secondPoint.y = caseRect.y - 1;

        for (int i = 0; i < spaceBT; ++i)
        {
            SDL_RenderDrawLine(*renderer, firstPoint.x, firstPoint.y - i,
                               secondPoint.x, secondPoint.y - i);
        }

        drawThickRect(&*renderer, gridRect, sf(2), 255, 255 , 255, 255);
    }
}

void renderSmallItem(SDL_Renderer **renderer, boat boats[]) {

    SDL_Point center;
    center.x = sf(16);
    center.y = sf(16);

    for (int i = 0; i < 5; ++i)
    {
        SDL_Rect clipRender = getSmallCaseRect(boats[i].actualCase.x,
                                               boats[i].actualCase.y);
        clipRender.w = sf(32) * boats[i].lenght;
        clipRender.h = sf(32);

        SDL_RenderCopyEx(*renderer, boats[i].texture, NULL, &clipRender,
                         boats[i].flipped * 90, &center, SDL_FLIP_NONE);

    }
}

void ifEnteredInSquare(SDL_Rect clip, int *mouseOver, SDL_Point mousePosition) {

    if ( ifPointInSquare(clip, mousePosition) == 1 && *mouseOver != 1) {
        *mouseOver = 1;

    }
    else if ( ifPointInSquare(clip, mousePosition) == 0 && *mouseOver == 1) {

        *mouseOver = 0;
    }
}

void cursorSwitchItem(SDL_Cursor** cursor, SDL_Point mousePosition,
                      draggableItem *item, int  *cursorMode) {

    ifEnteredInSquare(item->clip, &item->mouseOver, mousePosition);

    if (item->mouseOver == 1)
    {
        *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(*cursor);

        *cursorMode = 1;
    }
}

void getGridState(gridCase *plateau, draggableItem boats[]) {

    clearCaseToHighLight(plateau);

    getCaseState(plateau, &boats[0], 0);
    getCaseState(plateau, &boats[1], 0);
    getCaseState(plateau, &boats[2], 0);
    getCaseState(plateau, &boats[3], 0);
    getCaseState(plateau, &boats[4], 0);

    getCaseState(plateau, &boats[0], 1);
    getCaseState(plateau, &boats[1], 1);
    getCaseState(plateau, &boats[2], 1);
    getCaseState(plateau, &boats[3], 1);
    getCaseState(plateau, &boats[4], 1);

}

void ItemDragGlobal(SDL_Renderer **renderer, SDL_Event event,
                    SDL_Point mousePosition, int *isChange, draggableItem boats[], int key) {

    ItemDrag(&*renderer, &boats[0], event, mousePosition, &*isChange, key);
    ItemDrag(&*renderer, &boats[1], event, mousePosition, &*isChange, key);
    ItemDrag(&*renderer, &boats[2], event, mousePosition, &*isChange, key);
    ItemDrag(&*renderer, &boats[3], event, mousePosition, &*isChange, key);
    ItemDrag(&*renderer, &boats[4], event, mousePosition, &*isChange, key);
}

void cursorSwitchPlacement(SDL_Cursor **cursor, SDL_Point mousePosition,
                           int *cursorMode, draggableItem boats[]) {

    cursorSwitchItem(&*cursor, mousePosition, &boats[0], &*cursorMode);
    cursorSwitchItem(&*cursor, mousePosition, &boats[1], &*cursorMode);
    cursorSwitchItem(&*cursor, mousePosition, &boats[2], &*cursorMode);
    cursorSwitchItem(&*cursor, mousePosition, &boats[3], &*cursorMode);
    cursorSwitchItem(&*cursor, mousePosition, &boats[4], &*cursorMode);

    if (boats[0].mouseOver == 0 && boats[1].mouseOver == 0 &&
            boats[2].mouseOver == 0 && boats[3].mouseOver == 0 &&
            boats[4].mouseOver == 0 && *cursorMode == 1)
    {
        *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(*cursor);

        cursorMode = 0;

    }

    boats[0].mouseOver = 0;
    boats[1].mouseOver = 0;
    boats[2].mouseOver = 0;
    boats[3].mouseOver = 0;
    boats[4].mouseOver = 0;

}

scrollBar createScrollBarRGBA(SDL_Renderer **renderer, char *name, int value, int barId,
                              int barLenght, int barHeight, int barX, int barY, int circleRadius, int r, int g, int b, int a) {

    int offset = 3;

    int temp = 0;

    scrollBar bar;

    bar.changedValue = 0;

    SDL_Surface *surface = IMG_Load("../images/circle.png");

    SDL_Texture *texture = SDL_CreateTextureFromSurface(*renderer, surface);

    SDL_SetTextureColorMod(texture, 52, 151, 219);

    bar.clipRender.x = barX;
    bar.clipRender.y = barY;
    bar.clipRender.h = barHeight;
    bar.clipRender.w = barLenght;

    bar.topText = createTextTexture(&*renderer, name, "../fonts/Gonsterrat.ttf", sf(16),
                                    bar.clipRender.x, bar.clipRender.y - sf(16) , LEFT | VCENTER);

    bar.bottomText = createTextTexture(&*renderer, "0%", "../fonts/Gonsterrat.ttf", sf(10),
                                       bar.clipRender.x + sf(3), bar.clipRender.y + bar.clipRender.h + sf(8), LEFT | VCENTER);


    bar.clip.x = bar.clipRender.x - offset;
    bar.clip.y = bar.clipRender.y - offset;
    bar.clip.h = bar.clipRender.h + (offset * 2);
    bar.clip.w = bar.clipRender.w + (offset * 2);

    bar.circleRadius = circleRadius;
    bar.circleRadiusRender = 0;
    bar.circleTexture = texture;

    bar.mouseOver = 0;

    bar.drag = 0;

    bar.prev = 0;

    bar.fadeIn = 0;
    bar.fadeOut = 0;

    bar.circleCenterY = barY + (barHeight / 2);

    bar.value = value;
    temp = (value * bar.clipRender.w) / 100;
    bar.circleCenterX = temp + bar.clipRender.x;

    bar.id = barId;

    bar.color.r = r;
    bar.color.g = g;
    bar.color.b = b;
    bar.color.a = a;

    updateScrollBarText(&*renderer, &bar);

    return bar;
}

SDL_Texture *takeScreenShot(SDL_Renderer **renderer, SDL_Rect clip) {

    SDL_Surface *sshot = SDL_CreateRGBSurface(0, clip.w, clip.h, 32, 0x00ff0000,
                         0x0000ff00, 0x000000ff, 0xff000000);

    SDL_RenderReadPixels(*renderer, &clip, SDL_PIXELFORMAT_ARGB8888,
                         sshot->pixels, sshot->pitch);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(*renderer, sshot);

    SDL_FreeSurface(sshot);

    return texture;

}

void scrollBarGetState(SDL_Renderer **renderer, scrollBar *bar, SDL_Event event, SDL_Point mousePosition,
                       int *isChange, SDL_Cursor **cursor) {

    int tempValue = 0;

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:

        if (ifPointInSquare(bar->clip, mousePosition) == 1 ) {

            bar->drag = 1;

            bar->circleCenterX = mousePosition.x;

            *isChange = 1;
        }

        break;

    case SDL_MOUSEMOTION:

        if (bar->fadeOut == 0 && bar->fadeIn == 0)
        {
            if (ifPointInSquare(bar->clip, mousePosition) == 1)
            {
                if (bar->mouseOver == 0)
                {
                    *isChange = 1;
                    bar->mouseOver = 1;

                    bar->fadeIn = 1;

                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
                    SDL_SetCursor(*cursor);
                }
            }
            else if (bar->mouseOver == 1) {

                *isChange = 1;
                bar->mouseOver = 0;

                if (bar->drag != 1 )
                {
                    bar->fadeOut = 1;
                }

                if (bar->drag == 0)
                {
                    *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                    SDL_SetCursor(*cursor);
                }
            }
        }

        if (bar->drag == 1) {

            if (mousePosition.x <= (bar->clipRender.x + bar->clipRender.w) &&
                    mousePosition.x >= (bar->clipRender.x))
            {

                bar->circleCenterX = mousePosition.x;
                *isChange = 1;
            }
            else if (mousePosition.x >= bar->clipRender.x + bar->clipRender.w)
            {
                bar->circleCenterX = bar->clipRender.x + bar->clipRender.w;
                *isChange = 1;
            }
            else if (mousePosition.x <= bar->clipRender.x)
            {
                bar->circleCenterX = bar->clipRender.x;
                *isChange = 1;
            }
        }

        break;

    case SDL_MOUSEBUTTONUP:

        if (ifPointInSquare(bar->clip, mousePosition) == 0)
        {
            if (bar->drag == 1)
            {
                bar->fadeOut = 1;

                *cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                SDL_SetCursor(*cursor);
            }
        }

        bar->drag = 0;

        *isChange = 1;

        break;
    }

    if (bar->drag == 1)
    {
        tempValue = ((bar->circleCenterX - bar->clipRender.w +
                      (bar->clipRender.w - bar->clipRender.x))
                     / (double)bar->clipRender.w) * 100;

        if (tempValue != bar->value)
        {
            bar->value = tempValue;
            bar->changedValue = 1;
            updateScrollBarText(&*renderer, bar);
        }

        *isChange = 1;
    }
}

void scrollBarRender(SDL_Renderer **renderer, scrollBar bar) {

    SDL_Rect currentStatutBar = bar.clipRender;
    currentStatutBar.w = bar.circleCenterX - currentStatutBar.x;

    SDL_SetRenderDrawColor(*renderer, 255, 255, 255, bar.color.a);

    SDL_RenderFillRect(*renderer, &bar.clipRender);

    SDL_SetRenderDrawColor(*renderer, bar.color.r, bar.color.g, bar.color.b,
                           bar.color.a);

    SDL_RenderFillRect(*renderer, &currentStatutBar);

    SDL_SetTextureAlphaMod(bar.circleTexture, bar.color.a);

    SDL_SetTextureAlphaMod(bar.topText.texture, bar.color.a);

    SDL_RenderCopy(*renderer, bar.topText.texture, NULL, &bar.topText.clip);

    SDL_Rect circleStart;
    circleStart.x = bar.clipRender.x - (bar.clipRender.h / 2);
    circleStart.y = bar.clipRender.y;
    circleStart.w = bar.clipRender.h;
    circleStart.h = bar.clipRender.h;

    SDL_RenderCopy(*renderer, bar.circleTexture, NULL, &circleStart);

    if (currentStatutBar.w > bar.clipRender.w - (bar.clipRender.w * 0.05))
    {
        SDL_SetTextureColorMod(bar.circleTexture, bar.color.r, bar.color.g, bar.color.b);
    }
    else
    {
        SDL_SetTextureColorMod(bar.circleTexture, 255, 255, 255);
    }

    circleStart.x = bar.clipRender.x + bar.clipRender.w - (bar.clipRender.h / 2);
    circleStart.y = bar.clipRender.y;
    circleStart.w = bar.clipRender.h;
    circleStart.h = bar.clipRender.h;

    SDL_RenderCopy(*renderer, bar.circleTexture, NULL, &circleStart);


    SDL_SetTextureColorMod(bar.circleTexture, bar.color.r, bar.color.g, bar.color.b);

    SDL_Rect circleRect;
    circleRect.h = bar.circleRadiusRender * 2;
    circleRect.w = bar.circleRadiusRender * 2;
    circleRect.x = bar.circleCenterX - bar.circleRadiusRender;
    circleRect.y = bar.circleCenterY - bar.circleRadiusRender;

    SDL_RenderCopy(*renderer, bar.circleTexture, NULL, &circleRect);

    SDL_SetTextureAlphaMod(bar.bottomText.texture, bar.color.a);

    SDL_RenderCopy(*renderer, bar.bottomText.texture, NULL, &bar.bottomText.clip);
}

void renderRect(SDL_Renderer **renderer, int x, int y, int h, int w) {

    SDL_Rect test = {x, y, w, h};
    SDL_RenderFillRect(*renderer, &test);
}

void setBoatState(draggableItem boats[], boat boatsPlayer[]) {

    for (int y = 0; y < 5; ++y)
    {
        for (int j = 0; j < 10; ++j)
        {
            for (int i = 0; i < 10; ++i)
            {
                if (ifPointInSquare(getLargeCaseRect(i, j),
                                    boats[y].positionReference) == 1) {

                    boatsPlayer[y].actualCase.x = i;
                    boatsPlayer[y].actualCase.y = j;
                }
            }
        }

        boatsPlayer[y].flipped = boats[y].flipped;
        boatsPlayer[y].lenght = boats[y].nbCase;
        boatsPlayer[y].texture = boats[y].texture;
        boatsPlayer[y].isCount = 0;
        boatsPlayer[y].isSunk = 0;
        boatsPlayer[y].id = y+1;
    }
}

void getCaseHovered(gridCase *plateau, SDL_Point mousePosition, int *isChange,
                    int grid[]) {

    int a = 0;

    for (int caseY = 0; caseY < 10; ++caseY)
    {
        for (int caseX = 0; caseX < 10; ++caseX)
        {
            if (ifPointInSquare(getLargeCaseRect(caseX, caseY), mousePosition) &&
                    (grid[a] > -1 || grid[a] < -10))
            {
                plateau[a].highlighted = 1;
                *isChange = 1;
            }
            else {

                if (plateau[a].highlighted == 1)
                {
                    plateau[a].highlighted = 0;
                    *isChange = 1;
                }
            }
            a++;
        }
    }
}

int getCaseToReturn(gridCase *plateau) {

    int caseToReturn = -1;

    for (int i = 0; i < 100; ++i)
    {
        if (plateau[i].highlighted == 1)
        {
            caseToReturn = i;
        }
    }

    return caseToReturn;
}
