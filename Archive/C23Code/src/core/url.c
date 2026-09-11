#include "core/url.h"
#include "algo/urlcodec.h"
#include <string.h>

char *mz_url_decode(const char *src, size_t len, bool plus_as_space) {
    return mz_urlcodec_decode(src, len, plus_as_space, nullptr);
}

char *mz_url_encode(const char *src, size_t len) {
    return mz_urlcodec_encode(src, len, nullptr);
}
