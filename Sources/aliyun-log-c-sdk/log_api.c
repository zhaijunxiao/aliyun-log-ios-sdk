#include "log_util.h"
#include "log_api.h"
#include <string.h>
#include "log_sds.h"
#include "inner_log.h"

int LOG_OS_HttpPost(const char *url,
                    char **header_array,
                    int header_count,
                    const void *data,
                    int data_len,
                    post_log_result *http_response);

unsigned int LOG_GET_TIME();

void log_http_inject_headers(log_producer_config *config, char **src_headers, int src_count, char **dest_headers, int *dest_count);
void log_http_release_inject_headers(log_producer_config *config, char **dest_headers, int dest_count);

log_status_t sls_log_init(int32_t log_global_flag)
{
#if 0
    CURLcode ecode;
    if ((ecode = curl_global_init(log_global_flag)) != CURLE_OK)
    {
        aos_error_log("curl_global_init failure, code:%d %s.\n", ecode, curl_easy_strerror(ecode));
        return -1;
    }
#endif
    return 0;
}
void sls_log_destroy()
{
#if 0
    curl_global_cleanup();
#endif
}
#if 0
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t totalLen = size * nmemb;
    //printf("body  ---->  %d  %s \n", (int) (totalLen, (const char*) ptr);
    log_sds * buffer = (log_sds *)stream;
    if (*buffer == NULL)
    {
        *buffer = log_sdsnewEmpty(256);
    }
    *buffer = log_sdscpylen(*buffer, ptr, totalLen);
    return totalLen;
}

static size_t header_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t totalLen = size * nmemb;
    //printf("header  ---->  %d  %s \n", (int) (totalLen), (const char*) ptr);
    log_sds * buffer = (log_sds *)stream;
    // only copy header start with x-log-
    if (totalLen > 6 && memcmp(ptr, "x-log-", 6) == 0)
    {
        *buffer = log_sdscpylen(*buffer, ptr, totalLen);
    }
    return totalLen;
}
#endif

