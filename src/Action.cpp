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

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR) {}

ActionStatus BaseAction::getStatus() const { return status; }

void BaseAction::complete() { status = ActionStatus::COMPLETED; }

void BaseAction::error(string errorMsg) {
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
}
string BaseAction::getErrorMsg() const { return errorMsg; }

BaseAction::~BaseAction(){};

// ===================================SimulateStep=========================================

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}
void SimulateStep::act(WareHouse &wareHouse) {
    for (int i = 1; i <= numOfSteps; i++) {
       
        // ------------ step 1 ------------------
        
        for (Order *pending : wareHouse.getVectorOrders("pendingOrders")) {
            if (pending->getStatus() == OrderStatus::PENDING)
                wareHouse.findCollector(pending);

            else if (pending->getStatus() == OrderStatus::COLLECTING)
                wareHouse.findDriver(pending);
        }
        
        // ------------ step 2 ------------------
        
            for (Volunteer *v : wareHouse.getVectorVolunteers()) {
                if (v->isBusy())
                    v->step();
            }
        
        
        // ------------ step 3 ------------------

        for (Volunteer *v : wareHouse.getVectorVolunteers()) {
            if (v->isBusy()) {
                Order *order = &(wareHouse.getOrder((v->getActiveOrderId())));
                if (v->getMyType() == "collector") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                        wareHouse.addToVector("pendingOrders", order);
                        wareHouse.removeFromVector("inProcessOrders", *order);
                    }

                } else if (v->getMyType() == "limitedCollector") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                        wareHouse.addToVector("pendingOrders", order);
                        wareHouse.removeFromVector("inProcessOrders", *order);

                    if (!(dynamic_cast<LimitedCollectorVolunteer *>(v)->hasOrdersLeft()))
                        wareHouse.deleteVolunteer(v);
                    }
                }

                else if (v->getMyType() == "driver") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();
                        wareHouse.addToVector("completedOrders", order);
                        wareHouse.removeFromVector("inProcessOrders", *order);
                        (*order).setStatus(OrderStatus::COMPLETED);
                    }

                } else if (v->getMyType() == "limitedDriver") {
                    if (v->getCompletedOrderId() == v->getActiveOrderId()) {
                        v->resetActiveOrderId();

                        wareHouse.addToVector("completedOrders", order);
                        wareHouse.removeFromVector("inProcessOrders", *order);
                        (*order).setStatus(OrderStatus::COMPLETED);
                    

                    if (!(dynamic_cast<LimitedDriverVolunteer *>(v)->hasOrdersLeft())) {
                        wareHouse.deleteVolunteer(v);
                    }
                    }
                }
            }
        }
    }
    
    complete();
}

SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); };

string SimulateStep::toString() const {
    return "simulateStep " + std::to_string(numOfSteps) + " COMPLETED";
};

//======================================
//printActionsLog===============================

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(WareHouse &wareHouse) {
    vector<BaseAction *> actions = wareHouse.getActions();
    for (BaseAction *a : actions) {
        std::cout << a->toString() << std::endl;
    }
    complete();
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

string PrintActionsLog::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "log ERROR";
    else
        return "log COMPLETED";
}

//====================================== Close ===============================

Close::Close() {}

void Close::act(WareHouse &wareHouse) {
    wareHouse.close();
    complete();
}

Close *Close::clone() const { return new Close(*this); }
string Close::toString() const { return "Close Completed"; }

// =================================== AddOrder===============================

AddOrder::AddOrder(int id) : customerId(id) {}
void AddOrder::act(WareHouse &wareHouse) {
    if (!wareHouse.isCustomerExist(customerId) ||
     !wareHouse.getCustomer(customerId).canMakeOrder()) {
        error("Error: Cannot place this order");
        std::cout << getErrorMsg() << std::endl;
    }

    else {
        int distance = wareHouse.getCustomer(customerId).getCustomerDistance();
        int newId = wareHouse.getOrderCounter();
        Order *newOrder = new Order(newId, customerId, distance);
        wareHouse.addOrder(newOrder);
        wareHouse.getCustomer(customerId).addOrder(newId);
        complete();
    }
}
string AddOrder::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "order " + std::to_string(customerId) + " ERROR";
    else
        return "order " + std::to_string(customerId) + " COMPLETED";
}
AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

// ==================================
// AddCustomer==========================================

AddCustomer::AddCustomer(const string &customerName, const string &customerType,
                         int distance, int maxOrders)
    : customerName(customerName), customerType(stringToType(customerType)),
      distance(distance), maxOrders(maxOrders) {}

