#pragma once
#include "../PAG/PAG.h"
#include "../../version.h"
#ifdef HW_M5PAPER
#include <M5EPD.h>
#endif
#ifdef HW_M5CORE2
#include <M5Core2.h>
#endif
#include "../../rpc/RPC.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#define BT4_NUM_BUT 4
class Button4Page : public Page
{
private:
    String imageOn[BT4_NUM_BUT];
    String imageOff[BT4_NUM_BUT];
    String itemId[BT4_NUM_BUT];
    bool tLastState[BT4_NUM_BUT];
    bool btState[BT4_NUM_BUT];
    bool varOnly[BT4_NUM_BUT];
    String header;
    FS *fsHandler;
    JsonRPC rpc;
#ifdef HW_M5PAPER
    M5EPD_Canvas canvas = (&M5.EPD);
#endif
#ifdef HW_M5CORE2
    #define canvas M5.Lcd
#endif

    const uint16_t IMG_POS_X[BT4_NUM_BUT] = {60, 60, 163, 163};
    const uint16_t IMG_POS_Y[BT4_NUM_BUT] = {32, 135, 32, 135};
    const uint16_t IMG_WIDTH = 96;
    const uint16_t IMG_HEIGHT = 96;
    uint32_t lastUpdate = 0;
    static bool isInArea(int xT, int yT, int x, int y, int sizeX, int sizeY);
    void redrawBt(uint8_t img);
    void renderHeader(const char *string);

public:
    Button4Page(JsonObject obj, PAG_pos_t cp, bool paramUseSdCard);
    void activate();
    void deActivate();
    void draw();
    void handleInput(PAG_pos_t pos);
    void middleButtonPushed();
    String getHeader();
};