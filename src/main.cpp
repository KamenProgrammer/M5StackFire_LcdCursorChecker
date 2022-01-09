#include <vector>
#include <Arduino.h>
#include "M5Stack.h"

#define BTN_MENU_SIZE   (2)
#define BTN_MENU_POSY   (240 - (8 * BTN_MENU_SIZE))

uint8_t CharaWidth[] = {6, 8, 0, 16, 0, 27, 32, 55};

class SelectMenuItem {
private:
    const char* _name;
    int16_t _maxValue;
    int16_t _minValue;
    int16_t _nowValue;

   public:
    SelectMenuItem(const char* name, int16_t maxValue, int16_t minValue) {
        _name = name;
        if ( maxValue > minValue ) {
            _maxValue = maxValue;
            _minValue = minValue;
        } else {
            _maxValue = -1;
            _minValue = -1;
        }
        _nowValue = _minValue;
    }
    
    void incValue(void) {
        if (_nowValue >= _maxValue) {
            _nowValue = _minValue;
        } else {
            _nowValue++;
        }
    }

    void decValue(void) {
        if (_nowValue <= _minValue) {
            _nowValue = _maxValue;
        } else {
            _nowValue--;
        }
    }

    void drawItem(bool isSelected) {
        if (isSelected) {
            M5.Lcd.setTextColor(BLACK, WHITE);
        } else {
            M5.Lcd.setTextColor(WHITE, BLACK);
        }
        M5.Lcd.printf("%s:%d\n", _name, _nowValue);
        M5.Lcd.setTextColor(WHITE, BLACK);
    }

    int16_t getNowValue(void) {
        return _nowValue;
    }
};

std::vector<SelectMenuItem *> Items;
uint8_t SelectedItemNumber;

void setup() {

    M5.begin(true, false, false, false);

    Items.push_back(new SelectMenuItem("Size", 7, 1));
    Items.push_back(new SelectMenuItem("Font", 8, 1));
    Items.push_back(new SelectMenuItem("XLine", 319, 0));
    Items.push_back(new SelectMenuItem("YLine", 239, 0));

    SelectedItemNumber = 0;
}

void loop() {
    static bool isUpdated = true;
    uint8_t TextSize;
    uint8_t TextFont;

    // button action
    M5.update();
    if (M5.BtnA.isPressed()) {
    SelectedItemNumber++;
        if (SelectedItemNumber >= Items.size()) {
        SelectedItemNumber = 0;
        }
        isUpdated = true;
    }
    if (M5.BtnB.isPressed()) {
        Items[SelectedItemNumber]->incValue();
        isUpdated = true;
    }
    if (M5.BtnC.isPressed()) {
        Items[SelectedItemNumber]->decValue();
        isUpdated = true;
    }

    if (isUpdated) {
        int cursorX[2];
        int cursorY[2];
        uint8_t textLengthOneLine;

        TextSize = Items[0]->getNowValue();
        TextFont = Items[1]->getNowValue();

        // clear dispray
        M5.Lcd.fillScreen(BLACK);

        // draw text        
        M5.Lcd.setTextSize(TextSize);
        M5.Lcd.setTextFont(TextFont);
        M5.Lcd.setCursor(0, 0);

        // draw a text and cursor
        if (CharaWidth[TextFont - 1] != 0) {
            textLengthOneLine = (uint8_t)(320 / (CharaWidth[TextFont - 1] * TextSize));
            for (uint8_t i = 0; i < textLengthOneLine; i++) {
                M5.Lcd.printf("%d", (i + 1) % 10);
            }
        }
        cursorX[0] = M5.Lcd.getCursorX();
        cursorY[0] = M5.Lcd.getCursorY();
        //M5.Lcd.printf(" cursor=(%3d,%3d)\n", cursorX, cursorY);

        // draw a charactor and cursor
        M5.Lcd.printf("0");
        cursorX[1] = M5.Lcd.getCursorX();
        cursorY[1] = M5.Lcd.getCursorY();
        //M5.Lcd.printf(" cursor=(%3d,%3d)\n", cursorX, cursorY);

        // draw line
        M5.Lcd.drawFastVLine(Items[2]->getNowValue(), 0, 240, BLUE);
        M5.Lcd.drawFastHLine(0, Items[3]->getNowValue(), 320, BLUE);

         // draw button menu
        M5.Lcd.setTextFont(1);
        M5.Lcd.setTextSize(BTN_MENU_SIZE);
        M5.Lcd.setCursor(35, BTN_MENU_POSY);
        M5.Lcd.print("SEL");
        M5.Lcd.setCursor(47 + 106, BTN_MENU_POSY);
        M5.Lcd.print("+");
        M5.Lcd.setCursor(47 + 106 + 106, BTN_MENU_POSY);
        M5.Lcd.print("-");

        // draw select menu
        M5.Lcd.setTextFont(1);
        M5.Lcd.setTextSize(BTN_MENU_SIZE);
        uint8_t posY = 240 - ((8 * BTN_MENU_SIZE) * (Items.size() + 1));
        M5.Lcd.setCursor(0, posY);
        for (uint8_t i = 0; i < Items.size(); i++) {
            Items[i]->drawItem(SelectedItemNumber == i);
        }

        // draw cursol pos
        posY = 240 - ((8 * BTN_MENU_SIZE) * (Items.size() + 1));
        M5.Lcd.setCursor(120, posY);
        M5.Lcd.printf("full:(%3d,%3d)", cursorX[0], cursorY[0]);
        M5.Lcd.setCursor(120, posY + 16);
        M5.Lcd.printf("one :(%3d,%3d)", cursorX[1], cursorY[1]);

       // inhibit repeated draw
        isUpdated = false;
    }

    delay(100);
}