static const char sls_month_snames[12][4] =
        {
                "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
static const char sls_day_snames[7][4] =
        {
                "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };

void sls_rfc822_date(char *date_str, struct tm * xt)
{
    const char *s = NULL;
    int real_year = 2000;


    /* example: "Sat, 08 Jan 2000 18:31:41 GMT" */
    /*           12345678901234567890123456789  */

    s = &sls_day_snames[xt->tm_wday][0];
    *date_str++ = *s++;
    *date_str++ = *s++;
    *date_str++ = *s++;
    *date_str++ = ',';
    *date_str++ = ' ';
    *date_str++ = xt->tm_mday / 10 + '0';
    *date_str++ = xt->tm_mday % 10 + '0';
    *date_str++ = ' ';
    s = &sls_month_snames[xt->tm_mon][0];
    *date_str++ = *s++;
    *date_str++ = *s++;
    *date_str++ = *s++;
    *date_str++ = ' ';
    real_year = 1900 + xt->tm_year;
    /* This routine isn't y10k ready. */
    *date_str++ = real_year / 1000 + '0';
    *date_str++ = real_year % 1000 / 100 + '0';
    *date_str++ = real_year % 100 / 10 + '0';
    *date_str++ = real_year % 10 + '0';
    *date_str++ = ' ';
    *date_str++ = xt->tm_hour / 10 + '0';
    *date_str++ = xt->tm_hour % 10 + '0';
    *date_str++ = ':';
    *date_str++ = xt->tm_min / 10 + '0';
    *date_str++ = xt->tm_min % 10 + '0';
    *date_str++ = ':';
    *date_str++ = xt->tm_sec / 10 + '0';
    *date_str++ = xt->tm_sec % 10 + '0';
    *date_str++ = ' ';
    *date_str++ = 'G';
    *date_str++ = 'M';
    *date_str++ = 'T';
    *date_str++ = 0;
    return;
}

void get_now_time_str(char * buffer, int bufLen, int timeOffset)
{
    time_t rawtime = LOG_GET_TIME();
    struct tm timeinfo;
    if (timeOffset != 0)
    {
        rawtime += timeOffset;
    }
    gmtime_r(&rawtime, &timeinfo);
    sls_rfc822_date(buffer, &timeinfo);
}

void post_log_result_destroy(post_log_result * result)
{
    if (result != NULL)
    {
        if (result->errorMessage != NULL)
        {
            log_sdsfree(result->errorMessage);
        }
        if (result->requestID != NULL)
        {
            log_sdsfree(result->requestID);
        }
        free(result);
    }
}

#if 0
post_log_result * post_logs_from_lz4buf(const char *endpoint, const char * accesskeyId, const char *accessKey, const char *stsToken, const char *project, const char *logstore, lz4_log_buf * buffer, log_post_option * option)
{
    post_log_result * result = (post_log_result *)malloc(sizeof(post_log_result));
    memset(result, 0, sizeof(post_log_result));
    CURL *curl = curl_easy_init();
    if (curl != NULL)
    {
        // url
        log_sds url = NULL;
        if (option->using_https) {
            url = log_sdsnew("https://");
        } else {
            url = log_sdsnew("http://");
        }

        url = log_sdscat(url, project);
        url = log_sdscat(url, ".");
        url = log_sdscat(url, endpoint);
        url = log_sdscat(url, "/logstores/");
        url = log_sdscat(url, logstore);
        url = log_sdscat(url, "/shards/lb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        struct curl_slist *connect_to = NULL;
        if (option->remote_address != NULL)
        {
            // example.com::192.168.1.5:
            log_sds connect_to_item = log_sdsnew(project);
            connect_to_item = log_sdscat(connect_to_item, ".");
            connect_to_item = log_sdscat(connect_to_item, endpoint);
            connect_to_item = log_sdscat(connect_to_item, "::");
            connect_to_item = log_sdscat(connect_to_item, option->remote_address);
            connect_to_item = log_sdscat(connect_to_item, ":");

            connect_to = curl_slist_append(NULL, connect_to_item);
            curl_easy_setopt(curl, CURLOPT_CONNECT_TO, connect_to);
            log_sdsfree(connect_to_item);
        }

        char nowTime[64];
        get_now_time_str(nowTime, 64);

        char md5Buf[33];
        md5Buf[32] = '\0';
        int lz4Flag = option == NULL || option->compress_type == 1;
        md5_to_string((const char *)buffer->data, buffer->length, (char *)md5Buf);


        //puts(md5Buf);
        //puts(nowTime);

        struct curl_slist* headers = NULL;

        headers=curl_slist_append(headers, "Content-Type:application/x-protobuf");
        headers=curl_slist_append(headers, "x-log-apiversion:0.6.0");
        if (lz4Flag)
        {
            headers=curl_slist_append(headers, "x-log-compresstype:lz4");
        }
        if (stsToken != NULL)
        {
            log_sds tokenHeader = log_sdsnew("x-acs-security-token:");
            tokenHeader = log_sdscat(tokenHeader, stsToken);
            headers=curl_slist_append(headers, tokenHeader);
            log_sdsfree(tokenHeader);
        }
        headers=curl_slist_append(headers, "x-log-signaturemethod:hmac-sha1");
        log_sds headerTime = log_sdsnew("Date:");
        headerTime = log_sdscat(headerTime, nowTime);
        headers=curl_slist_append(headers, headerTime);
        log_sds headerMD5 = log_sdsnew("Content-MD5:");
        headerMD5 = log_sdscat(headerMD5, md5Buf);
        headers=curl_slist_append(headers, headerMD5);

        log_sds headerLen= log_sdsnewEmpty(64);
        headerLen = log_sdscatprintf(headerLen, "Content-Length:%d", (int)buffer->length);
        headers=curl_slist_append(headers, headerLen);

        log_sds headerRawLen = log_sdsnewEmpty(64);
        headerRawLen = log_sdscatprintf(headerRawLen, "x-log-bodyrawsize:%d", (int)buffer->raw_length);
        headers=curl_slist_append(headers, headerRawLen);

        log_sds headerHost = log_sdsnewEmpty(128);
        headerHost = log_sdscatprintf(headerHost, "Host:%s.%s", project, endpoint);
        headers=curl_slist_append(headers, headerHost);

        char sha1Buf[65];
        sha1Buf[64] = '\0';

        log_sds sigContent = log_sdsnewEmpty(512);
        if (lz4Flag)
        {
            if (stsToken == NULL)
            {
                sigContent = log_sdscatprintf(sigContent,
                                          "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                          md5Buf, nowTime, (int)buffer->raw_length, logstore);
            }
            else
            {
                sigContent = log_sdscatprintf(sigContent,
                                          "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                          md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore);
            }
        }
        else
        {
            if (stsToken == NULL)
            {
                sigContent = log_sdscatprintf(sigContent,
                                          "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                          md5Buf, nowTime, (int)buffer->raw_length, logstore);
            }
            else
            {
                sigContent = log_sdscatprintf(sigContent,
                                          "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                          md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore);
            }

        }

        //puts("#######################");
        //puts(sigContent);

        int destLen = signature_to_base64(sigContent, log_sdslen(sigContent), accessKey, strlen(accessKey), sha1Buf);
        sha1Buf[destLen] = '\0';
        //puts(sha1Buf);
        log_sds headerSig = log_sdsnewEmpty(256);
        headerSig = log_sdscatprintf(headerSig, "Authorization:LOG %s:%s", accesskeyId, sha1Buf);
        //puts(headerSig);
        headers=curl_slist_append(headers, headerSig);


        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (void *)buffer->data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, buffer->length);


        curl_easy_setopt(curl, CURLOPT_FILETIME, 1);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
        curl_easy_setopt(curl, CURLOPT_NETRC, CURL_NETRC_IGNORED);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "log-c-lite_0.1.0");

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);

        if (option != NULL)
        {
            // interface
            if (option->interface != NULL)
            {
                curl_easy_setopt(curl, CURLOPT_INTERFACE, option->interface);
            }
            if (option->operation_timeout > 0)
            {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, option->operation_timeout);
            }
            if (option->connect_timeout > 0)
            {
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, option->connect_timeout);
            }
        }



        log_sds header = log_sdsnewEmpty(64);
        log_sds body = NULL;


        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);

        //curl_easy_setopt(curl,CURLOPT_VERBOSE,1); //打印调试信息

        CURLcode res = curl_easy_perform(curl);
        //printf("result : %s \n", curl_easy_strerror(res));
        long http_code;
        if (res == CURLE_OK)
        {
            if ((res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code)) != CURLE_OK)
            {
                printf("get info result : %s \n", curl_easy_strerror(res));
                result->statusCode = -2;
            } else {
                result->statusCode = http_code;
            }
        }
        else
        {
            if (body == NULL)
            {
                body = log_sdsnew(curl_easy_strerror(res));
            }
            else
            {
                body = log_sdscpy(body, curl_easy_strerror(res));
            }
            result->statusCode = -1 * (int)res;
        }
        // header and body 's pointer may be modified in callback (size > 256)
        if (log_sdslen(header) > 0)
        {
            result->requestID = header;
        }
        else
        {
            log_sdsfree(header);
            header = NULL;
        }
        // body will be NULL or a error string(net error or request error)
        result->errorMessage = body;


        curl_slist_free_all(headers); /* free the list again */
        log_sdsfree(url);
        log_sdsfree(headerTime);
        log_sdsfree(headerMD5);
        log_sdsfree(headerLen);
        log_sdsfree(headerRawLen);
        log_sdsfree(headerHost);
        log_sdsfree(sigContent);
        log_sdsfree(headerSig);
        /* always cleanup */
        curl_easy_cleanup(curl);
        if (connect_to != NULL)
        {
            curl_slist_free_all(connect_to);
        }
    }


    return result;
}
#else
struct cur_slist {
  char *data;
  struct cur_slist *next;
};

