#ifndef __TWITTER_HTTP_H__
#define __TWITTER_HTTP_H__ 1

#include "base.h"

TW_EXPORT TWBlock *TWHttpPerformGet(const char *url);
TW_EXPORT TWBlock *TWHttpPerformPost(const char *url, const char *vars);

#endif /* defined(__TWITTER_HTTP_H__) */
