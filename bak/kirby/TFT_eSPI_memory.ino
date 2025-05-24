// TFT_eSPI_memory
//
// Example sketch which shows how to display an
// animated GIF image stored in FLASH memory
//
// written by Larry Bank
// bitbank@pobox.com
//
// Adapted by Bodmer for the TFT_eSPI Arduino library:
// https://github.com/Bodmer/TFT_eSPI
//
// To display a GIF from memory, a single callback function
// must be provided - GIFDRAW
// This function is called after each scan line is decoded
// and is passed the 8-bit pixels, RGB565 palette and info
// about how and where to display the line. The palette entries
// can be in little-endian or big-endian order; this is specified
// in the begin() method.
//
// The AnimatedGIF class doesn't allocate or free any memory, but the
// instance data occupies about 22.5K of RAM.

// #define USE_DMA       // ESP32 ~1.25x single frame rendering performance boost for badgers.h
//  Note: Do not use SPI DMA if reading GIF images from SPI SD card on same bus as TFT
#define NORMAL_SPEED  // Comment out for rame rate for render speed test

// Load GIF library
#include <AnimatedGIF.h>
AnimatedGIF gif;

// Example AnimatedGIF library images
#include "./kirby.h"

#define GIF_IMAGE kirby  //  No DMA  63 fps, DMA:  71fps

#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define DISPLAY_WIDTH tft.width()
#define DISPLAY_HEIGHT tft.height()
#define BUFFER_SIZE 256  // Optimum is >= GIF width or integral division of width

#ifdef USE_DMA
uint16_t usTemp[2][BUFFER_SIZE];  // Global to support DMA use
#else
uint16_t usTemp[1][BUFFER_SIZE];  // Global to support DMA use
#endif
bool dmaBuf = 0;

void setup() {
    Serial.begin(115200);

    tft.begin();
#ifdef USE_DMA
    tft.initDMA();
#endif
    tft.setRotation(1);
    tft.fillScreen(TFT_WHITE);

    gif.begin(BIG_ENDIAN_PIXELS);
}

void loop() {
    // Put your main code here, to run repeatedly:
    if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw)) {
        Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
        tft.startWrite();  // The TFT chip select is locked low
        while (gif.playFrame(true, NULL)) {
            yield();
        }
        gif.close();
        tft.endWrite();  // Release TFT chip select for other SPI devices
    }
}

void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *d, *usPalette;
    int x, y, iWidth, iCount;

    // Display bounds check and cropping
    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > DISPLAY_WIDTH)
        iWidth = DISPLAY_WIDTH - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y;  // current line
    if (y >= DISPLAY_HEIGHT || pDraw->iX >= DISPLAY_WIDTH || iWidth < 1)
        return;

    // Old image disposal
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2)  // restore to background color
    {
        for (x = 0; x < iWidth; x++) {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency)  // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        pEnd = s + iWidth;
        x = 0;
        iCount = 0;  // count non-transparent pixels
        while (x < iWidth) {
            c = ucTransparent - 1;
            d = &usTemp[0][0];
            while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE) {
                c = *s++;
                if (c == ucTransparent)  // done, stop
                {
                    s--;  // back up to treat it like transparent
                } else    // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            }  // while looking for opaque pixels
            if (iCount)  // any opaque pixels?
            {
                // DMA would degrtade performance here due to short line segments
                tft.setAddrWindow(pDraw->iX + x, y, iCount, 1);
                tft.pushPixels(usTemp, iCount);
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd) {
                c = *s++;
                if (c == ucTransparent)
                    x++;
                else
                    s--;
            }
        }
    } else {
        s = pDraw->pPixels;

        // Unroll the first pass to boost DMA performance
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        if (iWidth <= BUFFER_SIZE)
            for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
        else
            for (iCount = 0; iCount < BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA  // 71.6 fps (ST7796 84.5 fps)
        tft.dmaWait();
        tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
        tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
        dmaBuf = !dmaBuf;
#else  // 57.0 fps
        tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
        tft.pushPixels(&usTemp[0][0], iCount);
#endif

        iWidth -= iCount;
        // Loop if pixel buffer smaller than width
        while (iWidth > 0) {
            // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
            if (iWidth <= BUFFER_SIZE)
                for (iCount = 0; iCount < iWidth; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];
            else
                for (iCount = 0; iCount < BUFFER_SIZE; iCount++) usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA
            tft.dmaWait();
            tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
            dmaBuf = !dmaBuf;
#else
            tft.pushPixels(&usTemp[0][0], iCount);
#endif
            iWidth -= iCount;
        }
    }
} /* GIFDraw() */
