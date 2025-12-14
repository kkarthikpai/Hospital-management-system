#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

// UNIT II: Base Class - Inheritance
class Person {
protected:
    string name;
    int age;
    string contact;
public:
    Person() : name(""), age(0), contact("") {}
    Person(string n, int a, string c) : name(n), age(a), contact(c) {}
    virtual void display() const {}
    string getName() const { return name; }
    virtual ~Person() {}
};

// UNIT III: Patient Class - Inheritance, Constructors, Operator Overloading
class Patient : public Person {
private:
    int patientID;
    string disease;
    int bedNumber;
    double additionalCharges;
    int daysAdmitted;
    bool isAdmitted;

public:
    Patient() : Person(), patientID(0), disease(""), bedNumber(0), 
                additionalCharges(0.0), daysAdmitted(0), isAdmitted(false) {}
    
    Patient(int id, string n, int a, string c, string d, int bed) 
        : Person(n, a, c), patientID(id), disease(d), bedNumber(bed), 
          additionalCharges(0.0), daysAdmitted(1), isAdmitted(true) {}
    
    // UNIT III: Copy Constructor
    Patient(const Patient& p) : Person(p.name, p.age, p.contact) {
        patientID = p.patientID;
        disease = p.disease;
        bedNumber = p.bedNumber;
        additionalCharges = p.additionalCharges;
        daysAdmitted = p.daysAdmitted;
        isAdmitted = p.isAdmitted;
    }
    
    // Getters
    int getID() const { return patientID; }
    int getBedNumber() const { return bedNumber; }
    bool getAdmissionStatus() const { return isAdmitted; }
    int getDaysAdmitted() const { return daysAdmitted; }
    
    // UNIT I: Function - Calculate bill (3000 per day + additional)
    double getTotalBill() const {
        return (daysAdmitted * 3000) + additionalCharges;
    }
    
    // Setters
    void setDisease(string d) { disease = d; }
    void addCharges(double amount) { additionalCharges += amount; }
    void incrementDay() { daysAdmitted++; }
    void discharge() { isAdmitted = false; }
    void clearBill() { additionalCharges = -(daysAdmitted * 3000); }
    
    void display() const {
        cout << left << setw(6) << patientID << setw(18) << name << setw(5) << age 
             << setw(13) << contact << setw(18) << disease << setw(6) << bedNumber
             << setw(6) << daysAdmitted << setw(10) << fixed << setprecision(0) << getTotalBill() << endl;
    }
    
    void showBill() const {
        cout << "\n--- Bill Breakdown ---\n";
        cout << "Room: " << daysAdmitted << " days x Rs.3000 = Rs." << (daysAdmitted * 3000) << endl;
        cout << "Additional: Rs." << additionalCharges << endl;
        cout << "TOTAL: Rs." << getTotalBill() << endl;
    }
    
    // UNIT IV: File I/O
    void saveToFile(ofstream& out) const {
        out << patientID << "|" << name << "|" << age << "|" << contact << "|" 
            << disease << "|" << bedNumber << "|" << additionalCharges << "|" 
            << daysAdmitted << "|" << isAdmitted << endl;
    }
    
    void loadFromFile(ifstream& in) {
        string line;
        getline(in, line, '|'); patientID = atoi(line.c_str());
        getline(in, name, '|');
        getline(in, line, '|'); age = atoi(line.c_str());
        getline(in, contact, '|');
        getline(in, disease, '|');
        getline(in, line, '|'); bedNumber = atoi(line.c_str());
        getline(in, line, '|'); additionalCharges = atof(line.c_str());
        getline(in, line, '|'); daysAdmitted = atoi(line.c_str());
        getline(in, line); isAdmitted = (line == "1");
    }
    
    // UNIT III: Operator Overloading
    friend ostream& operator<<(ostream& os, const Patient& p) {
        os << "ID:" << p.patientID << " Name:" << p.name << " Bed:" << p.bedNumber;
        return os;
    }
};

// UNIT III: Hospital Class - Arrays, Dynamic Allocation
class Hospital {
private:
    vector<Patient> patients; // UNIT V: STL Container
    bool beds[100]; // UNIT I: Array
    int totalBeds;
    int nextPatientID;
    
public:
    Hospital(int beds) : totalBeds(beds), nextPatientID(1) {
        for(int i = 0; i < totalBeds; i++) this->beds[i] = false;
        loadFromFile();
    }
    
    // UNIT IV: Exception Classes
    class BedNotAvailableException {
    public:
        string message;
        BedNotAvailableException(string msg) : message(msg) {}
    };
    
    class PatientNotFoundException {
    public:
        string message;
        PatientNotFoundException(string msg) : message(msg) {}
    };
    
