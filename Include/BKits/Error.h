#pragma once

#include <string>

// clang-format off
namespace BKits {
namespace Error {

enum class ErrorCode {
    OK = 0,    
    INVALID_ARGUMENT,
};

class Status {
public:    
    Status(ErrorCode code = ErrorCode::OK, std::string msg = "")
    : code_(code), message_(std::move(msg)) {}

    bool ok() const { return code_ == ErrorCode::OK; }
    ErrorCode code() const { return code_; }
    const std::string& message() const { return message_; }

    static Status OK() { return Status(ErrorCode::OK); }

private:
    ErrorCode code_;
    std::string message_;
};

template <typename T>
class StatusOr {
public:
    StatusOr(Status s): status_(s), has_value_(false) {}
    StatusOr(T val) : value_(std::move(val)), status_(Status::OK()), has_value_(true) {}

    bool ok() const { return has_value_; }
    Status status() const { return status_; }

    T& value() {
        if (!has_value_){
            // Error 
        }
        return value_;
    }

private:
    T value_;
    Status status_;
    bool has_value_; 
};

}
}
// clang-format on