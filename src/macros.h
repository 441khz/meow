#ifndef MACROS_H
#define MACROS_H
// shitty hack for clipping against the size of the ti89 screen.
#define INLINE_NOCLIP                                                                                   \
  &(SCR_RECT) {{0, 0, 160, 100}}

/**
 * @todo better error handling for debug
 */
#ifndef NDEBUG
#define _STOP()                                                                                         \
  {                                                                                                     \
    GrayOff();                                                                                          \
    FontSetSys(F_8x10);                                                                                 \
    MoveTo(0, 0);                                                                                       \
    printf("STOP!\n%s\nERR-L%d", __func__, __LINE__);                                                   \
    while (1)                                                                                           \
      ;                                                                                                 \
  }
#define _ASSERT(cond)                                                                                   \
  {                                                                                                     \
    if (!(cond)) {                                                                                      \
      _STOP();                                                                                          \
    }                                                                                                   \
  }
#else
#define _STOP(code) ((void)0)
#define _ASSERT(cond) ((void)0)
#endif

/**
 * @brief Sprite 8 Grey internal function
 * @param[dark] - dark plane data
 * @param[light] - light plane data
 */
#define _Sprite8Grey(x, y, h, dark, light, attr)                                                        \
  {                                                                                                     \
    Sprite8(x, y, h, dark, GetPlane(DARK_PLANE), attr);                                                 \
    Sprite8(x, y, h, light, GetPlane(LIGHT_PLANE), attr);                                               \
  }

/**
 * @brief Sprite 8 Assume Monochrome internal function
 * Just draw the data on both planes as is
 */
#define _Sprite8AM(x, y, h, data, attr)                                                                 \
  {                                                                                                     \
    Sprite8(x, y, h, data, GetPlane(DARK_PLANE), attr);                                                 \
    Sprite8(x, y, h, data, GetPlane(LIGHT_PLANE), attr);                                                \
  }

/**
 * @brief Sprite 8 Grey Assume Linear internal function.
 * Assume the dark plane comes first and is precisely `sizeof(uint8_t[8])` [i.e: 64 bytes] wide.
 * Then assume the dark plane is contiguously after that, same size.
 * @param[data] - ptr to `uint8_t[8][2]` where `[0]=<dark plane data>`, `[1]=<light plane data>`.
 */
#define _Sprite8GreyAL(x, y, h, data, attr)                                                             \
  {                                                                                                     \
    Sprite8(x, y, h, data, GetPlane(DARK_PLANE), attr);                                                 \
    Sprite8(x, y, h, ((uint8_t *)data + (sizeof(uint8_t[8]))), GetPlane(LIGHT_PLANE), attr);            \
  }

#define DrawStrMono(x, y, str, attr)                                                                    \
  {                                                                                                     \
    SetPlane(LIGHT_PLANE);                                                                              \
    DrawStr(x, y, str, attr);                                                                           \
    SetPlane(DARK_PLANE);                                                                               \
    DrawStr(x, y, str, attr);                                                                           \
  }

#define DrawCharMono(x, y, str, attr)                                                                   \
  {                                                                                                     \
    SetPlane(LIGHT_PLANE);                                                                              \
    DrawChar(x, y, str, attr);                                                                          \
    SetPlane(DARK_PLANE);                                                                               \
    DrawChar(x, y, str, attr);                                                                          \
  }

#define ClrScrGrey()                                                                                    \
  {                                                                                                     \
    SetPlane(LIGHT_PLANE);                                                                              \
    ClrScr();                                                                                           \
    SetPlane(DARK_PLANE);                                                                               \
    ClrScr();                                                                                           \
  }

/**
 * @brief This hack lets us maintain the same call stack level rather than nesting deeper.
 */
#define HandleMenuSimple()                                                                              \
  {{menu_item_t *__tmp_mm_ptr = StartMenuManager();                                                     \
  if (__tmp_mm_ptr) {                                                                                   \
    __tmp_mm_ptr->callback(__tmp_mm_ptr->opaque);                                                       \
  }                                                                                                     \
  }                                                                                                     \
  }

#endif
