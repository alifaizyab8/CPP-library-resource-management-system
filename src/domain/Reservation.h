#pragma once
#include <string>

class Reservation
{
private:
    int reservationId;
    int userId;
    int resourceId;
    std::string reservationDate;
    std::string expiryDate;
    bool isFulfilled;
    bool isCancelled;
    std::string status;

public:
    Reservation() : reservationId(0), userId(0), resourceId(0), isFulfilled(false), isCancelled(false) {}

    Reservation(int id, int uId, int rId, const std::string &rDate, const std::string &eDate,
                bool fulfilled, bool cancelled, const std::string &stat)
        : reservationId(id), userId(uId), resourceId(rId), reservationDate(rDate), expiryDate(eDate),
          isFulfilled(fulfilled), isCancelled(cancelled), status(stat) {}

    // Getters
    int getReservationId() const { return reservationId; }
    int getUserId() const { return userId; }
    int getResourceId() const { return resourceId; }
    std::string getReservationDate() const { return reservationDate; }
    std::string getExpiryDate() const { return expiryDate; }
    bool getIsFulfilled() const { return isFulfilled; }
    bool getIsCancelled() const { return isCancelled; }
    std::string getStatus() const { return status; }

    // Setters
    void setReservationId(int id) { reservationId = id; }
    void setUserId(int uId) { userId = uId; }
    void setResourceId(int rId) { resourceId = rId; }
    void setReservationDate(const std::string &rDate) { reservationDate = rDate; }
    void setExpiryDate(const std::string &eDate) { expiryDate = eDate; }
    void setIsFulfilled(bool fulfilled) { isFulfilled = fulfilled; }
    void setIsCancelled(bool cancelled) { isCancelled = cancelled; }
    void setStatus(const std::string &stat) { status = stat; }
};
