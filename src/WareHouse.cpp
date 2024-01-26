#pragma once

#include <vector>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>

#include "../include/Volunteer.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Action.h"
#include "../include/WareHouse.h"

using namespace std;
using std::string;
using std::vector;

WareHouse::WareHouse(const string &configFilePath) {}
const vector<BaseAction *>& WareHouse::getActions() const {
    return actionsLog;
}
void WareHouse::addOrder(Order *order) {
    int cId = order->getCustomerId(); // customer id
    bool found = false;
    for (Customer *c : customers) {
        if (c->getId() == cId){
            found = true;
            if (!c->canMakeOrder()) {
                cout << "Cannot Place This Order" << endl;
            }
            else {
                pendingOrders.push_back(order);
                c->addOrder(order->getId()); // maybe idk
            }
        }
    }
    if (!found)
        cout << "Cannot Place This Order" << endl;
}
void WareHouse::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}
Customer &WareHouse::getCustomer(int customerId) const { // maybe return ????
    for (Customer *c : customers) {
        if (c->getId() == customerId) {
            return *c;
        }
    }
}

void WareHouse::AddCustomer(Customer *customer) {
    customers.push_back(customer);
    customerCounter++;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const { // return what
    for (Volunteer *v : volunteers) {
        if (v->getId() == volunteerId) {
            return *v;
        }
    }
}
Order &WareHouse::getOrder(int orderId) const {
    Order* order = nullptr;
    for(Order *o : pendingOrders){
        if(o->getId() == orderId)
            order = o;
    }
    for(Order *o : inProcessOrders){
        if(o->getId() == orderId)
            order = o;
    }
    for(Order *o : completedOrders){
        if(o->getId() == orderId)
            order = o;  
    } 
    return *order;    
}

void WareHouse::close() {

    for(Order *o : pendingOrders){
        std::cout << o->toString() << std::endl;
    }
    for(Order *o : inProcessOrders){
         std::cout << o->toString() << std::endl;
    }
    for(Order *o : completedOrders){
         std::cout << o->toString() << std::endl;
    }
    isOpen = false;
}

void WareHouse::open()
{
    isOpen = true;
    std::cout <<"WAREHOUSE IS OPEN!" << std::endl;
}

void WareHouse::parseText(const string &configFilePath)
{

    ifstream configFile(configFilePath);
    if (!configFile.is_open())
    {
        std::cerr << "Error opening the configuration file." << std::endl;
        return;
    }

    std::string line;
    while (getline(configFile, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "customer")
        {
            // Parse customer
            std::string name;
            std::string customerType;
            int distance;
            int maxOrders;

            iss >> name >> customerType >> distance >> maxOrders;

            Customer *newCustomer = nullptr;
            if (customerType == "Soldier")
            {
                newCustomer = new SoldierCustomer(customers.size(), name, distance, maxOrders);
            }
            else if (customerType == "Civilian")
            {
                newCustomer = new CivilianCustomer(customers.size(), name, distance, maxOrders);
            }
            else
            {
                std::cerr << "Unknown customer type: " << customerType << std::endl;
            }

            // add customer to warehouse
            if (newCustomer)
            {
                customers.push_back(newCustomer);
            }
        }
        else if (type == "volunteer")
        {
            // Parse volunteer
            std::string name;
            std::string volunteerType;
            int coolDown;
            int maxDistance;
            int distancePerStep;
            int maxOrders = -1;

            iss >> name >> volunteerType >> coolDown;

            Volunteer *newVolunteer = nullptr;
            if (volunteerType == "collector")
            {
                newVolunteer = new CollectorVolunteer(volunteers.size(), name, coolDown);
            }
            else if (volunteerType == "limited_collector")
            {
                iss >> maxOrders;
                newVolunteer = new LimitedCollectorVolunteer(volunteers.size(), name, coolDown, maxOrders);
            }
            else if (volunteerType == "driver")
            {
                iss >> maxDistance >> distancePerStep;
                newVolunteer = new DriverVolunteer(volunteers.size(), name, maxDistance, distancePerStep);
            }
            else if (volunteerType == "limited_driver")
            {
                iss >> maxDistance >> distancePerStep >> maxOrders;
                newVolunteer = new LimitedDriverVolunteer(volunteers.size(), name, maxDistance, distancePerStep, maxOrders);
            }
            else
            {
                std::cerr << "Unknown volunteer type: " << volunteerType << std::endl;
            }

            // add to warehouse
            if (newVolunteer)
            {
                volunteers.push_back(newVolunteer);
            }
        }
    }

    configFile.close();
}