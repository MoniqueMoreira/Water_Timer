#include "return_status.h"

static const char* RETURN_STATUS_MESSAGES[] = {
    "OK",
    "Error",
    "Not Initialized",
    "Timeout",
    "Not Found",
    "Invalid Parameter"
};

inline const char* RETURN_STATUS_Module(RETURN_STATUS_t status) {
    switch (status) {
        case RETURN_STATUS_OK: return RETURN_STATUS_MESSAGES[0];
        case RETURN_STATUS_ERROR: return RETURN_STATUS_MESSAGES[1];
        case RETURN_STATUS_NOT_INITIALIZED: return RETURN_STATUS_MESSAGES[2];
        case RETURN_STATUS_TIMEOUT: return RETURN_STATUS_MESSAGES[3];
        case RETURN_STATUS_NOT_FOUND: return RETURN_STATUS_MESSAGES[4];
        case RETURN_STATUS_INVALID_PARAM: return RETURN_STATUS_MESSAGES[5];
        default: return "Unknown Status";
    }
}