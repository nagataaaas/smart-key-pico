#ifndef __NETWORK_H
#define __NETWORK_H

int get_timestamp();

void connect_wifi();

enum class APILockState {
    LOCKED,
    UNLOCKED,
    UNKNOWN,
};

void unlockAPI(char* code);
void lockAPI(char* code);
APILockState getState(char* code);
#endif