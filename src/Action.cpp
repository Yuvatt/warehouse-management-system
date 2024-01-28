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

// BaseAction

BaseAction::BaseAction() {}

ActionStatus BaseAction::getStatus() const { return status; }

void BaseAction::complete() { status = ActionStatus::COMPLETED; }

void BaseAction::error(string errorMsg) {
    status = ActionStatus::ERROR;
    errorMsg = errorMsg;
}

string BaseAction::getErrorMsg() const { return errorMsg; }

// ===================================SimulateStep=============================================

SimulateStep::SimulateStep(int numOfSteps): numOfSteps(numOfSteps) {} 

void SimulateStep::act(WareHouse &wareHouse) {
    for (Order *pending : wareHouse.getVectorOrders("pendingOrders")) {
           // if (*pending.getStatusString() == "Pending")  ///////// first step
           // else if (*pending->getStatusString == "Collecting")
           // else         
        }

    for(Volunteer *v : wareHouse.getVectorVolunteers()){ 
        v->step(); 
        // if()
    }

};

string SimulateStep::toString() const {};

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
};



// ===================================AddOrder=================================================

AddOrder::AddOrder(int id) : customerId(id) {}

void AddOrder::act(WareHouse &wareHouse) {

    int distance = wareHouse.getCustomer(customerId).getCustomerDistance();
    int newId = wareHouse.getOrderCounter();
    Order* newOrder = new Order(newId, customerId, distance);
    wareHouse.addOrder(newOrder);
}

string AddOrder::toString() const { return "order been placed"; }

AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

// AddCustomer

AddCustomer::AddCustomer(const string &customerName, const string &customerType,
                         int distance, int maxOrders)
    :customerName(customerName), customerType(stringToType(customerType)),
      distance(distance), maxOrders(maxOrders) {
      }

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

// printOrderStatus

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse) {
    Order order = wareHouse.getOrder(orderId);
    if (!order.isValid())
        std::cout << "ORDER DOESN'T EXIST" << std::endl;

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

string PrintOrderStatus::toString() const {}

// PrintCustomerStatus

PrintCustomerStatus::PrintCustomerStatus(int customerId)
    : customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse) {

    if(!wareHouse.isCustomerExist(customerId))
        std::cout << "Customer Doesnt Exist" << std::endl;
    
    else{    
    Customer* customer = wareHouse.getCustomer(customerId).clone();
    //מצביע ללקוח, יוצרת לקוח חדש ששווה לאחד שאני מביאה

    for (int id :customer->getOrdersIds()) {
       std::cout<< wareHouse.getOrder(id).toString() << std::endl;
    }
    int numOfOrderLeft = (customer->getMaxOrders()) - (customer->getNumOrders());
    std::cout<< "numOrderLeft:" + std::to_string(numOfOrderLeft) << std::endl;
    }
}
string PrintCustomerStatus::toString()const{}
PrintCustomerStatus *PrintCustomerStatus::clone() const {}

//printVolunterrStatus

PrintVolunteerStatus::PrintVolunteerStatus(int id): volunteerId(id) {}
void PrintVolunteerStatus::act(WareHouse &wareHouse){

    if(!wareHouse.isVolunteerExist(volunteerId))
        std::cout << "Volunteer Doesn't Exist" << std::endl;
    else{
    Volunteer* volunteer = wareHouse.getVolunteer(volunteerId).clone();
    string type = volunteer->getMyType();
    if(type == "collector" || type == "driver")
        string orderLeft = "NO LIMIT";
    else {} //meed to continue

    std::cout << "VolunteerID:" + std::to_string(volunteerId) << std::endl;
    std::cout << "isBusy:" + std::to_string(volunteer->isBusy()) << std::endl;
    std::cout << "OrderID:" + std::to_string(volunteer->getActiveOrderId()) << std::endl;
       
    } 
}
PrintVolunteerStatus* PrintVolunteerStatus:: clone() const {}
string PrintVolunteerStatus:: toString() const {}

//=============================BackupWareHouse==================================

        // BackupWareHouse::BackupWareHouse(){}
        // void BackupWareHouse::act(WareHouse &wareHouse){
        //     bool isOpen = wareHouse.isOpen();
            
        //     WareHouse backUp = new WareHouse(isOpen,);
        // }
        // BackupWareHouse* BackupWareHouse::clone() const;
        // string BackupWareHouse::toString() const;

