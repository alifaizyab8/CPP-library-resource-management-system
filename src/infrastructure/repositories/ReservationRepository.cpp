#include "ReservationRepository.h"
#include <iostream>

using namespace std;

/* *************************************************************************
                 ---------- CONSTRUCTORS & DESTRUCTORS ----------
   *************************************************************************  */

ReservationRepository::ReservationRepository(sqlite3 *connection) : db(connection) {}
ReservationRepository::~ReservationRepository() {}

/* *************************************************************************
                    ---------- INSERT RESERVATIONS ----------
   *************************************************************************  */

bool ReservationRepository::insertReservation(Reservation &reservation)
{

    const char *sql =
        "INSERT INTO reservations "
        "(user_id, resource_id, reservation_date, expiry_date, "
        "is_fulfilled, is_cancelled, status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare INSERT: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, reservation.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, reservation.getResourceId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, reservation.getReservationDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, reservation.getExpiryDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, reservation.getIsFulfilled() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 6, reservation.getIsCancelled() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, reservation.getStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for INSERT: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
    {
        cerr << "Failed to execute INSERT: " << sqlite3_errmsg(db) << endl;
    }
    else
    {
        sqlite3_int64 lastId = sqlite3_last_insert_rowid(db);
        reservation.setReservationId(static_cast<int>(lastId));
    }

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- UPDATE RESERVATIONS ----------
   *************************************************************************  */

bool ReservationRepository::updateReservation(const Reservation &reservation)
{

    const char *sql =
        "UPDATE reservations SET "
        "user_id=?, resource_id=?, reservation_date=?, expiry_date=?, "
        "is_fulfilled=?, is_cancelled=?, status=? "
        "WHERE reservation_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare UPDATE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, reservation.getUserId()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, reservation.getResourceId()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, reservation.getReservationDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, reservation.getExpiryDate().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, reservation.getIsFulfilled() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 6, reservation.getIsCancelled() ? 1 : 0) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 7, reservation.getStatus().c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 8, reservation.getReservationId()) != SQLITE_OK)
    {

        cerr << "Failed to bind parameters for UPDATE: "
             << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute UPDATE: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                    ---------- DELETE RESERVATIONS ----------
   *************************************************************************  */

bool ReservationRepository::deleteReservation(int reservationId)
{

    const char *sql =
        "DELETE FROM reservations WHERE reservation_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare DELETE: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, reservationId) != SQLITE_OK)
    {
        cerr << "Failed to bind reservationId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        cerr << "Failed to execute DELETE: " << sqlite3_errmsg(db) << endl;

    sqlite3_finalize(stmt);
    return success;
}

/* *************************************************************************
                   ---------- GET RESERVATION BY ID's ----------
   *************************************************************************  */

std::unique_ptr<Reservation> ReservationRepository::getById(int reservationId)
{

    const char *sql =
        "SELECT reservation_id, user_id, resource_id, reservation_date, expiry_date, "
        "is_fulfilled, is_cancelled, status "
        "FROM reservations WHERE reservation_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    if (sqlite3_bind_int(stmt, 1, reservationId) != SQLITE_OK)
    {
        cerr << "Failed to bind reservationId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    std::unique_ptr<Reservation> reservation = nullptr;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        reservation = std::make_unique<Reservation>(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5) == 1,
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return reservation;
}

/* *************************************************************************
                ---------- GET RESERVATIONS BY USER ID's ----------
   *************************************************************************  */

std::vector<Reservation> ReservationRepository::getByUserId(int userId)
{

    std::vector<Reservation> reservations;

    const char *sql =
        "SELECT reservation_id, user_id, resource_id, reservation_date, expiry_date, "
        "is_fulfilled, is_cancelled, status "
        "FROM reservations WHERE user_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT BY USER: " << sqlite3_errmsg(db) << endl;
        return reservations;
    }

    if (sqlite3_bind_int(stmt, 1, userId) != SQLITE_OK)
    {
        cerr << "Failed to bind userId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return reservations;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        reservations.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5) == 1,
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return reservations;
}

/* *************************************************************************
               ---------- GET RESERVATIONS BY RESOURCE ID ----------
   *************************************************************************  */

std::vector<Reservation> ReservationRepository::getByResourceId(int resourceId)
{

    std::vector<Reservation> reservations;

    const char *sql =
        "SELECT reservation_id, user_id, resource_id, reservation_date, expiry_date, "
        "is_fulfilled, is_cancelled, status "
        "FROM reservations WHERE resource_id=?;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT BY RESOURCE: " << sqlite3_errmsg(db) << endl;
        return reservations;
    }

    if (sqlite3_bind_int(stmt, 1, resourceId) != SQLITE_OK)
    {
        cerr << "Failed to bind resourceId: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return reservations;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        reservations.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5) == 1,
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return reservations;
}

/* *************************************************************************
                    ---------- GET ALL RESERVATIONS ----------
   *************************************************************************  */

std::vector<Reservation> ReservationRepository::getAllReservations()
{

    std::vector<Reservation> reservations;

    const char *sql =
        "SELECT reservation_id, user_id, resource_id, reservation_date, expiry_date, "
        "is_fulfilled, is_cancelled, status "
        "FROM reservations;";

    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Failed to prepare SELECT ALL: " << sqlite3_errmsg(db) << endl;
        return reservations;
    }

    auto safeText = [](sqlite3_stmt *stmt, int col) -> string
    {
        const unsigned char *text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char *>(text) : "";
    };

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        reservations.emplace_back(
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            safeText(stmt, 3),
            safeText(stmt, 4),
            sqlite3_column_int(stmt, 5) == 1,
            sqlite3_column_int(stmt, 6) == 1,
            safeText(stmt, 7));
    }

    sqlite3_finalize(stmt);
    return reservations;
}

/* *************************************************************************
                    ---------- SAVE ALL RESERVATIONS ----------
   *************************************************************************  */

bool ReservationRepository::save(Reservation &reservation)
{
    if (reservation.getReservationId() == 0)
    {
        return insertReservation(reservation);
    }
    return updateReservation(reservation);
}