struct cur_slist * cur_slist_append(struct cur_slist *lst, const char *s)
{
    struct cur_slist *t = (struct cur_slist *)malloc(sizeof(struct cur_slist));
    t->data = strdup(s);
    t->next = NULL;

    if(lst == NULL)
        return t;

    struct cur_slist *orig = lst;

    while(lst->next) {
        lst = lst->next;
    }

    lst->next = t;

    return orig;
}

void cur_slist_free_all(struct cur_slist *lst)
{
    while(lst != NULL) {
        struct cur_slist *n = lst->next;
        free(lst->data);
        free(lst);
        lst = n;
    }
}

static int is_str_empty(const char *str)
{
    if (!str)
    {
        return 1;
    }

    if (strcmp(str, "") == 0) {
        return 1;
    }

    return 0;
}

post_log_result * post_logs_from_lz4buf(const char *endpoint, const char * accesskeyId, const char *accessKey, const char *stsToken, const char *project, const char *logstore, lz4_log_buf * buffer, log_post_option * option)
{
    return post_logs_from_lz4buf_with_config(NULL, endpoint, project, logstore, accesskeyId, accessKey, stsToken, buffer, option);
}

post_log_result * post_logs_from_lz4buf_with_config(log_producer_config *config, const char *endpoint, const char *project, const char *logstore, const char *accessKeyId, const char *accessKeySecret, const char *stsToken, lz4_log_buf *buffer, log_post_option *option)
{
    aos_info_log("start post_logs_from_lz4buf.");
    post_log_result * result = (post_log_result *)malloc(sizeof(post_log_result));
    memset(result, 0, sizeof(post_log_result));

    // pre-check parameters
    if (is_str_empty(endpoint) || is_str_empty(project) || is_str_empty(logstore))
    {
        result->statusCode = 405;
        result->requestID  =  log_sdsnewEmpty(64);
        result->errorMessage = log_sdsnew("Invalid producer config destination params");
        return result;
    }

    if (is_str_empty(accessKeyId) || is_str_empty(accessKeySecret))
    {
        result->statusCode = 405;
        result->requestID  =  log_sdsnewEmpty(64);
        result->errorMessage = log_sdsnew("Invalid producer config authority params");
        return result;
    }

    {
        // url
        log_sds url = NULL;
        if (option->using_https) {
            url = log_sdsnew("https://");
        } else {
            url = log_sdsnew("http://");
        }

        url = log_sdscat(url, project);
        url = log_sdscat(url, ".");
        url = log_sdscat(url, endpoint);
        url = log_sdscat(url, "/logstores/");
        url = log_sdscat(url, logstore);
        int shardkeyMode = option->mode == 1 && !is_str_empty(option->shardKey);
        if (shardkeyMode) {
            aos_info_log("key hash: %s", option->shardKey);
            url = log_sdscat(url, "/shards/route?key=");
            url = log_sdscat(url, option->shardKey);
        } else {
            url = log_sdscat(url, "/shards/lb");
        }

        // TODO: url

        char nowTime[64];
        get_now_time_str(nowTime, 64, option->ntp_time_offset);

        char md5Buf[33];
        md5Buf[32] = '\0';
        int lz4Flag = option == NULL || option->compress_type == 1;
        md5_to_string((const char *)buffer->data, buffer->length, (char *)md5Buf);


        //puts(md5Buf);
        //puts(nowTime);

        struct cur_slist* headers = NULL;

        headers=cur_slist_append(headers, "Content-Type:application/x-protobuf");
        headers=cur_slist_append(headers, "x-log-apiversion:0.6.0");
        if (lz4Flag)
        {
            headers=cur_slist_append(headers, "x-log-compresstype:lz4");
        }
        if (stsToken != NULL)
        {
            log_sds tokenHeader = log_sdsnew("x-acs-security-token:");
            tokenHeader = log_sdscat(tokenHeader, stsToken);
            headers=cur_slist_append(headers, tokenHeader);
            log_sdsfree(tokenHeader);
        }
        headers=cur_slist_append(headers, "x-log-signaturemethod:hmac-sha1");
        log_sds headerTime = log_sdsnew("Date:");
        headerTime = log_sdscat(headerTime, nowTime);
        headers=cur_slist_append(headers, headerTime);
        log_sds headerMD5 = log_sdsnew("Content-MD5:");
        headerMD5 = log_sdscat(headerMD5, md5Buf);
        headers=cur_slist_append(headers, headerMD5);

        log_sds headerLen= log_sdsnewEmpty(64);
        headerLen = log_sdscatprintf(headerLen, "Content-Length:%d", (int)buffer->length);
        headers=cur_slist_append(headers, headerLen);

        log_sds headerRawLen = log_sdsnewEmpty(64);
        headerRawLen = log_sdscatprintf(headerRawLen, "x-log-bodyrawsize:%d", (int)buffer->raw_length);
        headers=cur_slist_append(headers, headerRawLen);

        log_sds headerHost = log_sdsnewEmpty(128);
        headerHost = log_sdscatprintf(headerHost, "Host:%s.%s", project, endpoint);
        headers=cur_slist_append(headers, headerHost);

        char sha1Buf[65];
        sha1Buf[64] = '\0';

        log_sds sigContent = log_sdsnewEmpty(512);
        if (lz4Flag)
        {
            if (stsToken == NULL)
            {
                if (shardkeyMode) {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/route?key=%s",
                                              md5Buf, nowTime, (int)buffer->raw_length, logstore, option->shardKey);
                } else {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                              md5Buf, nowTime, (int)buffer->raw_length, logstore);
                }
            }
            else
            {
                if (shardkeyMode) {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/route?key=%s",
                                              md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore, option->shardKey);
                } else {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-compresstype:lz4\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                              md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore);
                }
            }
        }
        else
        {
            if (stsToken == NULL)
            {
                if (shardkeyMode) {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/route?key=%s",
                                              md5Buf, nowTime, (int)buffer->raw_length, logstore, option->shardKey);
                } else {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                              md5Buf, nowTime, (int)buffer->raw_length, logstore);
                }
            }
            else
            {
                if (shardkeyMode) {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/route?key=%s",
                                              md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore, option->shardKey);
                } else {
                    sigContent = log_sdscatprintf(sigContent,
                                              "POST\n%s\napplication/x-protobuf\n%s\nx-acs-security-token:%s\nx-log-apiversion:0.6.0\nx-log-bodyrawsize:%d\nx-log-signaturemethod:hmac-sha1\n/logstores/%s/shards/lb",
                                              md5Buf, nowTime, stsToken, (int)buffer->raw_length, logstore);
                }
            }

        }

        //puts("#######################");
        //puts(sigContent);

        int destLen = signature_to_base64(sigContent, log_sdslen(sigContent), accessKeySecret, strlen(accessKeySecret), sha1Buf);
        sha1Buf[destLen] = '\0';
        //puts(sha1Buf);
        log_sds headerSig = log_sdsnewEmpty(256);
        headerSig = log_sdscatprintf(headerSig, "Authorization:LOG %s:%s", accessKeyId, sha1Buf);
        //puts(headerSig);
        headers=cur_slist_append(headers, headerSig);

        // TODO: headers