void AddCustomer::act(WareHouse &wareHouse) {
    int newId = wareHouse.getCustomerCounter();
    if (customerType == CustomerType::Soldier) {
        SoldierCustomer *newCustomer = new SoldierCustomer(newId, customerName, distance, maxOrders);
        wareHouse.addCustomer(newCustomer);
    }
    if (customerType == CustomerType::Civilian) {
        CivilianCustomer *newCustomer = new CivilianCustomer(newId, customerName, distance, maxOrders);
        wareHouse.addCustomer(newCustomer);
    }
    complete();
}

AddCustomer *AddCustomer::clone() const { return new AddCustomer(*this); }

string AddCustomer::toString() const { 
    string type;
    if(customerType == CustomerType::Soldier)
        type = " solidier";
    else   
        type = " civilian";

    return "customer " + customerName + type + " " + std::to_string(distance) + " " + std::to_string(maxOrders)  + " COMPLETED" ; 
    }

CustomerType AddCustomer::stringToType(const string &customerType) {
    if (customerType == "soldier")
        return CustomerType::Soldier;
    return CustomerType::Civilian;
}

// ================================
// printOrderStatus================================

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
void PrintOrderStatus::act(WareHouse &wareHouse) {
    if (!wareHouse.isOrderExist(orderId)) {
        error("ORDER DOESN'T EXIST");
    } else {
        Order order = wareHouse.getOrder(orderId);
        string CustomerId = "" + std::to_string(order.getCustomerId());
        string status = order.getStatusString();
        string collectorId;
        string driverId;

        if (order.getDriverId() == NO_VOLUNTEER)
            driverId = "None";
        else
            driverId = std::to_string(order.getDriverId());

        if (order.getCollectorId() == NO_VOLUNTEER)
            collectorId = "None";
        else
            collectorId = std::to_string(order.getCollectorId());

        complete();

        std::cout << "OrderId: " + std::to_string(orderId) << std::endl;
        std::cout << "OrderStatus: " + status << std::endl;
        std::cout << "CustomerID: " + CustomerId << std::endl;
        std::cout << "Collector: " + collectorId << std::endl;
        std::cout << "Driver: " + driverId << std::endl;
    }
}
PrintOrderStatus *PrintOrderStatus::clone() const {
    return new PrintOrderStatus(*this);
}
string PrintOrderStatus::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "orderStatus " + std::to_string(orderId) + " ERROR";
    else
        return "orderStatus " + std::to_string(orderId) + " COMPLETED";
}

//================================== PrintCustomerStatus =======================

PrintCustomerStatus::PrintCustomerStatus(int customerId)
    : customerId(customerId) {}
void PrintCustomerStatus::act(WareHouse &wareHouse) {

    if (!wareHouse.isCustomerExist(customerId))
        error("Customer Doesn't Exist");

    else {
        std::cout << "CustomerID: " + std::to_string(customerId) << std::endl;
        Customer *customer = wareHouse.getCustomer(customerId).clone();
        for (int id : customer->getOrdersIds()) {
            std::cout <<"OrderID: " + std::to_string(id) << std::endl;
            std::cout << "OrderStatus: " + wareHouse.getOrder(id).getStatusString() << std::endl;
        }
        int numOfOrderLeft = (customer->getMaxOrders()) - (customer->getNumOrders());
        std::cout << "numOrderLeft: " + std::to_string(numOfOrderLeft) << std::endl;

        complete();
    }
    
}
string PrintCustomerStatus::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "customerStatus " + std::to_string(customerId) + " ERROR";
    else
        return "customerStatus " + std::to_string(customerId) + " COMPLETED";
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
        complete();
        std::cout << volunteer->toString() << std::endl;
    }
}
PrintVolunteerStatus *PrintVolunteerStatus::clone() const {
    return new PrintVolunteerStatus(*this);
}
string PrintVolunteerStatus::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "VolunteerStatus " + std::to_string(volunteerId) + " ERROR";
    else
        return "VolunteerStatus " + std::to_string(volunteerId) + " COMPLETED";
}

// ============================= BackupWareHousm =======================

BackupWareHouse::BackupWareHouse() {}

void BackupWareHouse::act(WareHouse &wareHouse) {
    wareHouse.backUp();
    complete();
}

BackupWareHouse *BackupWareHouse::clone() const {
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const { 
    return "BackupWareHouse COMPLETED";
}

// ============================== RestoreWareHouse =====================

RestoreWareHouse::RestoreWareHouse() {}

void RestoreWareHouse::act(WareHouse &wareHouse) {
    if (backup == nullptr) {
        error("No backup available");
        std:: cout << getErrorMsg() << std::endl; 
    }
    else {
        wareHouse.restore();
        complete();
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const {
    return new RestoreWareHouse(*this);
}

string RestoreWareHouse::toString() const {
    if (getStatus() == ActionStatus::ERROR)
        return "RestoreWareHouse ERROR";
    else
        return "RestoreWareHouse COMPLETED";
}
