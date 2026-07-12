#define CLAY_IMPLEMENTATION
#include "surface.hpp"
#include "types.hpp"

#include <raylib.h>
#include <raymath.h>
#include "stdint.h"
#include <cstring>
#include "stdio.h"
#include "stdlib.h"

#include "config.h"

static inline Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    // Measure string size for Font
    Clay_Dimensions textSize = { 0 };

    float maxTextWidth = 0.0f;
    float lineTextWidth = 0;
    int maxLineCharCount = 0;
    int lineCharCount = 0;

    float textHeight = config->fontSize;
    Font* fonts = (Font*)userData;
    Font fontToUse = fonts[config->fontId];
    // Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
    // RayLib ships with a default font, so we can continue with that built in one. 
    if (!fontToUse.glyphs) {
        fontToUse = GetFontDefault();
    }

    float scaleFactor = config->fontSize/(float)fontToUse.baseSize;

    for (int i = 0; i < text.length; ++i, lineCharCount++)
    {
        if (text.chars[i] == '\n') {
            maxTextWidth = fmax(maxTextWidth, lineTextWidth);
            maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);
            lineTextWidth = 0;
            lineCharCount = 0;
            continue;
        }
        int index = text.chars[i] - 32;
        if (fontToUse.glyphs[index].advanceX != 0) lineTextWidth += fontToUse.glyphs[index].advanceX;
        else lineTextWidth += (fontToUse.recs[index].width + fontToUse.glyphs[index].offsetX);
    }

    maxTextWidth = fmax(maxTextWidth, lineTextWidth);
    maxLineCharCount = CLAY__MAX(maxLineCharCount, lineCharCount);

    textSize.width = maxTextWidth * scaleFactor + (lineCharCount * config->letterSpacing);
    textSize.height = textHeight;

    return textSize;
}

// Get a ray trace from the screen position (i.e mouse) within a specific section of the screen
Ray getScreenToWorldPointWithZDistance(Vector2 position, Camera camera, int screenWidth, int screenHeight, float zDistance)
{
    Ray ray = { 0 };

    // Calculate normalized device coordinates
    // NOTE: y value is negative
    float x = (2.0f*position.x)/(float)screenWidth - 1.0f;
    float y = 1.0f - (2.0f*position.y)/(float)screenHeight;
    float z = 1.0f;

    // Store values in a vector
    Vector3 deviceCoords = { x, y, z };

    // Calculate view matrix from camera look at
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);

    Matrix matProj = MatrixIdentity();

    if (camera.projection == CAMERA_PERSPECTIVE)
    {
        // Calculate projection matrix from perspective
        matProj = MatrixPerspective(camera.fovy*DEG2RAD, ((double)screenWidth/(double)screenHeight), 0.01f, zDistance);
    }
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        double aspect = (double)screenWidth/(double)screenHeight;
        double top = camera.fovy/2.0;
        double right = top*aspect;

        // Calculate projection matrix from orthographic
        matProj = MatrixOrtho(-right, right, -top, top, 0.01, 1000.0);
    }

    // Unproject far/near points
    Vector3 nearPoint = Vector3Unproject(Vector3({ deviceCoords.x, deviceCoords.y, 0.0f }), matProj, matView);
    Vector3 farPoint = Vector3Unproject(Vector3({ deviceCoords.x, deviceCoords.y, 1.0f }), matProj, matView);

    // Calculate normalized direction vector
    Vector3 direction = Vector3Normalize(Vector3Subtract(farPoint, nearPoint));

    ray.position = farPoint;

    // Apply calculated vectors to ray
    ray.direction = direction;

    return ray;
}

void Surface::load(){
    // 1. Query minimum memory required for default element limits
    uint64_t memorySize = Clay_MinMemorySize();
    // 2. Allocate memory (malloc, stack, or custom allocator)
    void* memory = malloc(memorySize);
    // 3. Create arena [clay.h:2150-2158]
    arena = Clay_CreateArenaWithCapacityAndMemory(memorySize, memory);
    // 4. Initialize Clay [clay.h:2186-2188]
    Clay_Initialize(arena, Clay_Dimensions({ window.widthf, window.heightf }), Clay_ErrorHandler({ .errorHandlerFunction = handleError, .userData = this }));

    fonts[0] = LoadFontEx(PATH_ASSET("RobotoMono-Medium.ttf"), 48, 0, 400);
    SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_BILINEAR);
    fonts[1] = LoadFontEx(PATH_ASSET("Roboto-Regular.ttf"), 32, 0, 400);
    SetTextureFilter(fonts[1].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    initOverlay();

    imgArrowUp = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_UP));
    imgArrowUpRight = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_UP_RIGHT));
    imgArrowDownRight = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_DOWN_RIGHT));
    imgArrowDown = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_DOWN));
    imgArrowDownLeft = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_DOWN_LEFT));
    imgArrowUpLeft = LoadTexture(PATH_ASSET(URI_IMAGE_ARROW_UP_LEFT));

    // parchmentTexture = LoadTexture(PATH_ASSET("parchment.png"));
    // monkTexture = LoadTexture(PATH_ASSET("monk.png"));
}

void Surface::renderNull(Clay_RenderCommandArray& renderCommands) const {
}