//        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // TODO: data
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (void *)buffer->data);
//        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, buffer->length);

        const int max_header_count = 50;
        char *header_array[max_header_count];
        int header_count = 0;
        struct cur_slist *h = headers;
        while(h != NULL) {
            header_array[header_count] = h->data;
            header_count++;
            h = h->next;
        }

        char *dest_header_array[max_header_count];
        int *dest_count = (int *)malloc(sizeof(int));
        memset(dest_count, 0, sizeof(int));

        log_http_inject_headers(config, header_array, header_count, dest_header_array, dest_count);
        char **final_header_array = (*dest_count) == 0 ? header_array : dest_header_array;
        int final_header_count = (*dest_count) == 0 ? header_count : (*dest_count);
        
        post_log_result *http_response = (post_log_result*)malloc(sizeof(post_log_result));
        int res = LOG_OS_HttpPost(url, final_header_array, final_header_count, (const void *) buffer->data, buffer->length, http_response);
        log_http_release_inject_headers(config, dest_header_array, *dest_count);

        result->statusCode = res;
        result->requestID  = log_sdsnew(http_response->requestID);
        log_sds error_message = log_sdsnew("status: ");
        error_message = log_sdscatprintf(error_message, "%d, ", res);
        error_message = log_sdscat(error_message, http_response->errorMessage);
        result->errorMessage = error_message;

        cur_slist_free_all(headers); /* free the list again */
        log_sdsfree(url);
        log_sdsfree(headerTime);
        log_sdsfree(headerMD5);
        log_sdsfree(headerLen);
        log_sdsfree(headerRawLen);
        log_sdsfree(headerHost);
        log_sdsfree(sigContent);
        log_sdsfree(headerSig);
        
        free(http_response->requestID);
        free(http_response->errorMessage);
        free(http_response);
        free(dest_count);
    }


    return result;
}

