/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "thingtypedat.h"
#include "spritemanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/image.h>
#include <framework/graphics/texturemanager.h>
#include <framework/core/filestream.h>

ThingTypeDat::ThingTypeDat()
{
    m_category = DatInvalidCategory;
    m_id = 0;
    m_exactSize = 0;
    m_layers = 0;
    m_numPatternX = 0;
    m_numPatternY = 0;
    m_numPatternZ = 0;
    m_animationPhases = 0;
    m_groundSpeed = 0;
    m_maxTextLenght = 0;
    m_lightLevel = 0;
    m_lightColor = 0;
    m_miniMapColor = 0;
    m_lensHelp = 0;
    m_clothSlot = 0;
    m_elevation = 0;
}

void ThingTypeDat::unserialize(uint16 clientId, DatCategory category, const FileStreamPtr& fin)
{
    m_null = false;
    m_id = clientId;
    m_category = category;

    bool done = false;
    for(int i = 0 ; i < DatLastAttrib;++i) {
        int property = fin->getU8();
        if(property == DatLastAttrib) {
            done = true;
            break;
        }

        switch(property) {
            case DatAttribIsGround:
                m_isGround = true;
                m_groundSpeed = fin->getU16();
                if(m_groundSpeed == 0)
                    m_groundSpeed = 100;
                break;
            case DatAttribIsGroundBorder:
                m_isGroundBorder = true;
                break;
            case DatAttribIsOnBottom:
                m_isOnBottom = true;
                break;
            case DatAttribIsOnTop:
                m_isOnTop = true;
                break;
            case DatAttribIsContainer:
                m_isContainer = true;
                break;
            case DatAttribIsStackable:
                m_isStackable = true;
                break;
            case DatAttribIsForceUse:
                m_isForceUse = true;
                break;
            case DatAttribIsMultiUse:
                m_isMultiUse = true;
                break;
            case DatAttribIsWritable:
                m_isWritable = true;
                m_maxTextLenght = fin->getU16();
                break;
            case DatAttribIsWritableOnce:
                m_isWritableOnce = true;
                m_maxTextLenght = fin->getU16();
                break;
            case DatAttribIsFluidContainer:
                m_isFluidContainer = true;
                break;
            case DatAttribIsFluid:
                m_isFluid = true;
                break;
            case DatAttribIsNotWalkable:
                m_isNotWalkable = true;
                break;
            case DatAttribIsNotMoveable:
                m_isNotMoveable = true;
                break;
            case DatAttribBlockProjectile:
                m_blockProjectile = true;
                break;
            case DatAttribIsNotPathable:
                m_isNotPathable = true;
                break;
            case DatAttribIsPickupable:
                m_isPickupable = true;
                break;
            case DatAttribIsHangable:
                m_isHangable = true;
                break;
            case DatAttribHookSouth:
                m_isHookSouth = true;
                break;
            case DatAttribHookEast:
                m_isHookEast = true;
                break;
            case DatAttribIsRotateable:
                m_isRotateable = true;
                break;
            case DatAttribHasLight:
                m_hasLight = true;
                m_lightLevel = fin->getU16();
                m_lightColor = fin->getU16();
                break;
            case DatAttribDontHide:
                m_isDontHide = true;
                break;
            case DatAttribIsTranslucent:
                m_isTranslucent = true;
                break;
            case DatAttribHasDisplacement:
                m_hasDisplacement = true;
                m_displacement = Point(fin->getU16(), fin->getU16());
                break;
            case DatAttribHasElevation:
                m_hasElevation = true;
                m_elevation = fin->getU16();
                break;
            case DatAttribIsLyingCorpse:
                m_isLyingCorpse = true;
                break;
            case DatAttribAnimateAlways:
                m_isAnimateAlways = true;
                break;
            case DatAttribMiniMapColor:
                m_miniMapColor = true;
                m_miniMapColor = fin->getU16();
                break;
            case DatAttribLensHelp:
                m_lensHelp = true;
                m_lensHelp = fin->getU16();
                break;
            case DatAttribIsFullGround:
                m_isFullGround = true;
                break;
            case DatAttribIgnoreLook:
                m_isIgnoreLook = true;
                break;
            case DatAttribCloth:
                m_isCloth = true;
                m_clothSlot = fin->getU16();
                break;
            case DatAttribMarket:
                fin->getU16(); // category
                fin->getU16(); // trade as
                fin->getU16(); // show as
                fin->getString(); // name
                fin->getU16(); // restrict profession
                fin->getU16(); // level
                break;
            default:
                stdext::throw_exception("corrupt data, invalid type attribute");
                break;
        };
    }

    if(!done)
        stdext::throw_exception("corrupt data");

    int totalSprites = 1;
    for(int i = 0; i < DatLastDimension; ++i) {
        switch(i) {
            case DatWidth:
                m_size.setWidth(fin->getU8());
                break;
            case DatHeight:
                m_size.setHeight(fin->getU8());
                break;
            case DatExactSize:
                if(m_size.width() <= 1 && m_size.height() <= 1)
                    m_exactSize = 32;
                else
                    m_exactSize = std::min((int)fin->getU8(), std::max(m_size.width() * 32, m_size.height() * 32));
                break;
            case DatLayers:
                m_layers = fin->getU8();
                break;
            case DatPatternX:
                m_numPatternX = fin->getU8();
                break;
            case DatPatternY:
                m_numPatternY = fin->getU8();
                break;
            case DatPatternZ:
                m_numPatternZ = fin->getU8();
                break;
            case DatAnimationPhases:
                m_animationPhases = fin->getU8();
                break;
        }
    }

    totalSprites = m_size.width() * m_size.height() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * m_animationPhases;
    if(totalSprites == 0)
        stdext::throw_exception("a thing type has no sprites");
    if(totalSprites > 4096)
        stdext::throw_exception("a thing type has more than 4096 sprites");

    m_spritesIndex.resize(totalSprites);
    for(int i = 0; i < totalSprites; i++)
        m_spritesIndex[i] = fin->getU16();

    m_textures.resize(m_animationPhases);
    m_texturesFramesRects.resize(m_animationPhases);
    m_texturesFramesOriginRects.resize(m_animationPhases);
    m_texturesFramesOffsets.resize(m_animationPhases);
}

