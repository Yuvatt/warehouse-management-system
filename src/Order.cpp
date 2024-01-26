#pragma once

#include "../include/Order.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

#define NO_VOLUNTEER -1;

Order::Order(int id, int customerId, int distance) :
id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING) {}

int Order::getId() const
{
    return id;
}

int Order::getCustomerId() const
{
    return customerId;
}

void Order::setStatus(OrderStatus status)
{
    this->status = status;
}
void Order::setCollectorId(int collectorId)
{
    this->collectorId = collectorId;
}

void Order::setDriverId(int driverId)
{
    this->driverId = driverId;
}

int Order::getCollectorId() const
{
    return collectorId;
}

int Order::getDriverId() const
{
    return driverId;
}

OrderStatus Order::getStatus() const
{
    return status;
}

int Order::getDistance() const
{
    return distance;
}

const string Order::toString() const
{
    return "OrderId:" + id;
}

bool Order::isValid() { //if the order is not null
    if(id == -1)
        return false;
    return true;
}