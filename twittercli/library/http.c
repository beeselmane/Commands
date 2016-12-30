#include "tw_internal.h"
#include "keystore.h"
#include "http.h"

TW_PRIVATE static size_t TWDataCallback(void *data, size_t count, size_t esize, TWBlock *block)
{
    size_t size = (count * esize);

    if (!block->raw) block->raw = malloc(size);
    else block->raw = realloc(block->raw, block->size + size);

    if (!block->raw)
    {
        dlog("Allocation Error! [Size: %lu]", block->size + size);
        return(0);
    }

    void *copyaddr = block->raw + block->size;
    memccpy(copyaddr, data, (int)count, esize);
    block->size += size;
    return size;
}

TWBlock *TWHttpPerformPost(const char *url, const char *vars)
{
    dlog("Performing HTTP POST to URL %s with vars %s", url, vars);
    TWBlock *block = malloc(sizeof(TWBlock));
    memset(block, 0, sizeof(TWBlock));

    TWString *oauth_tokens[4] = {NULL, NULL, NULL, NULL};
    oauth_tokens[0] = TWKeyStoreGetConsumerToken();
    oauth_tokens[1] = TWKeyStoreGetConsumerSecret();
    oauth_tokens[2] = TWKeyStoreGetUserToken();
    oauth_tokens[3] = TWKeyStoreGetUserSecret();
    
    for (int i = 0; i < 4; i++)
    {
        if (!oauth_tokens[i]) return NULL;
        dlog("Have OAuth Token: %s", oauth_tokens[i]->raw);
    }

    char *full_url = calloc(strlen(url) + strlen(vars) + 2, sizeof(char));
    sprintf(full_url, "%s?%s", url, vars);
    
    char **args = NULL;
    int arg_count = oauth_split_url_parameters(full_url, &args);

    char *signed_url = oauth_sign_array2(&arg_count, &args, NULL, OA_HMAC, "POST", oauth_tokens[0]->raw, oauth_tokens[1]->raw, oauth_tokens[2]->raw, oauth_tokens[3]->raw);
    char *auth_parameters = oauth_serialize_url_sep(arg_count, 1, args, ", ", 6);
    char *other_parameters = oauth_serialize_url_sep(arg_count, 1, args, "", 1);
    struct curl_slist *slist = NULL;
    char header[kTWBufferSize];

    sprintf(header, "Authorization: OAuth %s", auth_parameters);
    curl_slist_append(slist, header);
    free(auth_parameters);

    char *final_url = calloc(strlen(url) + strlen(other_parameters), sizeof(char));
    sprintf(final_url, "%s", url);

    for (int i = 0; i < 4; i++)
    {
        free(oauth_tokens[i]->raw);
        free(oauth_tokens[i]);
    }

    for (int i = 0; i < arg_count; i++) free(args[i]);
    free(signed_url);
    free(args);

    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, other_parameters);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TWDataCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, block);
    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        dlog("Curl Failed. Error: %s", curl_easy_strerror(result));
        free(block->raw);
        free(block);
        
        block = NULL;
    } else {
        block->raw = realloc(block->raw, block->size + 1);
        char *loc = block->raw + block->size;
        (*loc) = 0;
    }
    
    curl_easy_cleanup(curl);
    return block;
}

TWBlock *TWHttpPerformGet(const char *url)
{
    dlog("Performing HTTP GET to URL %s", url);
    TWBlock *block = malloc(sizeof(TWBlock));
    memset(block, 0, sizeof(TWBlock));

    TWString *oauth_tokens[4] = {NULL, NULL, NULL, NULL};
    oauth_tokens[0] = TWKeyStoreGetConsumerToken();
    oauth_tokens[1] = TWKeyStoreGetConsumerSecret();
    oauth_tokens[2] = TWKeyStoreGetUserToken();
    oauth_tokens[3] = TWKeyStoreGetUserSecret();

    for (int i = 0; i < 4; i++)
    {
        if (!oauth_tokens[i]) return NULL;
        dlog("Have OAuth Token: %s", oauth_tokens[i]->raw);
    }

    CURL *curl = curl_easy_init();
    char *signed_url = oauth_sign_url2(url, NULL, OA_HMAC, "GET", oauth_tokens[0]->raw, oauth_tokens[1]->raw, oauth_tokens[2]->raw, oauth_tokens[3]->raw);

    for (int i = 0; i < 4; i++)
    {
        free(oauth_tokens[i]->raw);
        free(oauth_tokens[i]);
    }

    curl_easy_setopt(curl, CURLOPT_URL, signed_url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TWDataCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, block);
    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        dlog("Curl Failed. Error: %s", curl_easy_strerror(result));
        free(block->raw);
        free(block);

        block = NULL;
    } else {
        block->raw = realloc(block->raw, block->size + 1);
        char *loc = block->raw + block->size;
        (*loc) = 0;
    }

    curl_easy_cleanup(curl);
    return block;
}
