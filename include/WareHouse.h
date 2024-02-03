#pragma once
#include <string>
#include <vector>
#include "Order.h"
#include "Customer.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.
int stringToInt(const std::string& str);


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        void start();
        void addOrder(Order* order);
        void AddCustomer(Customer* customer);
        void addAction(BaseAction* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;

        bool isVolunteerExist(int volunteerId) const;
        bool isCustomerExist(int customerId) const;
        bool isOrderExist (int orderId) const; 
        int getOrderCounter() const;
        int getVolunteerCounter() const;
        int getCustomerCounter() const;
        vector<Order*> getVectorOrders(string nameOfVector) const;
        vector<Volunteer*> getVectorVolunteers();
        vector<Customer*> getVectorCustomers() const;
        const vector<BaseAction*> &getActions() const;

        void findCollector(Order *order);
        void findDriver(Order *order);
        void addToVector (string nameOfVector, Order* order);
        void removeFromVector (string nameOfVector, Order &order);
        void removeFromVector (Volunteer &volunteer);
        void deleteVolunteer (Volunteer *v);

        void backUp();
        void restore();

        void close();
        void open();
        
        void parseText(const string &configFilePath);
        void printCustomers();

// -------------------------Rule Of Five:------------------------------------- 

        // Destructor
        ~WareHouse();

        // Copy Constructor
        WareHouse(const WareHouse& other);

        // Copy Assignment Operator
        WareHouse& operator=(const WareHouse& other);

        // Move Constructor
        WareHouse(WareHouse&& other) noexcept;

        // Move Assignment Operator
        WareHouse& operator=(WareHouse&& other) noexcept;
        
//----------------------------------------------------------------------------
  
    private:
        bool isOpen;
        vector<BaseAction*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter;
};