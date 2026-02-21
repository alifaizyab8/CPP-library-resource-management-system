#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>

#include "../../domain/entities/Reservation.h"

class ReservationRepository {
private:
    sqlite3* db;

    bool insertReservation(const Reservation& reservation);
    bool updateReservation(const Reservation& reservation);

public:
    explicit ReservationRepository(sqlite3* connection);
    ~ReservationRepository();

    bool save(const Reservation& reservation);
    bool deleteReservation(int reservationId);

    Reservation* getById(int reservationId);
    std::vector<Reservation> getByUserId(int userId);
    std::vector<Reservation> getByResourceId(int resourceId);
    std::vector<Reservation> getAllReservations();
};