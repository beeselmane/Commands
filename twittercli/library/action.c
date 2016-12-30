#include "tw_internal.h"
#include "action.h"
#include "http.h"

char *TWGetResponseKey(char *key, void *data)
{
    char *key_loc = strstr(data, key);
    if (!key_loc) return NULL;
    
    char *head = strstr(key_loc, ":\"") + 2;
    char *tail = strchr(head, '\"');
    if (!tail) return NULL;
    
    size_t size = tail - head;
    char *buffer = calloc(size + 1, sizeof(char));
    strncpy(buffer, head, size);
    buffer[size] = 0;
    return buffer;
}

void *TWGetFeed()
{
    return TWHttpPerformGet(kTWFeedURL);
}

void *TWPostTweet(char *tweet)
{
    char buffer[kTWBufferSize];
    sprintf(buffer, "status=%s", tweet);
    return TWHttpPerformPost(kTWPostURL, oauth_url_escape(buffer));
}