    // UNIT I: Functions
    void admitPatient() {
        try {
            string name, contact, disease;
            int age, bedNumber;
            
            cout << "\n--- Admit Patient ---\n";
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Age: "; cin >> age;
            if(age <= 0 || age > 120) throw invalid_argument("Invalid age!");
            cout << "Contact: "; cin >> contact;
            cout << "Disease: "; cin.ignore(); getline(cin, disease);
            
            bedNumber = findAvailableBed();
            if(bedNumber == -1) throw BedNotAvailableException("No beds available!");
            
            Patient newPatient(nextPatientID++, name, age, contact, disease, bedNumber);
            patients.push_back(newPatient);
            beds[bedNumber - 1] = true;
            
            cout << "\nAdmitted! ID:" << nextPatientID - 1 << " Bed:" << bedNumber << " Charge:Rs.3000/day\n";
            saveToFile();
            
        } catch(const BedNotAvailableException& e) {
            cout << "\nError: " << e.message << endl;
        } catch(const invalid_argument& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }
    
    int findAvailableBed() {
        for(int i = 0; i < totalBeds; i++) {
            if(!beds[i]) return i + 1;
        }
        return -1;
    }
    
    void viewPatients() const {
        if(patients.empty()) {
            cout << "\nNo patients!\n";
            return;
        }
        
        cout << "\n--- All Patients ---\n";
        cout << left << setw(6) << "ID" << setw(18) << "Name" << setw(5) << "Age" 
             << setw(13) << "Contact" << setw(18) << "Disease" << setw(6) << "Bed"
             << setw(6) << "Days" << setw(10) << "Bill" << endl;
        cout << string(90, '-') << endl;
        
        for(size_t i = 0; i < patients.size(); i++) {
            if(patients[i].getAdmissionStatus()) {
                patients[i].display();
            }
        }
    }
    
    // UNIT IV: Template Function
    template<typename T>
    Patient* searchPatient(T id) {
        for(size_t i = 0; i < patients.size(); i++) {
            if(patients[i].getID() == id && patients[i].getAdmissionStatus()) {
                return &patients[i];
            }
        }
        return NULL;
    }
    
    void searchPatientMenu() {
        int id;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            cout << "\n--- Patient Found ---\n";
            p->display();
            p->showBill();
        } else {
            cout << "\nPatient not found!\n";
        }
    }
    
    void updatePatient() {
        int id;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            string disease;
            cout << "New disease: "; cin.ignore(); getline(cin, disease);
            p->setDisease(disease);
            cout << "\nUpdated!\n";
            saveToFile();
        }
    }
    
    void addToBill() {
        int id;
        double amount;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            p->showBill();
            cout << "\nAdd amount: Rs."; cin >> amount;
            if(amount < 0) {
                cout << "\nInvalid!\n";
                return;
            }
            p->addCharges(amount);
            cout << "\nAdded! New total: Rs." << p->getTotalBill() << endl;
            saveToFile();
        }
    }
    
    void incrementDays() {
        int id;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            p->incrementDay();
            cout << "\nDay incremented! Days:" << p->getDaysAdmitted() 
                 << " Bill:Rs." << p->getTotalBill() << endl;
            saveToFile();
        }
    }
    
    void payBill() {
        int id;
        double payment;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            double bill = p->getTotalBill();
            p->showBill();
            
            if(bill == 0) {
                cout << "\nBill already cleared!\n";
                return;
            }
            
            cout << "\nPayment: Rs."; cin >> payment;
            if(payment < bill) {
                cout << "\nPartial payment not allowed! Need Rs." << bill << endl;
                return;
            }
            
            if(payment > bill) cout << "Change: Rs." << (payment - bill) << endl;
            
            p->clearBill();
            cout << "\n*** PAID! Patient can be discharged ***\n";
            saveToFile();
        }
    }
    
    void dischargePatient() {
        int id;
        cout << "\nPatient ID: "; cin >> id;
        Patient* p = searchPatient(id);
        if(p) {
            if(p->getTotalBill() > 0) {
                cout << "\nCannot discharge! Clear bill first!\n";
                p->showBill();
                return;
            }
            
            cout << "\n--- Discharge ---\n";
            cout << "Name: " << p->getName() << endl;
            cout << "Days: " << p->getDaysAdmitted() << endl;
            cout << "Bill: Rs.0 (PAID)\n";
            
            beds[p->getBedNumber() - 1] = false;
            p->discharge();
            cout << "\nDischarged!\n";
            saveToFile();
        }
    }
    
    // UNIT IV: File I/O
    void saveToFile() const {
        ofstream out("hospital_data.txt");
        if(!out) return;
        out << nextPatientID << endl << patients.size() << endl;
        for(size_t i = 0; i < patients.size(); i++) {
            patients[i].saveToFile(out);
        }
        out.close();
    }
    
    void loadFromFile() {
        ifstream in("hospital_data.txt");
        if(!in) return;
        string line;
        getline(in, line); nextPatientID = atoi(line.c_str());
        getline(in, line); int count = atoi(line.c_str());
        for(int i = 0; i < count; i++) {
            Patient p;
            p.loadFromFile(in);
            patients.push_back(p);
            if(p.getAdmissionStatus()) beds[p.getBedNumber() - 1] = true;
        }
        in.close();
    }
    
    ~Hospital() { saveToFile(); }
};

// MAIN PROGRAM
int main() {
    Hospital hospital(50);
    int choice;
    
    do {
        cout << "\n=== Hospital Management System ===\n";
        cout << "1. Admit Patient\n2. View Patients\n3. Search Patient\n";
        cout << "4. Update Patient\n5. Add to Bill\n6. Increment Days\n";
        cout << "7. Pay Bill\n8. Discharge Patient\n9. Exit\n";
        cout << "Choice: "; cin >> choice;
        
        switch(choice) {
            case 1: hospital.admitPatient(); break;
            case 2: hospital.viewPatients(); break;
            case 3: hospital.searchPatientMenu(); break;
            case 4: hospital.updatePatient(); break;
            case 5: hospital.addToBill(); break;
            case 6: hospital.incrementDays(); break;
            case 7: hospital.payBill(); break;
            case 8: hospital.dischargePatient(); break;
            case 9: cout << "\nThank you!\n"; break;
            default: cout << "\nInvalid choice!\n";
        }
    } while(choice != 9);
    
    return 0;
}