#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

void send_discord_webhook(const char *url, const char *message) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        char json_payload[1024];
        snprintf(json_payload, sizeof(json_payload), "{\"content\": \"%s\"}", message);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    const char *webhook_url = "WEBHOOK_URL";
    const char *message = "testing";

    send_discord_webhook(webhook_url, message);

    return 0;
}
