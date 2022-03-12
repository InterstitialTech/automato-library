#ifndef AutomatoResult_h_INCLUDED
#define AutomatoResult_h_INCLUDED

enum ResultCode {
    rc_ok,
    rc_no_message_received,
    rc_invalid_message_type,
    rc_invalid_pin_number,
    rc_invalid_mem_address,
    rc_invalid_mem_length,
    rc_invalid_reply_message,
    rc_operation_forbidden,
    rc_reply_timeout,
    rc_rh_router_error_invalid_length,
    rc_rh_router_error_no_route,
    rc_rh_router_error_timeout,
    rc_rh_router_error_no_reply,
    rc_rh_router_error_unable_to_deliver,
    rc_invalid_rh_router_error,
    rc_count,  // total number of ResultCodes.
};

class AutomatoResult {
public:
    AutomatoResult();
    AutomatoResult(ResultCode rc);
    operator bool ();    // true for rc_ok, false otherwise.
    const char* as_string() const;
    ResultCode resultCode() const;
private:
    ResultCode rc;
};

#endif // AutomatoResult_h_INCLUDED

