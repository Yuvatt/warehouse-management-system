#pragma once
#include "../include/Volunteer.h"
#include "../include/Order.h"

using std::string;
using std::vector;

#define NO_ORDER -1

/// Volunteer

Volunteer::Volunteer(int id, const string &name)
    : id(id), name(name), completedOrderId(NO_ORDER), activeOrderId(NO_ORDER){};

int Volunteer::getId() const { return id; }

const string &Volunteer::getName() const { return name; }

int Volunteer::getActiveOrderId() const { return activeOrderId; }

int Volunteer::getCompletedOrderId() const { return completedOrderId; }

bool Volunteer::isBusy() const { return (activeOrderId != NO_ORDER); }

//============= CollectorVolunteer: Volunteer =============================================

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown)
    : Volunteer(id, name), coolDown(coolDown) {}

CollectorVolunteer *CollectorVolunteer::clone() const {
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step() { timeLeft--; }

int CollectorVolunteer::getCoolDown() const { return coolDown; }

int CollectorVolunteer::getTimeLeft() const { return timeLeft; }

bool CollectorVolunteer::decreaseCoolDown() {
    timeLeft--;
    return (timeLeft == 0);
}

bool CollectorVolunteer::hasOrdersLeft() const { return true; }

bool CollectorVolunteer::canTakeOrder(const Order &order) const {
    return (!isBusy() && order.getStatus() == OrderStatus::PENDING);
}

void CollectorVolunteer::acceptOrder(const Order &order) {
    if (canTakeOrder(order)) {
        int activeOrderId = order.getId();
        timeLeft = coolDown;
    }
}
string CollectorVolunteer:: getMyType() const {
    return "collector";
}

string CollectorVolunteer::toString() const {
    string s = "volunteerID:" + getId();
    string m = ",ActiveOrder:" + getActiveOrderId();
    return s + m;
}

void CollectorVolunteer::resetTimeLeft() { timeLeft = coolDown; }

//============= LimitedCollectorVolunteer==================================

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name,
                                                     int coolDown,
                                                     int maxOrders)
    : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const {
    return new LimitedCollectorVolunteer(*this);
}

bool LimitedCollectorVolunteer::hasOrdersLeft() const { return ordersLeft > 0; }

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const {
    return (!isBusy() && hasOrdersLeft() &&
            order.getStatus() == OrderStatus::PENDING);
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order) {
    if (canTakeOrder(order)) {
        int activeOrderId = order.getId();
        resetTimeLeft();
        ordersLeft--;
    }
}

int LimitedCollectorVolunteer::getMaxOrders() const { return maxOrders; }

int LimitedCollectorVolunteer::getNumOrdersLeft() const { return ordersLeft; }

string LimitedCollectorVolunteer:: getMyType() const {
    return "limitedCollector";
}

string LimitedCollectorVolunteer::toString() const {
    string s = "volunteerID:" + getId();
    string m = ",ActiveOrder:" + getActiveOrderId();
    return s + m;
}

//  ===========================DriverVolunteer =============================================

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance,
                                 int distancePerStep)
    : Volunteer(id, name), maxDistance(maxDistance),
      distancePerStep(distancePerStep) {}

DriverVolunteer *DriverVolunteer::clone() const {
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const { return distanceLeft; }

int DriverVolunteer::getMaxDistance() const { return maxDistance; }
int DriverVolunteer::getDistancePerStep() const { return distancePerStep; }
bool DriverVolunteer::
    decreaseDistanceLeft() { // Decrease distanceLeft by distancePerStep,return
                             // true if distanceLeft<=0,false otherwise
    distanceLeft = distanceLeft - distancePerStep;
    return distanceLeft <= 0;
}

bool DriverVolunteer::hasOrdersLeft() const { return true; }

bool DriverVolunteer::canTakeOrder(const Order &order) const {
    return (!isBusy() && order.getDistance() <= maxDistance &&
            order.getStatus() == OrderStatus::DELIVERING);
}

void DriverVolunteer::acceptOrder(const Order &order) {
    if (canTakeOrder(order)) {
        int activeOrderId = order.getId();
        distanceLeft = maxDistance;
    }
}

void DriverVolunteer::step() { distanceLeft = distanceLeft - distancePerStep; }

string DriverVolunteer:: getMyType() const {
    return "driver";
}

string DriverVolunteer::toString() const {
    string s = "volunteerID:" + getId();
    string m = ",ActiveOrder:" + getActiveOrderId();
    return s + m;
}

void DriverVolunteer::setDistance(int distance) { distanceLeft = distance; }

// =========================LimitedDriverVolunteer====================

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name,
                                               int maxDistance,
                                               int distancePerStep,
                                               int maxOrders)
    : DriverVolunteer(id, name, maxDistance, distancePerStep),
      maxOrders(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const {
    LimitedDriverVolunteer(*this);
}
int LimitedDriverVolunteer::getMaxOrders() const { return maxOrders; }

int LimitedDriverVolunteer::getNumOrdersLeft() const { return ordersLeft; }
bool LimitedDriverVolunteer::hasOrdersLeft() const { return ordersLeft > 0; }

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const {
    return (!isBusy() && hasOrdersLeft() &&
            order.getStatus() == OrderStatus::COLLECTING &&
            order.getDistance() <= getMaxDistance());
}

void LimitedDriverVolunteer::acceptOrder(const Order &order) {
    if (canTakeOrder(order)) {
        int activeOrderId = order.getId();
        setDistance(order.getDistance()); // set distance left
        ordersLeft--;
    }
}
string LimitedDriverVolunteer:: getMyType() const {
    return "LimitedDriver";
}
string LimitedDriverVolunteer::toString() const {
    string s = "volunteerID:" + getId();
    string m = ",ActiveOrder:" + getActiveOrderId();
    return s + m;
};