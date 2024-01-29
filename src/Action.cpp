#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include "../include/WareHouse.h"

using std::string;
using std::vector;

// ================================ BaseAction =============

BaseAction::BaseAction() {}

ActionStatus BaseAction::getStatus() const { return status; }

void BaseAction::complete() { status = ActionStatus::COMPLETED; }

void BaseAction::error(string errorMsg) {
    status = ActionStatus::ERROR;
    errorMsg = errorMsg;
}
string BaseAction::getErrorMsg() const { return errorMsg; }

// ===================================SimulateStep=========================================

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
void SimulateStep::act(WareHouse &wareHouse) {
    for (int i = 1; i <= numOfSteps; i++) {

        // ------------ step 1 ------------------
        for (Order *pending : wareHouse.getVectorOrders("pendingOrders")) {
            if (pending->getStatus() == OrderStatus::PENDING)
                findCollector(wareHouse, pending);

            else if (pending->getStatus() == OrderStatus::COLLECTING)
                findDriver(wareHouse, pending);
        }
        // ------------ step 2 ------------------
        for (Volunteer *v : wareHouse.getVectorVolunteers()) {
            if (v->isBusy())
                v->step();
        }

        // ------------ step 3 ------------------
        for (Volunteer *v : wareHouse.getVectorVolunteers()) {
            if (v->isBusy()) {
                Order order = wareHouse.getOrder((v->getActiveOrderId()));


                if (v->getMyType() == "collector") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                        wareHouse.addToVector("pendingOrders", &order);
                        wareHouse.removeFromVector("inProcessOrders", &order);
                    }

                } else if (v->getMyType() == "limitedCollector") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                    }
                    wareHouse.addToVector("pendingOrders", &order);
                    wareHouse.removeFromVector("inProcessOrders", &order);

                    if (dynamic_cast<LimitedCollectorVolunteer *>(v)
                            ->getNumOrdersLeft() == 0) {
                        // erase from volunteer vector
                        // delete volunteer with ~
                    }
                }

                else if (v->getMyType() == "driver") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                    }
                    wareHouse.addToVector("completedOrders", &order);
                    wareHouse.removeFromVector("inProcessOrders", &order);
                    order.setStatus(OrderStatus::COMPLETED);


                } else if (v->getMyType() == "limitedDriver") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                    }
                    wareHouse.addToVector("completedOrders", &order);
                    wareHouse.removeFromVector("inProcessOrders", &order);
                    order.setStatus(OrderStatus::COMPLETED);

                    if (dynamic_cast<LimitedDriverVolunteer *>(v)
                            ->getNumOrdersLeft() == 0) {
                        // erase from volunteer vector
                        // delete volunteer with ~
                    }
                }
            }
        }
    }
}


void SimulateStep::findDriver(WareHouse &wareHouse, Order *order) {
    for (Volunteer *v : wareHouse.getVectorVolunteers()) {
        if (v->getMyType() == "driver" || v->getMyType() == "limitedDriver") {
            if (v->canTakeOrder(*order)) {
                v->acceptOrder(*order);
                order->setDriverId(v->getId());
                vector<Order *> inProcess =
                    wareHouse.getVectorOrders("inProcessOrders");
                inProcess.push_back(order);
                // wareHouse.getVectorOrders("pendingOrders").erase(order);
                order->setStatus(OrderStatus::DELIVERING);
            }
        }
    }
}
void SimulateStep::findCollector(WareHouse &wareHouse, Order *order) {
    for (Volunteer *v : wareHouse.getVectorVolunteers()) {
        if (v->getMyType() == "collector" ||
            v->getMyType() == "limitedCollector") {
            if (v->canTakeOrder(*order)) {
                v->acceptOrder(*order);
                order->setCollectorId(v->getId());
                wareHouse.addToVector("inProcessOrders", order);
                wareHouse.removeFromVector("pendingOrders", order);
                order->setStatus(OrderStatus::COLLECTING);
            }
        }
    }
}
SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); };
string SimulateStep::toString() const {
    if(getStatus() == ActionStatus::ERROR)
        return "customerStatus " + std::to_string(numOfSteps) + "Error";
    else
        return "customerStatus " + std::to_string(numOfSteps) + "Completed";
 };


//====================================== Close ===============================
//    
//         Close(){}
//         void act(WareHouse &wareHouse) {}
//         Close *clone() const override
//         string toString() const override;
//     
//

// =================================== AddOrder================================================

AddOrder::AddOrder(int id) : customerId(id) {}
void AddOrder::act(WareHouse &wareHouse) {

    int distance = wareHouse.getCustomer(customerId).getCustomerDistance();
    int newId = wareHouse.getOrderCounter();
    Order *newOrder = new Order(newId, customerId, distance);
    wareHouse.addOrder(newOrder);
}
string AddOrder::toString() const { 
  return "order was added";
}
AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

// ================================== AddCustomer==========================================

AddCustomer::AddCustomer(const string &customerName, const string &customerType,
                         int distance, int maxOrders)
    : customerName(customerName), customerType(stringToType(customerType)),
      distance(distance), maxOrders(maxOrders) {}
void AddCustomer::act(WareHouse &wareHouse) {
    int newId = wareHouse.getOrderCounter();
    if (customerType == CustomerType::Soldier) {
        SoldierCustomer newCustomer(newId, customerName, distance, maxOrders);
        wareHouse.AddCustomer(&newCustomer);
    }
    if (customerType == CustomerType::Civilian) {
        CivilianCustomer newCustomer(newId, customerName, distance, maxOrders);
        wareHouse.AddCustomer(&newCustomer);
    }
}
AddCustomer *AddCustomer::clone() const { return new AddCustomer(*this); }
string AddCustomer::toString() const { return "Customer was added"; }
CustomerType AddCustomer::stringToType(const string &customerType) {
    if (customerType == "soldier")
        return CustomerType::Soldier;

    else if (customerType == "civilian")
        return CustomerType::Civilian;
}