void Surface::renderRaylib(Clay_RenderCommandArray& renderCommands) const {
    for (int j = 0; j < renderCommands.length; j++)
    {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        Clay_BoundingBox boundingBox = {renderCommand->boundingBox.x, renderCommand->boundingBox.y, renderCommand->boundingBox.width, renderCommand->boundingBox.height};
        switch (renderCommand->commandType)
        {
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *textData = &renderCommand->renderData.text;
                Font fontToUse = fonts[textData->fontId];
    
                int strlen = textData->stringContents.length + 1;
    
                if(strlen > temp_render_buffer_len) {
                    // Grow the temp buffer if we need a larger string
                    if(temp_render_buffer) free(temp_render_buffer);
                    temp_render_buffer = (char *) malloc(strlen);
                    temp_render_buffer_len = strlen;
                }
    
                // Raylib uses standard C strings so isn't compatible with cheap slices, we need to clone the string to append null terminator
                memcpy(temp_render_buffer, textData->stringContents.chars, textData->stringContents.length);
                temp_render_buffer[textData->stringContents.length] = '\0';
                DrawTextEx(fontToUse, temp_render_buffer, Vector2({boundingBox.x, boundingBox.y}), static_cast<float>(textData->fontSize), static_cast<float>(textData->letterSpacing), CLAY_COLOR_TO_RAYLIB_COLOR(textData->textColor));
    
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                Texture2D imageTexture = *(Texture2D *)renderCommand->renderData.image.imageData;
                Clay_Color tintColor = renderCommand->renderData.image.backgroundColor;
                if (tintColor.r == 0 && tintColor.g == 0 && tintColor.b == 0 && tintColor.a == 0) {
                    tintColor = Clay_Color({ 255, 255, 255, 255 });
                }

                // if userData and imageData are both set, the userData has scrollBar info
                // to calculate the offset for scrolling
                float imageOffset = 0.0f;
                if (renderCommand->userData) {
                    float scrollY = static_cast<ScrollbarData*>(renderCommand->userData)->scrollY;
                    if (scrollY + boundingBox.height <= 0) {
                        imageOffset = static_cast<float>(static_cast<int>(scrollY) % static_cast<int>(boundingBox.height));
                    } else {
                        imageOffset = scrollY;
                    }
                }

                DrawTexturePro(
                    imageTexture,
                    Rectangle({ 0, 0, static_cast<float>(imageTexture.width), static_cast<float>(imageTexture.height) }),
                    Rectangle({boundingBox.x, boundingBox.y+imageOffset, boundingBox.width, boundingBox.height }),
                    Vector2({}),
                    0,
                    CLAY_COLOR_TO_RAYLIB_COLOR(tintColor));
                // if userData is set, it is the scrollbarData info to render the second wrapping image 
                if (renderCommand->userData) {
                    DrawTexturePro(
                        imageTexture,
                        Rectangle({ 0, 0, static_cast<float>(imageTexture.width), static_cast<float>(imageTexture.height) }),
                        Rectangle({boundingBox.x, boundingBox.y+boundingBox.height+imageOffset, boundingBox.width, boundingBox.height }),
                        Vector2({}),
                        0,
                        CLAY_COLOR_TO_RAYLIB_COLOR(tintColor));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                BeginScissorMode(static_cast<int>(roundf(boundingBox.x)), static_cast<int>(roundf(boundingBox.y)), static_cast<int>(roundf(boundingBox.width)), static_cast<int>(roundf(boundingBox.height)));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                EndScissorMode();
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_START: {
                // setColorOverlay(CLAY_COLOR_TO_RAYLIB_COLOR(renderCommand->renderData.overlayColor.color));
                Color color = CLAY_COLOR_TO_RAYLIB_COLOR(renderCommand->renderData.overlayColor.color);
			    float colorFloat[4] = {
			        static_cast<float>(color.r)/255.0f,
			        static_cast<float>(color.g)/255.0f,
			        static_cast<float>(color.b)/255.0f,
			        static_cast<float>(color.a)/255.0f,
			    };

			    SetShaderValue(overlayShader, colorLoc, colorFloat, SHADER_UNIFORM_VEC4);
			    BeginShaderMode(overlayShader);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_END: {
                // disableColorOverlay();
        		EndShaderMode();
        		break;
            }
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
                if (config->cornerRadius.topLeft > 0) {
                    float radius = (config->cornerRadius.topLeft * 2) / (float)((boundingBox.width > boundingBox.height) ? boundingBox.height : boundingBox.width);
                    DrawRectangleRounded(Rectangle({ boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height }), radius, 8, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                } else {
                    DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                Clay_BorderRenderData *config = &renderCommand->renderData.border;
                // Left border
                if (config->width.left > 0) {
                    DrawRectangleV(Vector2({ boundingBox.x, boundingBox.y + config->cornerRadius.topLeft }), Vector2({ static_cast<float>(config->width.left), boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft }), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Right border
                if (config->width.right > 0) {
                    DrawRectangleV(Vector2({ boundingBox.x + boundingBox.width - config->width.right, boundingBox.y + config->cornerRadius.topRight }), Vector2({ static_cast<float>(config->width.right), boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight }), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Top border
                if (config->width.top > 0) {
                    DrawRectangleV(Vector2({ boundingBox.x + config->cornerRadius.topLeft, boundingBox.y }), Vector2({ boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight, static_cast<float>(config->width.top) }), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                // Bottom border
                if (config->width.bottom > 0) {
                    DrawRectangleV(Vector2({ boundingBox.x + config->cornerRadius.bottomLeft, boundingBox.y + boundingBox.height - config->width.bottom }), Vector2({ boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight, static_cast<float>(config->width.bottom) }), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topLeft > 0) {
                    DrawRing(Vector2({ roundf(boundingBox.x + config->cornerRadius.topLeft), roundf(boundingBox.y + config->cornerRadius.topLeft) }), roundf(config->cornerRadius.topLeft - config->width.top), config->cornerRadius.topLeft, 180, 270, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.topRight > 0) {
                    DrawRing(Vector2({ roundf(boundingBox.x + boundingBox.width - config->cornerRadius.topRight), roundf(boundingBox.y + config->cornerRadius.topRight) }), roundf(config->cornerRadius.topRight - config->width.top), config->cornerRadius.topRight, 270, 360, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomLeft > 0) {
                    DrawRing(Vector2({ roundf(boundingBox.x + config->cornerRadius.bottomLeft), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft) }), roundf(config->cornerRadius.bottomLeft - config->width.bottom), config->cornerRadius.bottomLeft, 90, 180, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                if (config->cornerRadius.bottomRight > 0) {
                    DrawRing(Vector2({ roundf(boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight) }), roundf(config->cornerRadius.bottomRight - config->width.bottom), config->cornerRadius.bottomRight, 0.1, 90, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
                }
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                Clay_CustomRenderData *config = &renderCommand->renderData.custom;
                CustomLayoutElement *customElement = (CustomLayoutElement *)config->customData;
                if (!customElement) continue;
                switch (customElement->type) {
                    case CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL: {
                        Clay_BoundingBox rootBox = renderCommands.internalArray[0].boundingBox;
                        float scaleValue = CLAY__MIN(CLAY__MIN(1, 768 / rootBox.height) * CLAY__MAX(1, rootBox.width / 1024), 1.5f);
                        Ray positionRay = getScreenToWorldPointWithZDistance(Vector2({ renderCommand->boundingBox.x + renderCommand->boundingBox.width / 2, renderCommand->boundingBox.y + (renderCommand->boundingBox.height / 2) + 20 }), Raylib_camera, static_cast<int>(roundf(rootBox.width)), static_cast<int>(roundf(rootBox.height)), 140);
                        BeginMode3D(Raylib_camera);
                            DrawModel(customElement->customData.model.model, positionRay.position, customElement->customData.model.scale * scaleValue, WHITE);        // Draw 3d model with texture
                        EndMode3D();
                        break;
                    }
                    default: break;
                }
                break;
            }
            default: {
                printf("Error: unhandled render command.");
                exit(1);
            }
        }
    }
}

void Surface::initOverlay() {
#ifdef __EMSCRIPTEN__
    // GLSL ES 2.0 shader for WebGL 1.0 used by Emscripten for Web
    // const char* overlayShaderCode = "precision mediump float;\n"
    //                                 "\n"
    //                                 "varying vec2 fragTexCoord;\n"
    //                                 "varying vec4 fragColor;\n"
    //                                 "\n"
    //                                 "uniform sampler2D texture0;\n"
    //                                 "uniform vec4 overlayColor;\n"
    //                                 "\n"
    //                                 "void main()\n"
    //                                 "{\n"
    //                                 "    vec4 texelColor = texture2D(texture0, fragTexCoord) * fragColor;\n"
    //                                 "\n"
    //                                 "    vec3 blendedRGB = mix(texelColor.rgb, overlayColor.rgb, overlayColor.a);\n"
    //                                 "\n"
    //                                 "    gl_FragColor = vec4(blendedRGB, texelColor.a);\n"
    //                                 "}";
    // GLSL ES 3.0 shader for WebGL 2.0 used by Emscripten for Web
    const char* overlayShaderCode = 
        "#version 300 es\n"               // 1. Must be the absolute first line
        "precision mediump float;\n"
        "\n"
        "in vec2 fragTexCoord;\n"        // 2. 'varying' becomes 'in'
        "in vec4 fragColor;\n"           // 2. 'varying' becomes 'in'
        "\n"
        "uniform sampler2D texture0;\n"
        "uniform vec4 overlayColor;\n"
        "\n"
        "out vec4 finalColor;\n"         // 4. Declare a custom output variable
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 texelColor = texture(texture0, fragTexCoord) * fragColor;\n"
        "    vec3 blendedRGB = mix(texelColor.rgb, overlayColor.rgb, overlayColor.a);\n"
        "    finalColor = vec4(blendedRGB, texelColor.a);\n"
        "}";
#else
	const char* overlayShaderCode = "#version 330\n"
	                                "\n"
	                                "in vec2 fragTexCoord;\n"
	                                "in vec4 fragColor;\n"
	                                "\n"
	                                "uniform sampler2D texture0;\n"
	                                "uniform vec4 overlayColor;\n"
	                                "\n"
	                                "out vec4 finalColor;\n"
	                                "\n"
	                                "void main()\n"
	                                "{\n"
	                                "    vec4 texelColor = texture(texture0, fragTexCoord) * fragColor;\n"
	                                "\n"
	                                "    vec3 blendedRGB = mix(texelColor.rgb, overlayColor.rgb, overlayColor.a);\n"
	                                "\n"
	                                "    finalColor = vec4(blendedRGB, texelColor.a);\n"
	                                "}";
#endif
    overlayShader = LoadShaderFromMemory(0, overlayShaderCode);
    colorLoc = GetShaderLocation(overlayShader, "overlayColor");
}

// void Surface::setColorOverlay(Color color) const {
//     overlayEnabled = true;
//     float colorFloat[4] = {
//         static_cast<float>(color.r)/255.0f,
//         static_cast<float>(color.g)/255.0f,
//         static_cast<float>(color.b)/255.0f,
//         static_cast<float>(color.a)/255.0f,
//     };

//     SetShaderValue(overlayShader, colorLoc, colorFloat, SHADER_UNIFORM_VEC4);
//     BeginShaderMode(overlayShader);
// }

// void Surface::disableColorOverlay() const {
//     if (overlayEnabled) {
//         EndShaderMode();
//         overlayEnabled = false;
//     } else {
//     	TraceLog(LOG_INFO, "OVERLAY HIT");
//     }
// }

void handleButtonClick(Clay_ElementId elementId, Clay_PointerData pointerData, void* userData) {
    Surface* self = static_cast<Surface*>(userData);
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        std::string idStr(elementId.stringId.chars);
        auto result = self->buttonActions.find(idStr.c_str());

        Action::Surface action = Action::Surface::DO_NOTHING;
        if (result != self->buttonActions.end()) {
            action = result->second;
        } else {
            TraceLog(LOG_ERROR, "SURFACE ERROR: Button ID not found.");
        }

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    	self->buttonAction = action;
    }
}

void Surface::buttonSimple(const Clay_ElementId& elementId, const Clay_String& buttonText) {
	// Clay_Color bgColor = Clay_Hovered() ? RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(BLUE);

    CLAY(elementId, { 
        .layout = {
            .sizing = { 
                .width = CLAY_SIZING_GROW(0)
            },
            .padding = CLAY_PADDING_ALL(12),
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
        }, 
        .backgroundColor = Clay_Hovered() ? SURFACE_BUTTON_COLOR_BG_HL : SURFACE_BUTTON_COLOR_BG,
        .border = { 
            .color = Clay_Color({ 220, 220, 220, 255 }), 
            .width = CLAY_BORDER_OUTSIDE(1) 
        },
    }) {
        Clay_Color textColor = SURFACE_BUTTON_COLOR_FG;
        if (Clay_Hovered() && buttonHoverId != elementId.id) {
            buttonHoverId = elementId.id;
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            textColor = SURFACE_BUTTON_COLOR_FG_HL;
        }
    	Clay_OnHover(handleButtonClick, this);
        CLAY_TEXT(buttonText, CLAY_TEXT_CONFIG({ .textColor = textColor, .fontSize = 32 }));
    }
}

void Surface::buttonImage(const Clay_ElementId& elementId, Texture2D* imgData) {
    // Clay_Color bgColor = Clay_Hovered() ? RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(BLUE);

    CLAY(elementId, { 
        .layout = {
            .sizing = { 
                .width = CLAY_SIZING_FIXED(55),
                .height = CLAY_SIZING_FIXED(55)
            },
            // .padding = CLAY_PADDING_ALL(12),
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
        }, 
        // .backgroundColor = SURFACE_BUTTON_COLOR_BG,
        .image = { .imageData = imgData },
    }) {
        // Clay_Color textColor = SURFACE_BUTTON_COLOR_FG;
        // if (Clay_Hovered() && buttonHoverId != elementId.id) {
        //     buttonHoverId = elementId.id;
        //     SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        //     textColor = SURFACE_BUTTON_COLOR_FG_HL;
        // }
        Clay_OnHover(handleButtonClick, this);
    }
}

void handleButtonTabClick(Clay_ElementId elementId, Clay_PointerData pointerData, void* userData) {
    Surface* self = static_cast<Surface*>(userData);
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        self->activeTabId = elementId.id;
    }
}

void Surface::buttonTab(const Clay_ElementId& elementId, const Clay_String& buttonText) {
    CLAY(elementId, { 
        .layout = {
            .sizing = { 
                .width = CLAY_SIZING_GROW(0)
            },
            .padding = CLAY_PADDING_ALL(8),
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER } 
        }, 
        .backgroundColor = Clay_Hovered() ? RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(BLUE),
    }) {
        if (Clay_Hovered() && buttonHoverId != elementId.id) {
            buttonHoverId = elementId.id;
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
        }
        Clay_OnHover(handleButtonTabClick, this);
        CLAY_TEXT(buttonText, CLAY_TEXT_CONFIG({ .textColor = { 255, 255, 255, 255 }, .fontSize = 24 }));
    }
}

static Clay_TransitionData ExitSlideUp(Clay_TransitionData initialState, Clay_TransitionProperty properties) {
    Clay_TransitionData targetState = initialState;
    if (properties & CLAY_TRANSITION_PROPERTY_OVERLAY_COLOR) {
        targetState.overlayColor = Clay_Color({ 255, 255, 255, 200 });
    }
    if (properties & CLAY_TRANSITION_PROPERTY_HEIGHT) {
        // targetState.overlayColor = Clay_Color({ 255, 255, 255, 0 });
        targetState.boundingBox.height = 0.0f;
    }
    if (properties & CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR) {
        targetState.backgroundColor.a = 0.0f;
    }
    if (properties & CLAY_TRANSITION_PROPERTY_BORDER_COLOR) {
        targetState.borderColor.a = 0.0f;
    }
    return targetState;
}

// static Clay_TransitionData TransitionFadeOut(Clay_TransitionData initialState, Clay_TransitionProperty properties) {
//     Clay_TransitionData targetState = initialState;
//     if (properties & CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR) {
//         targetState.backgroundColor.a = 0.0f;
//     }
//     if (properties & CLAY_TRANSITION_PROPERTY_BORDER_COLOR) {
//         targetState.borderColor.a = 0.0f;
//     }
//     return targetState;
// }

Action::Surface Surface::updateNull(const InputEvent& inputEvent) {
    return Action::Surface::DO_NOTHING;
}

Action::Surface Surface::updateMenu(const InputEvent& inputEvent) {

    bool isMouseDown = inputEvent.id == Event::Input::PRIMARY || inputEvent.id == Event::Input::PRIMARY_DOWN;
    Clay_Vector2 mousePosition = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition());
    Clay_SetPointerState(mousePosition, isMouseDown && !scrollbarData.mouseDown);

    Vector2 mouseWheelDelta = GetMouseWheelMoveV();
    float mouseWheelX = mouseWheelDelta.x;
    float mouseWheelY = mouseWheelDelta.y;
    Clay_UpdateScrollContainers(true, Clay_Vector2({ mouseWheelX, mouseWheelY }), GetFrameTime());

    if (inputEvent.id == Event::Input::PRIMARY_UP) {
        scrollbarData.mouseDown = false;
    }

        Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("TabContent")));
    if (isMouseDown && !scrollbarData.mouseDown && Clay_PointerOver(Clay_GetElementId(CLAY_STRING("ScrollBar")))) {
        scrollbarData.clickOrigin = mousePosition;
        scrollbarData.positionOrigin = *scrollContainerData.scrollPosition;
        scrollbarData.mouseDown = true;

    } else if (scrollbarData.mouseDown) {
        // Clay_ScrollContainerData scrollContainerData = Clay_GetScrollContainerData(Clay_GetElementId(CLAY_STRING("TabContent")));
        if (scrollContainerData.contentDimensions.height > 0) {
            Clay_Vector2 ratio = Clay_Vector2({
                scrollContainerData.contentDimensions.width / scrollContainerData.scrollContainerDimensions.width,
                scrollContainerData.contentDimensions.height / scrollContainerData.scrollContainerDimensions.height,
            });

            if (scrollContainerData.config.vertical) {
                scrollContainerData.scrollPosition->y = scrollbarData.positionOrigin.y + (scrollbarData.clickOrigin.y - mousePosition.y) * ratio.y;
            }
            
            if (scrollContainerData.config.horizontal) {
                scrollContainerData.scrollPosition->x = scrollbarData.positionOrigin.x + (scrollbarData.clickOrigin.x - mousePosition.x) * ratio.x;
            }
        }
    }

    if(scrollContainerData.scrollPosition) {

        scrollbarData.scrollY = scrollContainerData.scrollPosition->y - scrollbarData.positionOrigin.y;
        // TraceLog(LOG_INFO, "scroll %f", scrollbarData.scrollY);
    }

    // Action::Surface lastButtonAction = buttonAction;
    // buttonAction = Action::Surface::DO_NOTHING;

    return buttonAction;
}

void Surface::menuNull() {
    buttonAction = Action::Surface::DO_NOTHING;
}

void Surface::menuPause() {
    CLAY(CLAY_ID("ContainerPauseMenu"), { 
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0), 
                .height = CLAY_SIZING_GROW(0) 
            }, 
        }, 
        .backgroundColor = Clay_Color({ 0, 0, 0, 0 })
    }) {
        Clay_ElementId contentPauseMenuId = CLAY_ID("ContentPauseMenu");
        CLAY(contentPauseMenuId, {
            .layout = { 
                .sizing = { 
                    .width = CLAY_SIZING_PERCENT(0.55f), 
                    .height = CLAY_SIZING_PERCENT(0.5f) 
                }, 
                .padding = { 16, 16, 16, 16 },
                .childGap = 2,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            .backgroundColor = SURFACE_MENU_COLOR_BG,
            .floating = { 
                .offset = {0, 0}, 
                .zIndex = 1, 
                .attachPoints = { 
                    CLAY_ATTACH_POINT_CENTER_CENTER, 
                    CLAY_ATTACH_POINT_CENTER_CENTER 
                }, 
                .attachTo = CLAY_ATTACH_TO_PARENT 
            },
            .transition = {
                .handler = Clay_EaseOut,
                .duration = 0.3f,
                .properties = static_cast<Clay_TransitionProperty>(CLAY_TRANSITION_PROPERTY_DIMENSIONS | CLAY_TRANSITION_PROPERTY_POSITION | CLAY_TRANSITION_PROPERTY_OVERLAY_COLOR | CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR),
                .enter = { .setInitialState = ExitSlideUp },
                .exit = { .setFinalState = ExitSlideUp },
            }
        }) {
            if (Clay_Hovered() && buttonHoverId != contentPauseMenuId.id) {
                buttonHoverId = contentPauseMenuId.id;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            if (surfaceEvent == Event::Surface::SHOW_RETURN_MAIN_MENU_CONFIRMATION) {
                CLAY(CLAY_ID("PauseMenuConfirmationDialogue"), {
                    .layout = { 
                        .sizing = { 
                            .width = CLAY_SIZING_GROW(0), 
                            .height = CLAY_SIZING_PERCENT(0.5f)
                        }, 
                        .padding = { 16, 16, 16, 16 },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    },
                    .backgroundColor = { 140, 80, 200, 200 },
                    .floating = { 
                        .offset = {0, 0}, 
                        .zIndex = 1, 
                        .attachPoints = { 
                            CLAY_ATTACH_POINT_CENTER_CENTER, 
                            CLAY_ATTACH_POINT_CENTER_CENTER 
                        }, 
                        .attachTo = CLAY_ATTACH_TO_PARENT 
                    },
                    .border = { 
                        .color = Clay_Color({80, 80, 80, 255}), 
                        .width = CLAY_BORDER_OUTSIDE(2) 
                    },
                    .transition = {
                        .handler = Clay_EaseOut,
                        .duration = 0.3f,
                        .properties = static_cast<Clay_TransitionProperty>(CLAY_TRANSITION_PROPERTY_DIMENSIONS | CLAY_TRANSITION_PROPERTY_POSITION | CLAY_TRANSITION_PROPERTY_OVERLAY_COLOR | CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR),
                        .enter = { .setInitialState = ExitSlideUp },
                        .exit = { .setFinalState = ExitSlideUp },
                    }
                }) {
                    CLAY(CLAY_ID("ContainerConfirmationText"), {
                        .layout = {
                            .sizing = { 
                                .width = CLAY_SIZING_GROW(0), 
                            },
                            .padding = { 20, 20, 20, 20 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                    }) {
                        CLAY_TEXT(CLAY_STRING("All progress will be lost. Return to Main Menu?"), CLAY_TEXT_CONFIG({ 
                            .textColor = Clay_Color({255,255,255,255}),
                            .fontSize = 24,
                        }));
                    }

                    CLAY(CLAY_ID("ContainerConfirmationButtons"), {
                        .layout = {
                            .sizing = { 
                                .width = CLAY_SIZING_GROW(0), 
                            },
                            .childGap = 20,
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER },
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        },
                    }) {
                        buttonSimple(CLAY_ID("ButtonConfirm"), CLAY_STRING("Yes"));
                        buttonSimple(CLAY_ID("ButtonCancel"), CLAY_STRING("No"));
                    }
                }
            }
            // NOTES: CLAY_TEXT does not have .transition property, text cannot animate transition
            // and as a result any fading on the parent leave the text unchange and looks jarring.
            // solution is to either add .transition to each text element in Clay, or allow the parent
            // to somehow force fade the children text nodes in it if a transition property is set.
            CLAY_TEXT(CLAY_STRING("Pause Menu"), CLAY_TEXT_CONFIG({ 
                .textColor = SURFACE_BUTTON_COLOR_FG,
                .fontSize = 24,
            }));

            buttonSimple(CLAY_ID("ButtonGameResume"), CLAY_STRING("Resume Game"));
            buttonSimple(CLAY_ID("ButtonGameRestart"), CLAY_STRING("Restart Game"));
            // buttonSimple(CLAY_ID("ButtonMainMenu"), CLAY_STRING("Main Menu"));
            buttonSimple(CLAY_ID("ButtonQuit"), CLAY_STRING("Quit"));
        }
    }

    buttonAction = Action::Surface::DO_NOTHING;
}

void Surface::menuMain() {
    CLAY(CLAY_ID("ContainerMainMenu"), { 
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0), 
                .height = CLAY_SIZING_GROW(0) 
            }, 
        }, 
        .backgroundColor = Clay_Color({ 0, 0, 0, 0 })
    }) {
        Clay_ElementId contentMainMenuId = CLAY_ID("ContentMainMenu");
        CLAY(contentMainMenuId, {
            .layout = { 
                .sizing = { 
                    .width = CLAY_SIZING_PERCENT(0.55f), 
                    .height = CLAY_SIZING_PERCENT(0.5f) 
                }, 
                .padding = { 16, 16, 16, 16 },
                .childGap = 2,
                .layoutDirection = CLAY_TOP_TO_BOTTOM 
            },
            .backgroundColor = SURFACE_MENU_COLOR_BG,
            .floating = { 
                .offset = {0, 0}, 
                .zIndex = 1, 
                .attachPoints = { 
                    CLAY_ATTACH_POINT_CENTER_CENTER, 
                    CLAY_ATTACH_POINT_CENTER_CENTER 
                }, 
                .attachTo = CLAY_ATTACH_TO_PARENT 
            },
            .transition = {
                .handler = Clay_EaseOut,
                .duration = 0.3f,
                .properties = static_cast<Clay_TransitionProperty>(CLAY_TRANSITION_PROPERTY_DIMENSIONS | CLAY_TRANSITION_PROPERTY_POSITION | CLAY_TRANSITION_PROPERTY_OVERLAY_COLOR | CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR),
                .enter = { .setInitialState = ExitSlideUp },
                .exit = { .setFinalState = ExitSlideUp },
            }
        }) {
            if (Clay_Hovered() && buttonHoverId != contentMainMenuId.id) {
                buttonHoverId = contentMainMenuId.id;
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }
            // NOTES: CLAY_TEXT does not have .transition property, text cannot animate transition
            // and as a result any fading on the parent leave the text unchange and looks jarring.
            // solution is to either add .transition to each text element in Clay, or allow the parent
            // to somehow force fade the children text nodes in it if a transition property is set.
            CLAY_TEXT(CLAY_STRING("Main Menu"), CLAY_TEXT_CONFIG({ 
                .textColor = SURFACE_BUTTON_COLOR_FG,
                .fontSize = 24,
            }));

            buttonSimple(CLAY_ID("ButtonGameNew"), CLAY_STRING("New Game"));
            buttonSimple(CLAY_ID("ButtonQuit"), CLAY_STRING("Quit"));
        }
    }

    buttonAction = Action::Surface::DO_NOTHING;
}

void Surface::displayUnit(GameState gameState) {}

void Surface::displayGame(GameState gameState) {
    CLAY(CLAY_ID("HUDContainer"), { 
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0), 
                .height = CLAY_SIZING_GROW(0) 
            }, 
            .padding = { 16, 16, 16, 16 }, 
            .childGap = 16
        }, 
        .backgroundColor = Clay_Color({ 200, 200, 200, 0 })
    }) {
        CLAY(CLAY_ID("LeftCounter"), {
            .layout = {
                .sizing = { 
                    .width = CLAY_SIZING_GROW(0), 
                },
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
        }) {            
            // std::string&& numClicks = std::format("COUNTER: {}", gameState.raylibLogoClicks);
            // const char* numClicksText = TextFormat("COUNTER: %d", 10);
            scoreText = "Score " + std::to_string(gameState.score);
            // Clay_String numClicksClayString = CLAY__INIT(Clay_String){ .isStaticallyAllocated = true, .length = static_cast<int32_t>(strlen(numClicksText)), .chars = numClicksText };
            Clay_String numClicksClayString = CLAY__INIT(Clay_String){ .isStaticallyAllocated = true, .length = static_cast<int32_t>(scoreText.length()), .chars = scoreText.c_str() };
            // CLAY_TEXT(Clay__IntToString(gameState.raylibLogoClicks), CLAY_TEXT_CONFIG({ 
            CLAY_TEXT(numClicksClayString, CLAY_TEXT_CONFIG({
                .textColor = Clay_Color({ 255, 255, 255, 255 }),
                .fontSize = 48,
            }));
        }


        if (gameState.state == State::Game::OVER || gameState.state == State::Game::WIN) {
            Clay_ElementId contentPauseMenuId = CLAY_ID("ContentGameOverModal");
            CLAY(contentPauseMenuId, {
                .layout = { 
                    .sizing = { 
                        .width = CLAY_SIZING_PERCENT(0.55f), 
                        .height = CLAY_SIZING_PERCENT(0.5f) 
                    }, 
                    .padding = { 16, 16, 16, 16 },
                    .childGap = 2,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                },
                .backgroundColor = SURFACE_MENU_COLOR_BG,
                .floating = { 
                    .offset = {0, 0}, 
                    .zIndex = 1, 
                    .attachPoints = { 
                        CLAY_ATTACH_POINT_CENTER_CENTER, 
                        CLAY_ATTACH_POINT_CENTER_CENTER 
                    }, 
                    .attachTo = CLAY_ATTACH_TO_PARENT 
                },
                .transition = {
                    .handler = Clay_EaseOut,
                    .duration = 0.3f,
                    .properties = static_cast<Clay_TransitionProperty>(CLAY_TRANSITION_PROPERTY_DIMENSIONS | CLAY_TRANSITION_PROPERTY_POSITION | CLAY_TRANSITION_PROPERTY_OVERLAY_COLOR | CLAY_TRANSITION_PROPERTY_BACKGROUND_COLOR),
                    .enter = { .setInitialState = ExitSlideUp },
                    .exit = { .setFinalState = ExitSlideUp },
                }
            }) {
                if (Clay_Hovered() && buttonHoverId != contentPauseMenuId.id) {
                    buttonHoverId = contentPauseMenuId.id;
                    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
                }

                if (gameState.state == State::Game::OVER) {

                    CLAY_TEXT(CLAY_STRING("GAME OVER"), CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 48,
                    }));

                    scoreText = "Score: " + std::to_string(gameState.score);
                    Clay_String numClicksClayString = CLAY__INIT(Clay_String){ .isStaticallyAllocated = true, .length = static_cast<int32_t>(scoreText.length()), .chars = scoreText.c_str() };

                    CLAY_TEXT(numClicksClayString, CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 24,
                    }));

                    CLAY_TEXT(CLAY_STRING("Thank you for playing!"), CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 24,
                    }));
                }


                if (gameState.state == State::Game::WIN) {
                    
                    CLAY_TEXT(CLAY_STRING("YOU WIN"), CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 48,
                    }));

                    scoreText = "Score: " + std::to_string(gameState.score);
                    Clay_String numClicksClayString = CLAY__INIT(Clay_String){ .isStaticallyAllocated = true, .length = static_cast<int32_t>(scoreText.length()), .chars = scoreText.c_str() };

                    CLAY_TEXT(numClicksClayString, CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 24,
                    }));

                    CLAY_TEXT(CLAY_STRING("Thank you for playing!"), CLAY_TEXT_CONFIG({ 
                        .textColor = SURFACE_BUTTON_COLOR_FG,
                        .fontSize = 24,
                    }));
                }

                buttonSimple(CLAY_ID("ButtonGameRestart"), CLAY_STRING("Restart Game"));
                buttonSimple(CLAY_ID("ButtonQuit"), CLAY_STRING("Quit"));
            }





            // END GAME OVER MODAL
        }


        // HUD

        CLAY(CLAY_ID("ControlHUD"), {
            .layout = {
                .sizing = { 
                    .width = CLAY_SIZING_FIXED(165), 
                    // .height = CLAY_SIZING_FIXED(110), 
                    .height = CLAY_SIZING_GROW(0), 
                },
                .childAlignment = { .x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_BOTTOM },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            // .backgroundColor = SURFACE_MENU_COLOR_BG,
            // .floating = { 
            //     .offset = {0, 0}, 
            //     .zIndex = 1, 
            //     .attachPoints = { 
            //         CLAY_ATTACH_POINT_CENTER_CENTER, 
            //         CLAY_ATTACH_POINT_CENTER_BOTTOM 
            //     }, 
            //     .attachTo = CLAY_ATTACH_TO_PARENT 
            // },
        }) {            

            CLAY(CLAY_ID("HUDTop"), {
                .layout = {
                    .sizing = { 
                        .width = CLAY_SIZING_FIXED(165), 
                        // .height = CLAY_SIZING_FIXED(110), 
                        .height = CLAY_SIZING_FIXED(55), 
                    },
                    // .childAlignment = { .x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_BOTTOM }
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                }
            }) {            

                buttonImage(CLAY_ID("ButtonHudUpLeft"), &imgArrowUpLeft);
                buttonImage(CLAY_ID("ButtonHudUp"), &imgArrowUp);
                buttonImage(CLAY_ID("ButtonHudUpRight"), &imgArrowUpRight);
            }

            CLAY(CLAY_ID("HUDBottom"), {
                .layout = {
                    .sizing = { 
                        .width = CLAY_SIZING_FIXED(165), 
                        // .height = CLAY_SIZING_FIXED(110), 
                        .height = CLAY_SIZING_FIXED(55), 
                    },
                    // .childAlignment = { .x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_BOTTOM }
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                }
            }) {            

                buttonImage(CLAY_ID("ButtonHudDownLeft"), &imgArrowDownLeft);
                buttonImage(CLAY_ID("ButtonHudDown"), &imgArrowDown);
                buttonImage(CLAY_ID("ButtonHudDownRight"), &imgArrowDownRight);
            }

        }

    }

    buttonAction = Action::Surface::DO_NOTHING;
}


