#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/entities/FundRequest.h"

class FundRequestRepository {
private:
    sqlite3* db;

    bool insertFundRequest(const FundRequest& request);
    bool updateFundRequest(const FundRequest& request);

public:
    explicit FundRequestRepository(sqlite3* connection);
    ~FundRequestRepository();

    bool save(const FundRequest& request);
    bool deleteFundRequest(int requestId);

    std::unique_ptr<FundRequest> getById(int requestId);
    std::vector<FundRequest> getByUserId(int userId);
    std::vector<FundRequest> getAllFundRequests();
};