post_log_result * post_logs_from_lz4buf_webtracking(const char *endpoint, const char *project, const char *logstore, lz4_log_buf *buffer, log_post_option *option)
{
    aos_info_log("post_logs_from_lz4buf_webtracking start.");
    post_log_result * result = (post_log_result *)malloc(sizeof(post_log_result));
    memset(result, 0, sizeof(post_log_result));

    // pre-check parameters
    if (is_str_empty(endpoint) || is_str_empty(project) || is_str_empty(logstore))
    {
        result->statusCode = 405;
        result->requestID  =  log_sdsnewEmpty(64);
        result->errorMessage = log_sdsnew("Invalid producer config destination params");
        return result;
    }

    {
        // url
        log_sds url = NULL;
        if (option->using_https) {
            url = log_sdsnew("https://");
        } else {
            url = log_sdsnew("http://");
        }

        url = log_sdscat(url, project);
        url = log_sdscat(url, ".");
        url = log_sdscat(url, endpoint);
        url = log_sdscat(url, "/logstores/");
        url = log_sdscat(url, logstore);
        url = log_sdscat(url, "/track");

        char nowTime[64];
        get_now_time_str(nowTime, 64, option->ntp_time_offset);

        int lz4Flag = option->compress_type == 1;

        struct cur_slist* headers = NULL;

        headers=cur_slist_append(headers, "x-log-apiversion:0.6.0");
        if (lz4Flag)
        {
            headers=cur_slist_append(headers, "x-log-compresstype:lz4");
        }

        log_sds headerRawLen = log_sdsnewEmpty(64);
        headerRawLen = log_sdscatprintf(headerRawLen, "x-log-bodyrawsize:%d", (int)buffer->raw_length);
        headers=cur_slist_append(headers, headerRawLen);

        const int max_header_count = 50;
        char *header_array[max_header_count];
        int header_count = 0;
        struct cur_slist *h = headers;
        while(h != NULL) {
            header_array[header_count] = h->data;
            header_count++;
            h = h->next;
        }

        aos_info_log("post_logs_from_lz4buf_webtracking, start LOG_OS_HttpPost.");
        post_log_result *http_response = (post_log_result*)malloc(sizeof(post_log_result));
        int res = LOG_OS_HttpPost(url, header_array, header_count,
                                  (const void *) buffer->data, buffer->length, http_response);
        aos_info_log("post_logs_from_lz4buf_webtracking, LOG_OS_HttpPost res: %d.", res);

        result->statusCode = res;
        result->requestID  = log_sdsnew(http_response->requestID);
        log_sds error_message = log_sdsnew("status: ");
        error_message = log_sdscatprintf(error_message, "%d, ", res);
        error_message = log_sdscat(error_message, http_response->errorMessage);
        result->errorMessage = error_message;

        cur_slist_free_all(headers); /* free the list again */
        log_sdsfree(url);
        log_sdsfree(headerRawLen);
    }

    return result;
}
#endif
