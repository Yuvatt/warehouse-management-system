#pragma once

#include "../include/Order.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

#define NO_VOLUNTEER = -1;

Order::Order(int id, int customerId, int distance)
    : id(id), customerId(customerId), distance(distance),
      status(OrderStatus::PENDING), driverId(-1), collectorId(-1) {}

int Order::getId() const { return id; }

int Order::getCustomerId() const { return customerId; }

void Order::setStatus(OrderStatus status) { this->status = status; }
void Order::setCollectorId(int collectorId) { this->collectorId = collectorId; }

void Order::setDriverId(int driverId) { this->driverId = driverId; }

int Order::getCollectorId() const { return collectorId; }

int Order::getDriverId() const { return driverId; }

OrderStatus Order::getStatus() const { return status; }
// return the Order status in string
const string Order::getStatusString() const {
    if (status == OrderStatus::COLLECTING)
        return "Collecting";
    if (status == OrderStatus::DELIVERING)
        return "Delivering";
    if (status == OrderStatus::PENDING)
        return "Pending";
    if (status == OrderStatus::COMPLETED)
        return "Completed";
}

int Order::getDistance() const { return distance; }

const string Order::toString() const {
    return "OrderId:" + std::to_string(id) + "/n" +
           "OrderStatus:" + getStatusString() + "/n" +
           "CustomerId:" + std::to_string(customerId) + "/n" +
           "Collector:" + std::to_string(collectorId) + "/n" +
           "Driver:" + std::to_string(driverId);
}

// if the order is not null
bool Order::isValid() {
    if (id == -1)
        return false;
    return true;
}