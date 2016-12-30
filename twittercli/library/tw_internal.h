#ifndef __TW_INTERNAL__
#define __TW_INTERNAL__ 1

#include "../oauth/oauth.h"

#include <curl/curl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>

#define TW_PRIVATE __attribute__((visibility("hidden")))

#define kTWDataDirectoryLength strlen(kTWDataDirectoryName)
#define kTWDataDirectoryName   ".twitter"
#define kTWKeyConsumerSecret   "consumersecret"
#define kTWKeyConsumerToken    "consumerkey"
#define kTWKeyUserSecret       "usersecret"
#define kTWKeyUserToken        "usertoken"
#define kTWPrefix              "libTwitter: "
#define kTWBufferSize          2048

#if DEBUG
    #define debug(x) x
    #define dlog(m, ...) printf(kTWPrefix m "\n", __VA_ARGS__);
#else /* RELEASE */
    #define debug(x)
    #define dlog(m, ...)
#endif /* DEBUG */

TW_PRIVATE void TWKeyStoreInitialize();

#endif /* __TW_INTERNAL__ */
