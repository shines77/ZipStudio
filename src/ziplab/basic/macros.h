
#ifndef ZIPLAB_BASIC_MARCOS_H
#define ZIPLAB_BASIC_MARCOS_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define ZIPLAB_DEFINED(X)           ((##X) && (##X != 0))

#define ZIPLAB_STRINGIFY(Text)      ZIPLAB_TO_STRING(Text)
#define ZIPLAB_TO_STRING(Text)      #Text

#define ZIPLAB_GLUE(a, b)           a ## b
#define ZIPLAB_JOIN(a, b)           ZIPLAB_GLUE(a, b)

#define ZIPLAB_TO_BYTE(n)           ((n) & 0xFFu)
#define ZIPLAB_TO_WORD(n)           ((n) & 0xFFFFu)
#define ZIPLAB_TO_DWORD(n)          ((n) & 0xFFFFFFFFu)

#define ZIPLAB_TO_INT(a, b, c, d) \
                                  ((ZIPLAB_TO_BYTE(a) << 24) | (ZIPLAB_TO_BYTE(b) << 16) | \
                                   (ZIPLAB_TO_BYTE(c) <<  8) | (ZIPLAB_TO_BYTE(d) <<  0))

#define ZIPLAB_TO_INT2(a, b)        ZIPLAB_TO_INT(0, 0, a, b)
#define ZIPLAB_TO_INT3(a, b, c)     ZIPLAB_TO_INT(0, a, b, c)

#define ZIPLAB_TO_VER3(a, b, c)   ((ZIPLAB_TO_BYTE(a) << 24) | (ZIPLAB_TO_BYTE(b) << 16) | \
                                   (ZIPLAB_TO_WORD(c) <<  0))

#define ZIPLAB_TO_VER4(a, b, c, d) \
                                    ZIPLAB_TO_INT(a, b, c, d)

#define ZIPLAB_TO_DOT2(a, b, c)     a ##.## b
#define ZIPLAB_TO_DOT3(a, b, c)     a ##.## b ##.## c
#define ZIPLAB_TO_DOT4(a, b, c, d) \
                                    a ##.## b ##.## c ##.## d

//                                  0 ~ 8191 (9999)
#define ZIPLAB_TO_DEC(n)            ((n) & 0x1FFFu)

//                                  0 ~ 99
#define ZIPLAB_TO_DEC1(n)           ((n) & 0x7Fu)
#define ZIPLAB_TO_DEC2(n)           (((n) & 0x7Fu) * 100)
#define ZIPLAB_TO_DEC3(n)           (((n) & 0x7Fu) * 10000)
#define ZIPLAB_TO_DEC4(n)           (((n) & 0x7Fu) * 1000000)
#define ZIPLAB_TO_DEC5(n)           (((n) & 0x7Fu) * 100000000)

#define ZIPLAB_MAKE_DEC4(a, b, c, d) \
                                   (ZIPLAB_TO_DEC4(a) + ZIPLAB_TO_DEC3(b) + \
                                    ZIPLAB_TO_DEC2(c) + ZIPLAB_TO_DEC1(d))
#define ZIPLAB_MAKE_DEC4_(a, b, c, d) \
                                   (ZIPLAB_TO_DEC5(a) + ZIPLAB_TO_DEC4(b) + \
                                    ZIPLAB_TO_DEC3(c) + ZIPLAB_TO_DEC (d))

#define ZIPLAB_MAKE_DEC2(a, b)      ZIPLAB_MAKE_DEC4(0, 0, a, b)

#define ZIPLAB_MAKE_DEC3(a, b, c)   ZIPLAB_MAKE_DEC4(0, a, b, c)

#define ZIPLAB_MAKE_DEC3_(a, b, c) \
                                    ZIPLAB_MAKE_DEC4_(0, a, b, c)

#endif // ZIPLAB_BASIC_MARCOS_H