void Surface::beginEvent(Event::Surface event) {
    surfaceEvent = event;
}

void Surface::clearEvent() {
    surfaceEvent = Event::Surface::NO_EVENT;
}

void Surface::handleError(Clay_ErrorData errorData) {
    TraceLog(LOG_INFO, "%s", errorData.errorText.chars);
    Surface* self = static_cast<Surface*>(errorData.userData);

    if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
        // reinitializeClay = true;
        Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    } else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
        // reinitializeClay = true;
        Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
    }

    uint64_t memorySize = Clay_MinMemorySize();
    void* memory = malloc(memorySize);
    self->arena = Clay_CreateArenaWithCapacityAndMemory(memorySize, memory);
    Clay_Initialize(self->arena, Clay_Dimensions({ self->window.widthf, self->window.heightf }), Clay_ErrorHandler({ .errorHandlerFunction = self->handleError, .userData = self }));
}

void Surface::resize(int width, int height) {
    // if (width < 720) {
    //     sidebarWidth = CLAY_SIZING_FIXED(150);
    // } else {
    //     sidebarWidth = CLAY_SIZING_PERCENT(0.2f);
    // }

	Clay_SetLayoutDimensions(Clay_Dimensions({ static_cast<float>(width), static_cast<float>(height) }));
}

void Surface::transition(State::App appState, State::Screen screen) {
    switch(screen) {
        case State::Screen::MAIN:
            menu = &Surface::menuMain;
            display = &Surface::displayUnit;
            update = &Surface::updateMenu;
            render = &Surface::renderRaylib;
            break;
        case State::Screen::GAME:
            render = &Surface::renderRaylib;
            switch(appState) {
                case State::App::PAUSE:
                    menu = &Surface::menuPause;
                    display = &Surface::displayUnit;
                    update = &Surface::updateMenu;
                    break;
                case State::App::RUN:
                    menu = &Surface::menuNull;
                    display = &Surface::displayGame;
                    update = &Surface::updateMenu;
                    break;
                default:
                    menu = &Surface::menuNull;
                    display = &Surface::displayUnit;
                    update = &Surface::updateNull;
            }

            break;
        default:
            menu = &Surface::menuNull;
            display = &Surface::displayUnit;
            update = &Surface::updateNull;
            render = &Surface::renderNull;
    };
}

void Surface::unload(){
    UnloadTexture(imgArrowUp);
    UnloadTexture(imgArrowUpRight);
    UnloadTexture(imgArrowDownRight);
    UnloadTexture(imgArrowDown);
    UnloadTexture(imgArrowDownLeft);
    UnloadTexture(imgArrowUpLeft);


    if(temp_render_buffer) free(temp_render_buffer);
    temp_render_buffer_len = 0;

    UnloadShader(overlayShader);
    UnloadFont(fonts[0]);
    UnloadFont(fonts[1]);

    free(arena.memory);

    // UnloadTexture(parchmentTexture);
    // UnloadTexture(profilePicture);
    // UnloadTexture(monkTexture);
}
