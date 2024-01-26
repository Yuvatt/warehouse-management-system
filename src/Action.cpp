#pragma once
#include <string>
#include <vector>
#include <iostream>

#include "../include/WareHouse.h"
#include "../include/Volunteer.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Action.h"

using std::string;
using std::vector;

// BaseAction

BaseAction::BaseAction() {}

ActionStatus BaseAction::getStatus() const
{
    return status;
}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    errorMsg = errorMsg;
}

string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

// AddOrder

AddOrder::AddOrder(int id) : customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{
    int distance = wareHouse.getCustomer(customerId).getCustomerDistance();
    int newId = orderCounter+1;
    Order newOrder(newId, customerId, distance);
    wareHouse.addOrder(&newOrder);
}

string AddOrder::toString() const
{
    return "order been placed";
}

AddOrder* AddOrder::clone() const
{
    return new AddOrder(*this);
}

// AddCustomer

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders) :
customerName(customerName), customerType(stringToType(customerType)),distance(distance), maxOrders(maxOrders) {}

void AddCustomer::act(WareHouse &wareHouse) {
    int newId = customerCounter +1;
    if (customerType == CustomerType::Soldier) {
        SoldierCustomer newCustomer(newId, customerName, distance,maxOrders);
        wareHouse.AddCustomer(&newCustomer);
    }
    if (customerType == CustomerType::Civilian) {
        CivilianCustomer newCustomer(newId, customerName, distance,maxOrders);
        wareHouse.AddCustomer(&newCustomer);
    }
}

AddCustomer* AddCustomer::clone() const {
    return new AddCustomer(*this);
}
string AddCustomer::toString() const {
    return "Customer was added";
}

CustomerType stringToType(const string &customerType)
{
    if (customerType == "soldier")
        return CustomerType::Soldier;

    else if (customerType == "civilian")
        return CustomerType::Civilian;
}

//printOrderStatus

PrintOrderStatus::PrintOrderStatus(int id): orderId(id) {}

void PrintOrderStatus:: act(WareHouse &wareHouse) {
    Order order = wareHouse.getOrder(orderId);
     if(order.isValid()) 
         std::cout << "ORDER DOESN'T EXIST" << std::endl;

    string CustomerId = "" + std::to_string(order.getCustomerId()); 

    string status;
    string collectorId;
    string driverId;

    if(order.getStatus() == OrderStatus::COLLECTING){
        status = "COLLECTING";
    }
    else if(order.getStatus() == OrderStatus::DELIVERING){
        status = "DELIVERING";
    }
    else if(order.getStatus() == OrderStatus::PENDING){
        status = "PENDING";
    }
    else if(order.getStatus() == OrderStatus::COMPLETED){
        status = "COMPLETED";
    }

    if(order.getDriverId() == -1)
        driverId = "NONE";
    else 
        driverId = std::to_string(order.getDriverId());

     if(order.getCollectorId() == -1)
        collectorId = "NONE";
    else 
        collectorId = std::to_string(order.getCollectorId());

    string output = order.toString() + 
    "OrderStatus:" + status +
    "CustomerID:" + CustomerId +
    "Collector:" + collectorId +
    "Driver:" + driverId; 
}

PrintOrderStatus* PrintOrderStatus::clone() const {
    return new PrintOrderStatus(*this);
 }
 
string PrintOrderStatus::toString() const {

}