// ================================ printOrderStatus================================

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
void PrintOrderStatus::act(WareHouse &wareHouse) {
    Order order = wareHouse.getOrder(orderId);

    if (!order.isValid() && wareHouse.isOrderExist(orderId))
        error("ORDER DOESN'T EXIST");

    string CustomerId = "" + std::to_string(order.getCustomerId());
    string status = order.getStatusString();
    string collectorId;
    string driverId;

    if (order.getDriverId() == -1)
        driverId = "NONE";
    else
        driverId = std::to_string(order.getDriverId());

    if (order.getCollectorId() == -1)
        collectorId = "NONE";
    else
        collectorId = std::to_string(order.getCollectorId());

    std::cout << order.toString() << std::endl;
    std::cout << "OrderStatus:" + status << std::endl;
    std::cout << "CustomerID:" + CustomerId << std::endl;
    std::cout << "Collector:" + collectorId << std::endl;
    std::cout << "Driver:" + driverId << std::endl;
}
PrintOrderStatus *PrintOrderStatus::clone() const {
    return new PrintOrderStatus(*this);
}
string PrintOrderStatus::toString() const {
    if(getStatus() == ActionStatus::ERROR)
        return "orderStatus " + std::to_string(orderId) +  "Error";
    else
        return "orderStatus " + std::to_string(orderId) + "Completed";
}

//================================== PrintCustomerStatus =======================

PrintCustomerStatus::PrintCustomerStatus(int customerId)
    : customerId(customerId) {}
void PrintCustomerStatus::act(WareHouse &wareHouse) {

    if (!wareHouse.isCustomerExist(customerId))
        error("Customer Doesnt Exist");

    else {
        Customer *customer = wareHouse.getCustomer(customerId).clone();
        for (int id : customer->getOrdersIds()) {
            std::cout << wareHouse.getOrder(id).toString() << std::endl;
        }
        int numOfOrderLeft =
            (customer->getMaxOrders()) - (customer->getNumOrders());
        std::cout << "numOrderLeft:" + std::to_string(numOfOrderLeft)
                  << std::endl;
    }
}
string PrintCustomerStatus::toString() const {
    if(getStatus() == ActionStatus::ERROR)
        return "customerStatus " + std::to_string(customerId) + "Error";
    else
        return "customerStatus " + std::to_string(customerId) + "Completed";
}
PrintCustomerStatus *PrintCustomerStatus::clone() const {
    return new PrintCustomerStatus(*this);
}

//================================== printVolunteerStatus ======================

PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id) {}
void PrintVolunteerStatus::act(WareHouse &wareHouse) {

    if (!wareHouse.isVolunteerExist(volunteerId))
        error("Volunteer Doesn't Exist");

    else {
        Volunteer *volunteer = wareHouse.getVolunteer(volunteerId).clone();
        string type = volunteer->getMyType();
        string timeLeft = "None";
        string orderLeft = "No Limit";
        string orderId = "None";

        // order id
        if (volunteer->isBusy())
            orderId = std::to_string(volunteer->getActiveOrderId());

        // time\distance left
        if (type == "collector") {
            timeLeft = std::to_string(
                dynamic_cast<CollectorVolunteer *>(volunteer)->getTimeLeft());
        } else if (type == "driver" && volunteer->isBusy()) {
            timeLeft = std::to_string(
                dynamic_cast<DriverVolunteer *>(volunteer)->getDistanceLeft());
        }

        else if (type == "limitedCollector") {
            timeLeft = std::to_string(
                dynamic_cast<CollectorVolunteer *>(volunteer)->getTimeLeft());
            orderLeft = std::to_string(
                dynamic_cast<LimitedCollectorVolunteer *>(volunteer)
                    ->getNumOrdersLeft()); // casting
        }

        else if (type == "limitedDriver") {
            if (volunteer->isBusy())
                timeLeft = std::to_string(dynamic_cast<DriverVolunteer *>(volunteer)
                                          ->getDistanceLeft());
            orderLeft =
                std::to_string(dynamic_cast<LimitedDriverVolunteer *>(volunteer)
                                   ->getNumOrdersLeft());
        }

        std::cout << "VolunteerID: " + std::to_string(volunteerId) << std::endl;
        std::cout << "isBusy: " + volunteer->isBusy() << std::endl;
        std::cout << "OrderID: " + orderId << std::endl;
        std::cout << "TimeLeft: " + timeLeft << std::endl;
        std::cout << "ordersLeft: " + orderLeft << std::endl;
    }
}
PrintVolunteerStatus *PrintVolunteerStatus::clone() const {
    return new PrintVolunteerStatus(*this);
}
string PrintVolunteerStatus::toString() const {
    if(getStatus() == ActionStatus::ERROR)
        return "volunteerStatus " + std::to_string(volunteerId) + "Error";
    else
        return "volunteerStatus " + std::to_string(volunteerId) + "Completed";
}

//============================= BackupWareHousm =======================

// BackupWareHouse::BackupWareHouse(){}
// void BackupWareHouse::act(WareHouse &wareHouse){
//     bool isOpen = wareHouse.isOpen();

//     WareHouse backUp = new WareHouse(isOpen,);
// }
// BackupWareHouse* BackupWareHouse::clone() const;
// string BackupWareHouse::toString() const;

// ============================== RestoreWareHouse =====================
//     
//         RestoreWareHouse();
//         void act(WareHouse &wareHouse) override;
//         RestoreWareHouse *clone() const override;
//         string toString() const override;
//    