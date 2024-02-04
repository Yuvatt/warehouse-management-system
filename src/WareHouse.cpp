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
    : isOpen(false), actionsLog(), volunteers(), pendingOrders(),
      inProcessOrders(), completedOrders(), customers(), customerCounter(0),
      volunteerCounter(0), orderCounter(0) {

    // Parse the file and populate the vectors
    std::ifstream inputFile(configFilePath);
    if (!inputFile.is_open() || inputFile.fail()) {
        std::cout << "Configuration file is not accessible or empty, starting "
                     "with empty configuration"
                  << std::endl;
    }
    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string category;
        iss >> category;

        if (category == "customer") {
            std::string name, type;
            int distance, maxOrders;
            iss >> name >> type >> distance >> maxOrders;

            if (type == "soldier") {
                SoldierCustomer *temp = new SoldierCustomer(
                    getCustomerCounter(), name, distance, maxOrders);
                customers.push_back(temp);
                temp = nullptr;
                customerCounter++;
            }

            else if (type == "civilian") {
                CivilianCustomer *temp = new CivilianCustomer(
                    getCustomerCounter(), name, distance, maxOrders);
                customers.push_back(temp);
                temp = nullptr;
                customerCounter++;
            }

        }

        else if (category == "volunteer") {
            std::string name, type;
            iss >> name >> type;

            if (type == "collector") {
                int coolDown;
                iss >> coolDown;
                CollectorVolunteer *temp = new CollectorVolunteer(
                    getVolunteerCounter(), name, coolDown);
                volunteers.push_back(temp);
                temp = nullptr;
                volunteerCounter++;
            }

            else if (type == "limited_collector") {
                int coolDown, maxOrders;
                iss >> coolDown >> maxOrders;
                LimitedCollectorVolunteer *temp = new LimitedCollectorVolunteer(
                    getVolunteerCounter(), name, coolDown, maxOrders);
                volunteers.push_back(temp);
                temp = nullptr;
                volunteerCounter++;

            }

            else if (type == "driver") {
                int maxDistance, distance_per_step;
                iss >> maxDistance >> distance_per_step;
                DriverVolunteer *temp =
                    new DriverVolunteer(getVolunteerCounter(), name,
                                        maxDistance, distance_per_step);
                volunteers.push_back(temp);
                temp = nullptr;
                volunteerCounter++;

            }

            else if (type == "limited_driver") {
                int maxDistance, distance_per_step, maxOrders;
                iss >> maxDistance >> distance_per_step >> maxOrders;
                LimitedDriverVolunteer *temp = new LimitedDriverVolunteer(
                    getVolunteerCounter(), name, maxDistance, distance_per_step,
                    maxOrders);
                volunteers.push_back(temp);
                temp = nullptr;
                volunteerCounter++;
            }
        }
    };

    inputFile.close(); // release the file resource
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
        int number;

        ss >> action;
        if (action == "close") {
            Close *close = new Close();
            close->act(*this);
            addAction(close);
        }

        else if (action == "log") {
            PrintActionsLog *print = new PrintActionsLog();
            print->act(*this);
            addAction(print);
        }

        else if (action == "backup") {
            BackupWareHouse *backup = new BackupWareHouse();
            addAction(backup);
            backup->act(*this);
        } else if (action == "restore") {
            RestoreWareHouse *restore = new RestoreWareHouse();
            restore->act(*this);
            addAction(restore);
        }

        else if (action == "customer") {
            string name, type;
            int distance, maxOrders;
            ss >> name >> type >> distance >> maxOrders;
            AddCustomer *cu = new AddCustomer(name, type, distance,maxOrders);
            cu->act(*this);
            addAction(cu);

        } else if (action == "order") {
            ss >> number;
            AddOrder *order = new AddOrder(number);
            order->act(*this);
            addAction(order);
        }

        else if (action == "orderStatus") {
            ss >> number;
            PrintOrderStatus *print = new PrintOrderStatus(number);
            print->act(*this);
            addAction(print);
        }

        else if (action == "volunteerStatus") {
            ss >> number;
            PrintVolunteerStatus *print = new PrintVolunteerStatus(number);
            print->act(*this);
            addAction(print);
        }

        else if (action == "customerStatus") {
            ss >> number;
            PrintCustomerStatus print(number);
            print.act(*this);
            addAction(&print);

        } else if (action == "step") {
            ss >> number;
            SimulateStep *step = new SimulateStep(number);
            step->act(*this);
            addAction(step);
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
vector<Volunteer *> WareHouse::getVectorVolunteers() { return volunteers; }
vector<Customer *> WareHouse::getVectorCustomers() const { return customers; }
const vector<BaseAction *> &WareHouse::getActions() const { return actionsLog; }

void WareHouse::findCollector(Order *order) {
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
void WareHouse::findDriver(Order *order) {
    bool found = false;
    std::vector<Volunteer *>::iterator it;
    for (it = volunteers.begin(); it != volunteers.end() && !found; it++) {
        Volunteer *v = (*it);

        if (v->getMyType() == "driver" || v->getMyType() == "limitedDriver") {
            if (v->canTakeOrder(*order)) {
                v->acceptOrder(*order);
                order->setDriverId(v->getId());
                inProcessOrders.push_back(order);
                removeFromVector("pendingOrders", *order);
                order->setStatus(OrderStatus::DELIVERING);
                found = true;
            }
        }
    }
}

void WareHouse::addToVector(string nameOfVector, Order *order) {
    if (nameOfVector == "pendingOrders")
        pendingOrders.push_back(order);
    else if (nameOfVector == "inProcessOrders")
        inProcessOrders.push_back(order);
    else
        completedOrders.push_back(order);
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

    else if (nameOfVector == "inProcessOrders") {
        std::vector<Order *>::iterator it = inProcessOrders.begin();
        while (it != inProcessOrders.end()) {
            if ((*it)->getId() == order.getId())
                inProcessOrders.erase(it);
            else
                ++it;
        }
    }
}
void WareHouse::removeFromVector(Volunteer &volunteer) {
    std::vector<Volunteer *>::iterator it = volunteers.begin();
    while (it != volunteers.end()) {
        if ((*it)->getId() == volunteer.getId())
            volunteers.erase(it);
        else
            ++it;
    }
}

void WareHouse::deleteVolunteer(Volunteer *v) {
    removeFromVector(*v);
    delete v;
    volunteerCounter--;
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
    if (backup == nullptr)
        backup = new WareHouse(*this);
    else
        *backup = *this;
}

void WareHouse::restore() {
    resetVectors();
    *this = *backup;
}

void WareHouse::resetVectors() {
    actionsLog = {};
    volunteers = {};
    pendingOrders = {};
    inProcessOrders = {};
    completedOrders = {};
    customers = {};
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

int stringToInt(const std::string &str) {
    std::istringstream iss(str);
    int result = 0;

    if (!(iss >> result)) {
        return 0;
    }

    return result;
}

// Destructor
WareHouse::~WareHouse() {
    for (BaseAction *action : actionsLog)
        delete action;
    for (Volunteer *volunteer : volunteers)
        delete volunteer;
    for (Order *order : pendingOrders)
        delete order;
    for (Order *order : inProcessOrders)
        delete order;
    for (Order *order : completedOrders)
        delete order;
    for (Customer *customer : customers)
        delete customer;
}

// Copy Constructor
WareHouse::WareHouse(const WareHouse &other)
    : isOpen(other.isOpen), actionsLog(other.actionsLog),
      volunteers(other.volunteers), pendingOrders(other.pendingOrders),
      inProcessOrders(other.inProcessOrders),
      completedOrders(other.completedOrders), customers(other.customers),
      customerCounter(other.customerCounter),
      volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter) {}

// // Copy Assignment Operator
WareHouse &WareHouse::operator=(const WareHouse &other) {
    if (this != &other) {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        for (BaseAction *action : other.actionsLog) {
            actionsLog.push_back(action->clone());
        }

        for (Volunteer *volunteer : other.volunteers) {
            volunteers.push_back(volunteer->clone());
        }

        for (Order *order : other.pendingOrders) {
            pendingOrders.push_back(new Order(*order));
        }

        for (Order *order : other.inProcessOrders) {
            inProcessOrders.push_back(new Order(*order));
        }

        for (Order *order : other.completedOrders) {
            completedOrders.push_back(new Order(*order));
        }
        for (Customer *customer : other.customers) {
            customers.push_back(customer->clone());
        }
    }
    return *this;
}

// Move Constructor
WareHouse::WareHouse(WareHouse &&other) noexcept
    : isOpen(std::move(other.isOpen)), actionsLog(std::move(other.actionsLog)),
      volunteers(std::move(other.volunteers)),
      pendingOrders(std::move(other.pendingOrders)),
      inProcessOrders(std::move(other.inProcessOrders)),
      completedOrders(std::move(other.completedOrders)),
      customers(std::move(other.customers)),
      customerCounter(std::move(other.customerCounter)),
      volunteerCounter(std::move(other.volunteerCounter)),
      orderCounter(std::move(other.orderCounter)) {}

// Move Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &&other) noexcept {
    if (this != &other) {
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        actionsLog = std::move(other.actionsLog);
        volunteers = std::move(other.volunteers);
        customers = std::move(other.customers);
        pendingOrders = std::move(other.pendingOrders);
        inProcessOrders = std::move(other.inProcessOrders);
        completedOrders = std::move(other.completedOrders);
    }
    return *this;
};