void ThingTypeDat::draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    if(m_null)
        return;

    const TexturePtr& texture = getTexture(animationPhase); // texture might not exists, neither its rects.

    int frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    Point textureOffset;
    Rect textureRect;

    if(scaleFactor != 1.0f) {
        textureRect = m_texturesFramesOriginRects[animationPhase][frameIndex];
    } else {
        textureOffset = m_texturesFramesOffsets[animationPhase][frameIndex];
        textureRect = m_texturesFramesRects[animationPhase][frameIndex];
    }

    Rect screenRect(dest + (textureOffset - m_displacement - (m_size.toPoint() - Point(1, 1)) * 32) * scaleFactor,
                    textureRect.size() * scaleFactor);

    g_painter->drawTexturedRect(screenRect, texture, textureRect);
}

const TexturePtr& ThingTypeDat::getTexture(int animationPhase)
{
    TexturePtr& animationPhaseTexture = m_textures[animationPhase];
    if(!animationPhaseTexture) {
        // we don't need layers in common items, they will be pre-drawn
        int textureLayers = 1;
        int numLayers = m_layers;
        if(m_category == DatCreatureCategory && numLayers >= 2) {
             // 5 layers: outfit base, red mask, green mask, blue mask, yellow mask
            textureLayers = 5;
            numLayers = 5;
        }

        int indexSize = textureLayers * m_numPatternX * m_numPatternY * m_numPatternZ;
        Size textureSize = getBestTextureDimension(m_size.width(), m_size.height(), indexSize);
        ImagePtr fullImage = ImagePtr(new Image(textureSize * Otc::TILE_PIXELS));

        m_texturesFramesRects[animationPhase].resize(indexSize);
        m_texturesFramesOriginRects[animationPhase].resize(indexSize);
        m_texturesFramesOffsets[animationPhase].resize(indexSize);

        for(int z = 0; z < m_numPatternZ; ++z) {
            for(int y = 0; y < m_numPatternY; ++y) {
                for(int x = 0; x < m_numPatternX; ++x) {
                    for(int l = 0; l < numLayers; ++l) {
                        bool spriteMask = (m_category == DatCreatureCategory && l > 0);
                        int frameIndex = getTextureIndex(l % textureLayers, x, y, z);
                        Point framePos = Point(frameIndex % (textureSize.width() / m_size.width()) * m_size.width(),
                                               frameIndex / (textureSize.width() / m_size.width()) * m_size.height()) * Otc::TILE_PIXELS;

                        for(int h = 0; h < m_size.height(); ++h) {
                            for(int w = 0; w < m_size.width(); ++w) {
                                uint spriteIndex = getSpriteIndex(w, h, spriteMask ? 1 : l, x, y, z, animationPhase);
                                ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);
                                if(spriteImage) {
                                    if(spriteMask) {
                                        static Color maskColors[] = { Color::red, Color::green, Color::blue, Color::yellow };
                                        spriteImage->overwriteMask(maskColors[l - 1]);
                                    }
                                    Point spritePos = Point(m_size.width()  - w - 1,
                                                            m_size.height() - h - 1) * Otc::TILE_PIXELS;

                                    fullImage->blit(framePos + spritePos, spriteImage);
                                }
                            }
                        }

                        Rect drawRect(framePos + Point(m_size.width(), m_size.height()) * Otc::TILE_PIXELS - Point(1,1), framePos);
                        for(int x = framePos.x; x < framePos.x + m_size.width() * Otc::TILE_PIXELS; ++x) {
                            for(int y = framePos.y; y < framePos.y + m_size.height() * Otc::TILE_PIXELS; ++y) {
                                uint8 *p = fullImage->getPixel(x,y);
                                if(p[3] != 0x00) {
                                    drawRect.setTop   (std::min(y, (int)drawRect.top()));
                                    drawRect.setLeft  (std::min(x, (int)drawRect.left()));
                                    drawRect.setBottom(std::max(y, (int)drawRect.bottom()));
                                    drawRect.setRight (std::max(x, (int)drawRect.right()));
                                }
                            }
                        }

                        m_texturesFramesRects[animationPhase][frameIndex] = drawRect;
                        m_texturesFramesOriginRects[animationPhase][frameIndex] = Rect(framePos, Size(m_size.width(), m_size.height()) * Otc::TILE_PIXELS);
                        m_texturesFramesOffsets[animationPhase][frameIndex] = drawRect.topLeft() - framePos;
                    }
                }
            }
        }
        animationPhaseTexture = TexturePtr(new Texture(fullImage, true));
        animationPhaseTexture->setSmooth(true);
    }
    return animationPhaseTexture;
}

