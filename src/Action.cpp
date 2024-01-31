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
 
BaseAction::~BaseAction() {};


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
                        // deleteVolunteer(wareHouse, v);
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
                        // deleteVolunteer(wareHouse, v);
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

// void SimulateStep::deleteVolunteer(WareHouse &wareHouse, Volunteer *v){
//     vector<Volunteer*> volunteers = wareHouse.getVectorVolunteers();
//     auto iter = std::find(volunteers.begin(), volunteers.end(), v);
//     if(iter != volunteers.end())
//         volunteers.erase(iter);
//     //destructor 
// };

SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); };
string SimulateStep::toString() const {
    if(getStatus() == ActionStatus::ERROR)
        return "customerStatus " + std::to_string(numOfSteps) + "Error";
    else
        return "customerStatus " + std::to_string(numOfSteps) + "Completed";
 };

//====================================== printActionsLog ===============================


    
    PrintActionsLog::PrintActionsLog() {}

    void PrintActionsLog::act(WareHouse &wareHouse) {
        vector<BaseAction*> actions = wareHouse.getActions();
        for(BaseAction* a : actions){
            std:: cout << a->toString() << std::endl;
        }
    }

    PrintActionsLog* PrintActionsLog::clone() const {
        return new PrintActionsLog(*this);
    }

    string PrintActionsLog::toString() const {
         if(getStatus() == ActionStatus::ERROR)
            return "log ERROR";
        else
            return "log COMPLETED";
    }


//====================================== Close ===============================
   
        Close::Close() {}
        
        void Close::act(WareHouse &wareHouse) {
            wareHouse.close();
        }
        
        Close* Close::clone() const {
            return new Close(*this);
        }
        string Close::toString() const {
            return "Close Completed";
        }

// =================================== AddOrder================================================

AddOrder::AddOrder(int id) : customerId(id) {}
void AddOrder::act(WareHouse &wareHouse) {
    if (!wareHouse.isCustomerExist(customerId) || !wareHouse.getCustomer(customerId).canMakeOrder()) {
        error("Error: Cannot place this order");
        std:: cout << getErrorMsg() << std::endl; }

    else{
        int distance = wareHouse.getCustomer(customerId).getCustomerDistance();
        int newId = wareHouse.getOrderCounter();
        Order *newOrder = new Order(newId, customerId, distance);
        wareHouse.addOrder(newOrder);
    }
}
string AddOrder::toString() const { 
    if(getStatus() == ActionStatus::ERROR)
        return "order" + std::to_string(customerId) + "ERROR";
    else
        return "order " + std::to_string(customerId) + "COMPLETED";
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
    return CustomerType::Civilian;
}

// ================================ printOrderStatus================================

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}
void PrintOrderStatus::act(WareHouse &wareHouse) {
    if(!wareHouse.isOrderExist(orderId)) {
        error("ORDER DOESN'T EXIST"); 
    }
    else {
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
    if(getStatus() == ActionStatus::ERROR)
        return "orderStatus " + std::to_string(orderId) +  "ERROR";
    else
        return "orderStatus " + std::to_string(orderId) + "COMPLETED";
}

//================================== PrintCustomerStatus =======================

PrintCustomerStatus::PrintCustomerStatus(int customerId)
    : customerId(customerId) {}
void PrintCustomerStatus::act(WareHouse &wareHouse) {

    if (!wareHouse.isCustomerExist(customerId))
        error("Customer Doesn't Exist");

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
        return "customerStatus " + std::to_string(customerId) + "ERROR";
    else
        return "customerStatus " + std::to_string(customerId) + "COMPLETED";
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
        if(volunteer->isBusy())
            std::cout << "isBusy: True" << std::endl;  
        else
            std::cout << "isBusy: False" << std::endl;  
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
        return "VolunteerStatus " + std::to_string(volunteerId) + "Error";
    else
        return "VolunteerStatus " + std::to_string(volunteerId) + "Completed";
}

// ============================= BackupWareHousm =======================

BackupWareHouse::BackupWareHouse(){}

void BackupWareHouse::act(WareHouse &wareHouse){
    wareHouse.backUp();
}

BackupWareHouse* BackupWareHouse::clone() const {
    return new BackupWareHouse(*this);
}

string BackupWareHouse::toString() const{
    return "BackupWareHouse COMPLETED";
}

// ============================== RestoreWareHouse =====================
    
    RestoreWareHouse::RestoreWareHouse() {}

    void RestoreWareHouse::act(WareHouse &wareHouse) {
        if(backup == nullptr)
            error("No backup available");
        else
            wareHouse.restore();
    }

    RestoreWareHouse* RestoreWareHouse::clone() const {
        return new RestoreWareHouse(*this);
    }

    string RestoreWareHouse::toString() const {
        if(getStatus() == ActionStatus::ERROR)
            return getErrorMsg();
        else
            return "RestoreWareHouse COMPLETED";
    }
   