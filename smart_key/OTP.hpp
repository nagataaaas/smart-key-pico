#ifndef __HMAC_H
#define __HMAC_H

#define RESULT_LEN 64

std::string get_hotp_token(std::string secret, int intervals_no);

std::string get_totp_token(std::string secret);

#endif