Size ThingTypeDat::getBestTextureDimension(int w, int h, int count)
{
    const int MAX = 32;

    int k = 1;
    while(k < w)
        k<<=1;
    w = k;

    k = 1;
    while(k < h)
        k<<=1;
    h = k;

    int numSprites = w*h*count;
    assert(numSprites <= MAX*MAX);
    assert(w <= MAX);
    assert(h <= MAX);

    Size bestDimension = Size(MAX, MAX);
    for(int i=w;i<=MAX;i<<=1) {
        for(int j=h;j<=MAX;j<<=1) {
            Size candidateDimension = Size(i, j);
            if(candidateDimension.area() < numSprites)
                continue;
            if((candidateDimension.area() < bestDimension.area()) ||
               (candidateDimension.area() == bestDimension.area() && candidateDimension.width() + candidateDimension.height() < bestDimension.width() + bestDimension.height()))
                bestDimension = candidateDimension;
        }
    }

    return bestDimension;
}

uint ThingTypeDat::getSpriteIndex(int w, int h, int l, int x, int y, int z, int a) {
    uint index =
        ((((((a % m_animationPhases)
        * m_numPatternZ + z)
        * m_numPatternY + y)
        * m_numPatternX + x)
        * m_layers + l)
        * m_size.height() + h)
        * m_size.width() + w;
    assert(index < m_spritesIndex.size());
    return index;
}

uint ThingTypeDat::getTextureIndex(int l, int x, int y, int z) {
    return ((l * m_numPatternZ + z)
               * m_numPatternY + y)
               * m_numPatternX + x;
}
