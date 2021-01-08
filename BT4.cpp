#include "BT4.h"
#include "../../version.h"
#ifdef HW_M5PAPER
#include <M5EPD.h>
#endif
#ifdef HW_M5CORE2
#include <M5Core2.h>
#endif
#include <ArduinoJson.h>
#include "../GUI.h"
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
Button4Page::Button4Page(JsonObject obj, PAG_pos_t cp, bool paramUseSdCard)
{
    canvas_pos.x = 0;
    canvas_pos.y = 0;
    canvas_pos = cp;
    for (uint32_t x = 0; x < BT4_NUM_BUT; x++)
    {
        Serial.printf("BT4 INF Loading Button: %d\r\n", x);
        imageOn[x] = obj["imagesOn"][x].as<String>();
        imageOff[x] = obj["imagesOff"][x].as<String>();
        itemId[x] = obj["ids"][x].as<String>();
        varOnly[x] = obj["varOnly"][x].as<bool>();
        header = obj["head"].as<String>();
        if (paramUseSdCard)
        {
            fsHandler = &SD;
        }
        else
        {
            fsHandler = &SPIFFS;
        }

        GUI_CheckImage(imageOn[x]);
        GUI_CheckImage(imageOff[x]);
    }
    header = obj["head"].as<String>();
#ifdef HW_M5PAPER
    canvas.createCanvas(320, 240);
#endif
    Serial.printf("BT4 INF Button page loaded\r\n");
}

/**
 * Header Render function
 * */
void Button4Page::renderHeader(const char *string)
{
    canvas.setTextSize(1);
    canvas.fillRect(3, 4, 316, 20, PAG_FOREGND);
    canvas.setTextColor(0);
    canvas.setTextDatum(TC_DATUM);
    canvas.drawString(string, 160, 3, 4);
}

void Button4Page::activate()
{
    active = true;
#ifdef HW_M5PAPER
    canvas.fillCanvas(BLACK);
#endif
#ifdef HW_M5CORE2
    canvas.fillScreen(BLACK);
#endif

    PAG_pos_t pos;
    pos.y = -1;
    pos.x = -1;
    //handleInput(pos);
    draw();
}
void Button4Page::deActivate()
{
    active = false;
}

String Button4Page::getHeader()
{
    return (header);
}
void Button4Page::draw()
{
    uint32_t z = 0;

    for (uint32_t x; x < BT4_NUM_BUT; x++)
    {
        if (btState[x])
        {
            canvas.drawBmpFile(*fsHandler, imageOn[x].c_str(), IMG_POS_X[x], IMG_POS_Y[x]);
        }
        else
        {
            canvas.drawBmpFile(*fsHandler, imageOff[x].c_str(), IMG_POS_X[x], IMG_POS_Y[x]);
        }
    }
    renderHeader(header.c_str());
#ifdef HW_M5PAPER
    if (GUI_cachedUpdate())
    {
        canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_NONE);
    }
    else
    {
        canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_GC16);
    }
#endif
}

void Button4Page::redrawBt(uint8_t img)
{

    if (btState[img])
    {
        canvas.drawBmpFile(*fsHandler, imageOn[img].c_str(), IMG_POS_X[img], IMG_POS_Y[img]);
    }
    else
    {
        canvas.drawBmpFile(*fsHandler, imageOff[img].c_str(), IMG_POS_X[img], IMG_POS_Y[img]);
    }

#ifdef HW_M5PAPER
    canvas.pushCanvas(canvas_pos.x, canvas_pos.y, UPDATE_MODE_GC16);
#endif
}
void Button4Page::middleButtonPushed()
{
}
bool Button4Page::isInArea(int xT, int yT, int x, int y, int sizeX, int sizeY)
{
    bool retVal = ((xT > 0) && (xT > x) && (xT < (x + sizeX)) && (yT > y) && (yT < (y + sizeY)));
    return (retVal);
}
void Button4Page::handleInput(PAG_pos_t pos)
{
    char cstr[16];
    if (active)
    {
        if (pos.x != -1)
        {
            bool tTState;
            for (int x; x < BT4_NUM_BUT; x++)
            {
                tTState = isInArea(pos.x, pos.y, IMG_POS_X[x], IMG_POS_Y[x], IMG_HEIGHT, IMG_WIDTH);
                if (tTState != tLastState[x])
                {
                    tLastState[x] = tTState;
                    if (tTState)
                    {
                        btState[x] = !btState[x];
                        if (varOnly[x])
                        {
                            if (btState[x])
                            {
                                JsonRPC::execute_boolean("SetValue", itemId[x] + ", true");
                            }
                            else
                            {
                                JsonRPC::execute_boolean("SetValue", itemId[x] + ", false");
                            }
                        }
                        else
                        {
                            if (btState[x])
                            {
                                JsonRPC::execute_boolean("RequestAction", itemId[x] + ", true");
                            }
                            else
                            {
                                JsonRPC::execute_boolean("RequestAction", itemId[x] + ", false");
                            }
                        }

                        redrawBt(x);
                    }
                }
            }
            lastUpdate = millis();
        }
        else
        {
            tLastState[0] = tLastState[1] = tLastState[2] = tLastState[3] = false;
            if ((millis() - lastUpdate) > 500)
            {
                for (uint32_t x = 0; x < BT4_NUM_BUT; x++)
                {
                    bool uVal = false;
                    uVal = JsonRPC::execute_boolean("GetValue", String(itemId[x]));
                    if (JsonRPC::checkStatus())
                    {

                        if (btState[x] != uVal)
                        {
                            btState[x] = uVal;
                            Serial.printf("BT4 INF State of Bt %d changed\r\n", uVal);
                            redrawBt(x);
                        }
                    }
                }
                lastUpdate = millis();
            }
        }
    }
}
