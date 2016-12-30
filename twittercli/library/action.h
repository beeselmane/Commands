#ifndef __TWITTER_ACTION_H__
#define __TWITTER_ACTION_H__

#include "base.h"

TW_EXPORT void *TWGetFeed();
TW_EXPORT void *TWPostTweet(char *tweet);
TW_EXPORT char *TWGetResponseKey(char *key, void *data);

#endif /* __TWITTER_ACTION_H__ */
