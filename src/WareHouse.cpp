#pragma once

#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>

#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include "../include/WareHouse.h"

using namespace std;
using std::string;
using std::vector;

WareHouse::WareHouse(const string &configFilePath): isOpen(false), customerCounter(0), orderCounter(0), volunteerCounter(0) {
    parseText(configFilePath);
}

void WareHouse::start() {

    open();
    // read line
    while (isOpen) {
        std::cout << "insert action: " << std::flush;
        string input;
        std::getline(std::cin, input);
        std::stringstream ss(input);
        string action;
        string num;

        ss >> action;
        ss >> num;
        int number = stringToInt(num);
        if (action == "close") {
            isOpen = false; //
        }
        else if (action == "order") {
            if (!isCustomerExist(number))
                std::cout << "There is no customer with customerID=" + num
                          << std::endl;
            else {
                AddOrder order(number);
                order.act(*this);
                std::cout << "COMPLETED" << std::endl;
            }
        } else if (action == "orderStatus") {
            Order order = getOrder(number);
            if (!order.isValid())
                std::cout << "Error: order doesn't exist" << std::endl;
            else {
                PrintOrderStatus print(number);
                print.act(*this);
            }
        } else if (action == "volunteerStatus") {
                if(!isVolunteerExist(number))
                    std::cout << "Error: volunteer doesn't exist" << std::endl;
                else{
                    PrintVolunteerStatus print(number);
                    print.act(*this);
                }
        }
        else if (action == "customerStatus") {
                if(!isCustomerExist(number))
                    std::cout << "Error: customer doesn't exist" << std::endl;
                else{
                    PrintCustomerStatus print(number);
                    print.act(*this);
                }
        }
        else if (action == "step") {} // todo
        
        else if (action == "log") {
            for(BaseAction *action : actionsLog){
               std::cout << action->toString() << std::endl; 
            }
        }
    }
}

    int stringToInt(const std::string &str) {
        std::istringstream iss(str);
        int result = 0;

        if (!(iss >> result)) {
            std::cerr << "Error: Conversion failed." << std::endl;
            // Return a default value (0 in this case) if conversion fails
            return 0;
        }

        return result;
    }
    const vector<BaseAction *> &WareHouse::getActions() const {
        return actionsLog;
    }

    void WareHouse::addOrder(Order *order) {
        int cId = order->getCustomerId(); // customer id
        bool found = false;
        for (Customer *c : customers) {
            if (c->getId() == cId) {
                found = true;
                if (!c->canMakeOrder()) {
                    cout << "Cannot Place This Order" << endl;
                } else {
                    pendingOrders.push_back(order);
                    c->addOrder(order->getId());
                    orderCounter++;
                }
            }
        }
        if (!found)
            cout << "there is no customer with an orderId=" +
                        std::to_string(cId)
                 << endl;
    }
    void WareHouse::addAction(BaseAction * action) {
        actionsLog.push_back(action);
    }

    Customer &WareHouse::getCustomer(int customerId)
        const { 
        for (Customer *c : customers) {
            if (c->getId() == customerId) {
                return *c;
            }
        }
    }

    void WareHouse::AddCustomer(Customer * customer) {
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
        Order *order = nullptr;
        for (Order *o : pendingOrders) {
            if (o->getId() == orderId)
                order = o;
        }
        for (Order *o : inProcessOrders) {
            if (o->getId() == orderId)
                order = o;
        }
        for (Order *o : completedOrders) {
            if (o->getId() == orderId)
                order = o;
        }
        return *order;
    }

    int WareHouse::getOrderCounter() const {
        return orderCounter;
    } 
    int WareHouse::getVolunteerCounter() const{
        return volunteerCounter;
    }
    int WareHouse::getCustomerCounter() const{
        return customerCounter;
    }

    vector<Order*> WareHouse::getVectorOrders(string nameOfVector) const {
        if(nameOfVector == "pendingOrders")
            return pendingOrders;
        else if(nameOfVector == "inProcessOrders")
            return inProcessOrders;
        else
            return completedOrders;
    }

        vector<Volunteer*> WareHouse::getVectorVolunteers() const {
            return volunteers;
    }

        vector<Customer*> WareHouse::getVectorCustomers() const {
            return customers;
    }


    bool WareHouse::isCustomerExist(int customerId) const {
        for (Customer *c : customers) {
            if (c->getId() == customerId) {
                return true;
            }
        }
        return false;
    }

    bool WareHouse::isVolunteerExist(int volunteerId) const {
        for (Volunteer *v : volunteers) {
            if (v->getId() == volunteerId) {
                return true;
            }
        }
        return false;
    }

    void WareHouse::close() {

        for (Order *o : pendingOrders) {
            std::cout << o->toString() << std::endl;
        }
        for (Order *o : inProcessOrders) {
            std::cout << o->toString() << std::endl;
        }
        for (Order *o : completedOrders) {
            std::cout << o->toString() << std::endl;
        }
        isOpen = false;
    }

    void WareHouse::open() {
        isOpen = true;
        std::cout << "WAREHOUSE IS OPEN!" << std::endl;
    }
    

    void WareHouse::printCustomers() {
        for (Customer *c : customers) {
            std::cout << c->getName() << std::endl;
        }
    }

    void WareHouse::parseText(const string &configFilePath) {

        ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            std::cerr << "Error opening the configuration file." << std::endl;
            return;
        }

        customerCounter = 0;
        volunteerCounter = 0;
        std::string line;
        while (getline(configFile, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "customer") {
                // Parse customer
                std::string name;
                std::string customerType;
                int distance;
                int maxOrders;

                iss >> name >> customerType >> distance >> maxOrders;

                Customer *newCustomer = nullptr;
                if (customerType == "soldier") {
                    newCustomer = new SoldierCustomer(customers.size(), name,
                                                      distance, maxOrders);
                } else if (customerType == "civilian") {
                    newCustomer = new CivilianCustomer(customers.size(), name,
                                                       distance, maxOrders);
                } else {
                    std::cerr << "Unknown customer type: " << customerType
                              << std::endl;
                }

                // add customer to warehouse
                if (newCustomer) {
                    AddCustomer(newCustomer);
                }
            } else if (type == "volunteer") {
                // Parse volunteer
                std::string name;
                std::string volunteerType;
                int coolDown;
                int maxDistance;
                int distancePerStep;
                int maxOrders = -1;

                iss >> name >> volunteerType >> coolDown;

                Volunteer *newVolunteer = nullptr;
                if (volunteerType == "collector") {
                    newVolunteer = new CollectorVolunteer(volunteers.size(),
                                                          name, coolDown);
                } else if (volunteerType == "limited_collector") {
                    iss >> maxOrders;
                    newVolunteer = new LimitedCollectorVolunteer(
                        volunteers.size(), name, coolDown, maxOrders);
                } else if (volunteerType == "driver") {
                    iss >> maxDistance >> distancePerStep;
                    newVolunteer = new DriverVolunteer(
                        volunteers.size(), name, maxDistance, distancePerStep);
                } else if (volunteerType == "limited_driver") {
                    iss >> maxDistance >> distancePerStep >> maxOrders;
                    newVolunteer = new LimitedDriverVolunteer(
                        volunteers.size(), name, maxDistance, distancePerStep,
                        maxOrders);
                } else {
                    std::cerr << "Unknown volunteer type: " << volunteerType
                              << std::endl;
                }

                // add to warehouse
                if (newVolunteer) {
                    volunteerCounter++;
                    volunteers.push_back(newVolunteer);
                }
            }
        }

        configFile.close();
    }