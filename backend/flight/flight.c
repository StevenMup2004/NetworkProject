#include "flight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* join_with_dash(const char* row1, const char* row2, const char* row3) {

    size_t length = strlen(row1) + strlen(row2) + strlen(row3) + 3; 

    char* result = (char*)malloc(length * sizeof(char));
    if (!result) {
        perror("Failed to allocate memory");
        return NULL;
    }
    snprintf(result, length, "%s - %s - %s", row1, row2, row3);
    return result;
}


  

int fetch_flights(Flight **flights, int *count) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return 1;
    }

    if (mysql_query(conn, 
    "SELECT f.*, ap.*, al.*, da.*, aa.* FROM flights f "
    "JOIN airplanes ap ON f.airplane_id = ap.airplane_id "
    "JOIN airlines al ON ap.airline_id = al.airline_id "
    "JOIN airports da ON f.departure_airport = da.airport_id "
    "JOIN airports aa ON f.arrival_airport = aa.airport_id")) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return 1;
    }

    int num_rows = mysql_num_rows(res);
    *count = num_rows;
    *flights = malloc(sizeof(Flight) * num_rows);

    for (int i = 0; i < num_rows; i++) {
        MYSQL_ROW row = mysql_fetch_row(res);
        strncpy((*flights)[i].flight_id, row[0], sizeof((*flights)[i].flight_id) - 1);
        strncpy((*flights)[i].departure_time, row[3], sizeof((*flights)[i].departure_time) - 1);
        (*flights)[i].duration_minutes= atoi(row[4]);
        (*flights)[i].capacity= atoi(row[10]);
        (*flights)[i].price= atoi(row[11]);
        strncpy((*flights)[i].airplane_name, row[13], sizeof((*flights)[i].airplane_name) - 1);
        strncpy((*flights)[i].departure_airport, join_with_dash(row[17], row[18], row[19]), sizeof((*flights)[i].departure_airport) - 1);
        strncpy((*flights)[i].arrival_airport, join_with_dash(row[20], row[21], row[22]), sizeof((*flights)[i].arrival_airport) - 1);
        (*flights)[i].available_economy = (*flights)[i].capacity - 30 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "Economy");
        (*flights)[i].available_business= 20 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "Business");
        (*flights)[i].available_first_class = 10 - get_ticket_count_by_flight_id((*flights)[i].flight_id, "First class");
    }

    mysql_free_result(res);
    disconnect_db(conn);

    return 0;
}

int get_ticket_count_by_flight_id(const char *flight_id, const char *type) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return -1;
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024];
    
    snprintf(query, sizeof(query), 
             "SELECT bd.type, COUNT(*) AS total_tickets "
             "FROM bookings b "
             "JOIN booking_details bd ON b.booking_id = bd.booking_id "
             "WHERE b.flight_id = '%s' and bd.type = '%s' "
             "GROUP BY bd.type;", flight_id, type);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        return -1;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        return -1;
    }
    int num_rows = mysql_num_rows(res);
    if (num_rows == 0){
        return 0;
    }
    row = mysql_fetch_row(res);
        if (row != NULL) {
            return atoi(row[1]);
        }

    mysql_free_result(res);
    disconnect_db(conn);
}

char** get_seat_codes_by_flight_id(const char *flight_id, int *seat_count) {
    MYSQL *conn = connect_db();
    if (conn == NULL) {
        return NULL;
    }

    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[1024];

    snprintf(query, sizeof(query),
             "SELECT bd.seat_code "
             "FROM flights f "
             "JOIN bookings b ON f.flight_id = b.flight_id "
             "JOIN booking_details bd ON b.booking_id = bd.booking_id "
             "WHERE f.flight_id = '%s';", flight_id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "SELECT failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return NULL;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(conn));
        disconnect_db(conn);
        return NULL;
    }

    *seat_count = mysql_num_rows(res);
    if (*seat_count == 0) {
        mysql_free_result(res);
        disconnect_db(conn);
        return NULL; 
    }

    char **seat_codes = malloc(sizeof(char*) * (*seat_count));
    if (seat_codes == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        mysql_free_result(res);
        disconnect_db(conn);
        return NULL;
    }

    int i = 0;
    while ((row = mysql_fetch_row(res)) != NULL) {
        seat_codes[i] = strdup(row[0]);
        if (seat_codes[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for seat_code\n");
            for (int j = 0; j < i; j++) {
                free(seat_codes[j]);
            }
            free(seat_codes);
            mysql_free_result(res);
            disconnect_db(conn);
            return NULL;
        }
        i++;
    }

    mysql_free_result(res);
    disconnect_db(conn);
    return seat_codes;
}

