#include <iostream>
#include <cassert>
#include <cstring>
#include <string>

class Employee {
private:
    std::string name;
    unsigned int expInMo;
    void copy(const Employee& other) {
        expInMo = other.expInMo;
        name = other.name;
    }
public:
    Employee() {
        name = {};
        expInMo = 0;
    }
    Employee(std::string _name, unsigned int _exp) {
        name = _name;
        expInMo = _exp;
    }
    Employee(const Employee& other) {
        copy(other);
    }
    virtual ~Employee(){}
    Employee& operator =(const Employee& other) {
        if (this != &other) {
            copy(other);
        }
        return *this;
    }
    virtual void printInfo()const = 0;
    std::string getName()const {
        return this->name;
    }
    const unsigned int getExp()const {
        return expInMo;
    }
    virtual Employee* clone()const = 0;
    virtual void serialize(std::ostream& out)const = 0;
    virtual void deserialize(std::istream& in) = 0;
};

void Employee::printInfo()const {
    std::cout << "Employee Name: " << name << std::endl;
    std::cout << "Employee experience: " << expInMo << std::endl;
}

void Employee::serialize(std::ostream& out)const {
    out << "Name: " << name << "\n" << "Experience: " << expInMo << "\n";
}

void Employee::deserialize(std::istream& in) {
    std::getline(in, this->name);
    in >> expInMo;
}



class Programmer : public Employee {
private:
    bool knowsCpp;
    bool knowsCSharp;
    void copy(const Programmer& other) {
        knowsCpp = other.knowsCpp;
        knowsCSharp = other.knowsCSharp;
    }
public:
    Programmer(): Employee() {
        knowsCpp = 0;
        knowsCSharp = 0;
    }
    Programmer(bool kCpp, bool kCs, std::string name, unsigned int exp): Employee(name, exp) 
    {
        knowsCpp = kCpp;
        knowsCSharp = kCs;
    }
    Programmer(const Programmer& other) : Employee(other.getName(), other.getExp()) {
        copy(other);
    }
    Programmer& operator=(const Programmer& other) {
        if (this != &other) {
            knowsCpp = 0;
            knowsCSharp = 0;
            copy(other);
        }
        return *this;
    }
    void printInfo()const override {
        this->Employee::printInfo();
        std::cout << "Employee knows Cpp: " << std::boolalpha << knowsCpp << std::endl;
        std::cout << "Employee knows CSharp: " << std::boolalpha << knowsCSharp << std::endl;

    }
    Programmer* clone()const override {
        return new Programmer(*this);
    }
    void serialize(std::ostream& out)const override {
        out << "Programmer: \n";
        this->Employee::serialize(out);
        out << "Programmer knows Cpp: " << std::boolalpha << knowsCpp << "\n";
        out << "Programmer knows CSharp: " << std::boolalpha << knowsCSharp << "\n";
    }
    void deserialize(std::istream& in)override {
        this->Employee::deserialize(in);
        in >> knowsCpp >> knowsCSharp;
        in >> std::ws;
    }

};

class Manager : public Employee {
private:
    unsigned int numOfPeopleManaged;
    void copy(const Manager& other) {
        numOfPeopleManaged = other.numOfPeopleManaged;
    }
public:
    Manager() {
        numOfPeopleManaged = 0;
    }
    Manager(unsigned int npm, std::string name, unsigned int exp) :Employee(name, exp) {
        numOfPeopleManaged = npm;
    }
    Manager(const Manager& other) :Employee(other.getName(), other.getExp()) {
        copy(other);
    }
    Manager& operator =(const Manager& other) {
        if (this != &other) {
            numOfPeopleManaged = 0;
            copy(other);
        }
        return *this;
    }
    void printInfo()const override {
        this->Employee::printInfo();
        std::cout << "Number of people managed: " << numOfPeopleManaged << std::endl;
    }
    Manager* clone() const override {
        return new Manager(*this);
    }
    void serialize(std::ostream& out)const override {
        out << "Manager: \n";
        this->Employee::serialize(out);
        out << "Number of people managed: " << numOfPeopleManaged << std::endl;
    }
    void deserialize(std::istream& in)override {
        this->Employee::deserialize(in);
        in >> numOfPeopleManaged;
        in >> std::ws;
    }
};

