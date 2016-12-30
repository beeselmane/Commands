#include "tw_internal.h"
#include "keystore.h"

#ifndef _KEYSTORE_DEBUG_
    #undef debug
    #undef dlog

    #define debug(x)
    #define dlog(m, ...)
#endif /* _KEYSTORE_DEBUG_ */

static char *TWKeyStorePath;

void TWKeyStoreInitialize()
{
    const char *user_home = getenv("HOME");
    if (!user_home) user_home = getpwuid(getuid())->pw_dir;
    TWKeyStorePath = calloc(strlen(user_home) + kTWDataDirectoryLength + 3, sizeof(char));
    sprintf(TWKeyStorePath, "%s/%s/", user_home, kTWDataDirectoryName);

    if (access(TWKeyStorePath, F_OK | R_OK | W_OK | X_OK))
        if (mkdir(TWKeyStorePath, S_IRWXU))
            perror("mkdir");

    dlog("Data Directory Initialized to %s", TWKeyStorePath);
}

TWString *TWKeyStoreGetKey(const char *name)
{
    char fullpath[kTWBufferSize];
    sprintf(fullpath, "%s%s", TWKeyStorePath, name);
    dlog("Getting key %s [%s]", name, fullpath);
    FILE *fp = fopen(fullpath, "rb");

    struct stat stats;
    stat(fullpath, &stats);

    if (!fp)
    {
        dlog("Error opening file %s", fullpath)
        return NULL;
    }

    TWString *string = calloc(1, sizeof(TWString));
    string->raw = malloc(stats.st_size + sizeof(char));
    fgets(string->raw, (int)stats.st_size, fp);
    string->length = stats.st_size;
    fclose(fp);

    dlog("Got value %s for key %s", string->raw, name)
    return string;
}

TWString *TWKeyStoreGetConsumerSecret()
{
    return TWKeyStoreGetKey(kTWKeyConsumerSecret);
}

TWString *TWKeyStoreGetConsumerToken()
{
    return TWKeyStoreGetKey(kTWKeyConsumerToken);
}

TWString *TWKeyStoreGetUserSecret()
{
    return TWKeyStoreGetKey(kTWKeyUserSecret);
}

TWString *TWKeyStoreGetUserToken()
{
    return TWKeyStoreGetKey(kTWKeyUserToken);
}

void TWKeyStoreSetKey(const char *name, const char *value)
{
    char fullpath[kTWBufferSize];
    sprintf(fullpath, "%s%s", TWKeyStorePath, name);
    dlog("Setting key %s [%s] to %s", name, fullpath, value);
    FILE *fp = fopen(fullpath, "wb");

    if (!fp)
    {
        dlog("Error opening file %s!", fullpath);
        return;
    }

    fwrite(value, sizeof(char), strlen(value), fp);
    fclose(fp);
}

void TWKeyStoreSetConsumerSecret(const char *value)
{
    TWKeyStoreSetKey(kTWKeyConsumerSecret, value);
}

void TWKeyStoreSetConsumerToken(const char *value)
{
    TWKeyStoreSetKey(kTWKeyConsumerToken, value);
}

void TWKeyStoreSetUserSecret(const char *value)
{
    TWKeyStoreSetKey(kTWKeyUserSecret, value);
}

void TWKeyStoreSetUserToken(const char *value)
{
    TWKeyStoreSetKey(kTWKeyUserToken, value);
}
