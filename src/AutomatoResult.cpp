#include <AutomatoResult.h>

const char* resultString(ResultCode rc)
{
    switch (rc) {
        case rc_ok:
            return "ok";
        case rc_no_message_received:
            return "no message received";
        case rc_invalid_message_type:
            return "invalid message type";
        case rc_invalid_pin_number:
            return "invalid pin number";
        case rc_invalid_mem_address:
            return "invalid mem address";
        case rc_invalid_mem_length:
            return "invalid mem length";
        case rc_invalid_reply_message:
            return "expected a reply message";
        case rc_operation_forbidden:
            return "operation forbidden";
        case rc_reply_timeout:
            return "timeout waiting for reply message";
        case rc_rh_router_error_invalid_length:
            return "router error invalid length";
        case rc_rh_router_error_no_route:
            return "router error no route";
        case rc_rh_router_error_timeout:
            return "router error timeout";
        case rc_rh_router_error_no_reply:
            return "router error no reply";
        case rc_rh_router_error_unable_to_deliver:
            return "router error unable to deliver";
        case rc_invalid_rh_router_error:
            return "invalid rh router error code";
        case rc_invalid_mapfield_index:
            return "rc_invalid_mapfield_index";
        case rc_invalid_address_type:
            return "rc_invalid_address_type";
        default:
            return "unknown error code";
    }
}

AutomatoResult::operator bool () {
    if (this->rc == rc_ok)
        return true;
    else
        return false;
}

const char* AutomatoResult::as_string() const
{
    return resultString(rc);
}

ResultCode AutomatoResult::resultCode() const
{
    return this->rc;
}


AutomatoResult::AutomatoResult()
{
    this->rc = rc_ok;
}

AutomatoResult::AutomatoResult(ResultCode rc)
{
    this->rc = rc;
}





