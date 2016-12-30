#ifndef __TWITTER_BASE_H__
#define __TWITTER_BASE_H__

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define kTWFeedURL "https://api.twitter.com/1.1/statuses/home_timeline.json"
#define kTWPostURL "https://api.twitter.com/1.1/statuses/update.json"

#ifdef __cplusplus
    #define TW_EXPORT extern "C"
#else /* C */
    #define TW_EXPORT extern
#endif /* C++ */

typedef struct {
    void *raw;
    size_t size;
} TWBlock;

typedef struct {
    char *raw;
    size_t length;
} TWString;

#endif /* __TWITTER_BASE_H__ */
