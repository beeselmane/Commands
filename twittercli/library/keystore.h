#ifndef __TWITTER_KEYSTORE_H__
#define __TWITTER_KEYSTORE_H__

#include "base.h"

TW_EXPORT TWString *TWKeyStoreGetKey(const char *name);
TW_EXPORT TWString *TWKeyStoreGetConsumerSecret();
TW_EXPORT TWString *TWKeyStoreGetConsumerToken();
TW_EXPORT TWString *TWKeyStoreGetUserSecret();
TW_EXPORT TWString *TWKeyStoreGetUserToken();

TW_EXPORT void TWKeyStoreSetKey(const char *name, const char *value);
TW_EXPORT void TWKeyStoreSetConsumerSecret(const char *value);
TW_EXPORT void TWKeyStoreSetConsumerToken(const char *value);
TW_EXPORT void TWKeyStoreSetUserSecret(const char *value);
TW_EXPORT void TWKeyStoreSetUserToken(const char *value);

#endif /* defined(__TWITTER_KEYSTORE_H__) */
