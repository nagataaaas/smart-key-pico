#include "OTP.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <string>

#include "crypto.cpp"

// python

// def get_hotp_token(secret: str, intervals_no: int) -> str:
//     msg = struct.pack(">Q", intervals_no)
//     h = hmac.new(secret.encode(), msg, hashlib.sha1).digest()
//     o = h[19] & 15
//     h = (struct.unpack(">I", h[o:o + 4])[0] & 0x7fffffff) % 1000000
//     return str(h)

// def get_totp_token(secret):
//     x = get_hotp_token(secret=secret, intervals_no=int(time.time()) // 30)
//     return (x[::-1].zfill(6))[::-1]

// C++

std::string get_hotp_token(std::string secret, int intervals_no) {
    std::string msg = std::to_string(intervals_no);

    char Data[2048], Key[2048];
    SHA256_DATA SD256;
    SHA256(&SD256, Data, 0);
    unsigned char* result = SD256.Val_String;
    unsigned char offset = result[63] & 15;
    unsigned int bin_code = (result[offset] & 0x7f) << 24 | (result[offset + 1] & 0xff) << 16 | (result[offset + 2] & 0xff) << 8 | (result[offset + 3] & 0xff);
    unsigned int hotp = bin_code % 1000000;
    free(result);
    return std::to_string(hotp);
}

std::string get_totp_token(std::string secret) {
    return get_hotp_token(secret, std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 30);
}