Employee* factory(const std::string& type) {
    if (type == "Programmer") {
        return new Programmer();
    }
    else if (type == "Manager") {
        return new Manager();
    }
   
    return nullptr;
   
}

class PersonalQuarter {
private:
    Employee** employees;
    unsigned int size;
    unsigned int capacity;
    void resize() {
        assert(this->capacity > 0);
        this->capacity *= 2;
        Employee** more = new Employee *[capacity];
        for (int i = 0; i < size; i++)
        {
            more[i] = employees[i]->clone();
        }
        delete[]this->employees;
        employees = more;
    }
    void destroy() {
        for (size_t i = 0; i < size; i++)
        {
            delete this->employees[i];
        }
        delete[]this->employees;
    }
    void copy(const PersonalQuarter& other) {
        capacity = other.capacity;
        size = other.size;
        assert(this->size < this->capacity);
        employees = new Employee * [this->capacity];
        for (size_t i = 0; i < this->size; i++)
        {
            employees[i] = other.employees[i]->clone();
        }
       
    }
    void setCap(int n) {
        assert(n > 0);
        capacity = n;
    }
public:
    PersonalQuarter(std::istream& in) {
        in >> size;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        capacity = size*2;
        this->employees = new Employee * [this->capacity];
        for (size_t i = 0; i < size; i++)
        {
            std::string type;
            std::getline(in, type);

            Employee* newEmployee = factory(type);
            newEmployee->deserialize(in);
            this->employees[i] = newEmployee;
        }
    }
    PersonalQuarter(unsigned int _cap) {
        setCap(_cap);
        size = 0;
        employees = new Employee *[capacity];
    }
    PersonalQuarter(const PersonalQuarter& other) {
        copy(other);
    }
    ~PersonalQuarter() {
        this->destroy();
    }
    PersonalQuarter& operator =(const PersonalQuarter& other) {
        if (this != &other) {
            this->destroy();
            copy(other);
        }
        return *this;
    }
    PersonalQuarter& employNew(const Employee* const newEmployee)
    {
        if (this->size == this->capacity - 1)
        {
            this->resize();
        }
        this->employees[this->size] = newEmployee->clone();
        ++this->size;
        return *this;
    }
    PersonalQuarter& removeEmployee(const Employee* const _Employee) {
        Employee** res = new Employee * [this->capacity];
        int count = 0;
        for (size_t i = 0; i < size; i++)
        {
            if ((this->employees[i]->getName() != _Employee->getName()) && (this->employees[i]->getExp() != _Employee->getExp()))
            {
                res[count] = this->employees[i];
                ++count;
            }
        }
   
        delete this->employees;
        this->employees = res;
        --size;
        return *this;
    }
    PersonalQuarter& removeNonExperiencedEmployees() {
        for (size_t i = 0; i < size; i++)
        {
            if (this->employees[i]->getExp() < 3) {
                removeEmployee(this->employees[i]);
            }
        }
        return *this;
    }
    void printAllInfo() {
        for (int i = 0; i < size; i++)
        {
            std::cout << "Employee #: " << i + 1 << "\n";
            employees[i]->printInfo();
            std::cout << std::endl;
        }
    }
    void serialize(std::ostream& out) {
        out << "Size: " << size << "\n";
        out << "Capacity: " << capacity << "\n";
        for (size_t i = 0; i < size; i++)
        {
            out << "Person #" << i + 1 << "\n";
            this->employees[i]->serialize(out);
            out << "\n";
        }
    }
};


int main()
{
    std::string name1 ="Johnyy";
    std::string name2 ="Hirs";
    std::string name3 ="Kechi";
    std::string name4 ="pase";
    Manager M1(5, name1, 12);
    Programmer P1(true, true, name2, 28);
    Programmer P2 (false, true, name3, 25);
    Programmer P3 (true, false, name4, 30);
    PersonalQuarter PQ1(10);
    PQ1.employNew(&M1);
    PQ1.employNew(&P1);
    PQ1.employNew(&P2);
    PQ1.employNew(&P3);
    PQ1.printAllInfo();
    PQ1.removeEmployee(&P3);
    std::cout << "-------------------------------------------------------------------" << std::endl;
    PQ1.printAllInfo();
    std::cout << "-------------------------------------------------------------------" << std::endl;
}
