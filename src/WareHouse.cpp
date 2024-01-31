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

WareHouse::WareHouse(const string &configFilePath) 
    : isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), 
     customers(), customerCounter(0), volunteerCounter(0), orderCounter(0){
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
            Close* close = new Close();
            close->act(*this);
            addAction(close);
        }        

        else if (action == "order") {
            AddOrder order(number);
            order.act(*this);
            addAction(&order);
        } 
        
        else if (action == "orderStatus") {
            PrintOrderStatus print(number);
            print.act(*this);
            addAction(&print);
        }

        else if (action == "volunteerStatus") {
            PrintVolunteerStatus print(number);
            print.act(*this);
            addAction(&print);
        }

        else if (action == "customerStatus") {
            PrintCustomerStatus print(number);
            print.act(*this);
            addAction(&print);


        } else if (action == "step") {
            int numberOfSteps = stringToInt(num);
            SimulateStep step(numberOfSteps);
            step.act(*this);
            addAction(&step);
        }

        else if (action == "log") {
            PrintActionsLog *print = new PrintActionsLog();
            print->act(*this);   
            addAction(print);
        }
    }
}

void WareHouse::addOrder(Order *order) {
    pendingOrders.push_back(order);
    orderCounter++;
}
    
 
void WareHouse::addAction(BaseAction *action) { actionsLog.push_back(action); }
void WareHouse::AddCustomer(Customer *customer) {
    customers.push_back(customer);
    customerCounter++;
}

Customer &WareHouse::getCustomer(int customerId) const {
    for (Customer *c : customers) {
        if (c->getId() == customerId) {
            return *c;
        }
    }
    SoldierCustomer *noOne = new SoldierCustomer(-1, "No Customer", -1, -1);
    return *noOne;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const {
    for (Volunteer *v : volunteers) {
        if (v->getId() == volunteerId) {
            return *v;
        }
    }
    CollectorVolunteer *noOne = new CollectorVolunteer(-1, "No Volunteer", -1);
    return *noOne; 
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
int WareHouse::getOrderCounter() const { return orderCounter; }
int WareHouse::getVolunteerCounter() const { return volunteerCounter; }
int WareHouse::getCustomerCounter() const { return customerCounter; }

vector<Order *> WareHouse::getVectorOrders(string nameOfVector) const {
    if (nameOfVector == "pendingOrders")
        return pendingOrders;
    else if (nameOfVector == "inProcessOrders")
        return inProcessOrders;
    else
        return completedOrders;
}
vector<Volunteer *> WareHouse::getVectorVolunteers() {
    return volunteers;
}
vector<Customer *> WareHouse::getVectorCustomers() const { return customers; }

const vector<BaseAction *> &WareHouse::getActions() const { return actionsLog; }
void WareHouse:: findCollector(Order *order){
    bool found = false;
    std::vector<Volunteer *>::iterator it; 
    for (it = volunteers.begin(); it != volunteers.end() && !found; it++) {
        Volunteer *v = (*it);

        if (v->getMyType() == "collector" ||
            v->getMyType() == "limitedCollector") {
            if (v->canTakeOrder(*order)) {
                v->acceptOrder(*order);
                order->setCollectorId(v->getId());
                inProcessOrders.push_back(order);
                removeFromVector("pendingOrders", *order);
                order->setStatus(OrderStatus::COLLECTING);
                found = true;
            }
        }
    }
}
void WareHouse:: findDriver (Order *order){
    bool found = false;
    std::vector<Volunteer *>::iterator it;
    for (it = volunteers.begin();
         it != volunteers.end() && !found; it++) {
        Volunteer *v = (*it);
        if (v->getMyType() == "driver" || v->getMyType() == "limitedDriver") {
            if (v->canTakeOrder(*order)) {
                v->acceptOrder(*order);
                order->setDriverId(v->getId());
                inProcessOrders.push_back(order);
                // wareHouse.getVectorOrders("pendingOrders").erase(order);
                order->setStatus(OrderStatus::DELIVERING);
                found = true;
            }
        }
    }
}

void WareHouse::addToVector(string nameOfVector, Order &order) {
    if (nameOfVector == "pendingOrders")
        pendingOrders.push_back(&order);
    else if (nameOfVector == "inProcessOrders")
        inProcessOrders.push_back(&order);
    else
        completedOrders.push_back(&order);
}
void WareHouse::removeFromVector(string nameOfVector, Order &order) {
    if (nameOfVector == "pendingOrders") {
        std::vector<Order *>::iterator it = pendingOrders.begin();
        while (it != pendingOrders.end()) { 
            if ((*it)->getId() == order.getId())
                pendingOrders.erase(it);
            else
                ++it;
        }
    }

    else if (nameOfVector == "inProcessOrders")
        inProcessOrders.push_back(&order);
    else
        completedOrders.push_back(&order);
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
bool WareHouse::isOrderExist(int orderId) const {
    for (Order *o : pendingOrders) {
        if (o->getId() == orderId)
            return true;
    }
    for (Order *o : inProcessOrders) {
        if (o->getId() == orderId)
            return true;
    }
    for (Order *o : completedOrders) {
        if (o->getId() == orderId)
            return true;
    }
    return false;
}

void WareHouse::backUp() {
    if(backup == nullptr)
        backup = new WareHouse(*this);
    else
        *backup = *this;
}

void WareHouse::restore(){
    *this = *backup;
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
                newVolunteer =
                    new CollectorVolunteer(volunteers.size(), name, coolDown);
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

