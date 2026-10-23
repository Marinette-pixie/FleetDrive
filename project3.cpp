#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
#include <stdexcept>

// ============================================================================
// UNIT 6: CUSTOM EXCEPTION CLASSES
// ============================================================================
class RentalException : public std::runtime_error {
public:
    explicit RentalException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidDurationException : public RentalException {
public:
    explicit InvalidDurationException(const std::string& msg) : RentalException(msg) {}
};

class VehicleUnavailableException : public RentalException {
public:
    explicit VehicleUnavailableException(const std::string& msg) : RentalException(msg) {}
};

// ============================================================================
// UNIT 6: TEMPLATES (CLASS & FUNCTION TEMPLATES)
// ============================================================================
// Class template with multiple parameters: Type T and non-type parameter Capacity
template <typename T, int Capacity>
class FleetCatalog {
private:
    T items[Capacity];
    int count;
public:
    FleetCatalog() : count(0) {}

    void add(const T& item) {
        if (count >= Capacity) {
            throw std::overflow_error("Catalog capacity exceeded!");
        }
        items[count++] = item;
    }

    T get(int index) const {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Catalog index out of bounds!");
        }
        return items[index];
    }

    int size() const { return count; }
};

// Function template and overloaded function template
template <typename T>
T findCheapest(T a, T b) {
    return (a < b) ? a : b;
}

template <typename T>
T findCheapest(T a, T b, T c) {
    return findCheapest(findCheapest(a, b), c);
}

// ============================================================================
// UNIT 3: OPERATOR OVERLOADING & TYPE CONVERSION
// ============================================================================
class AddOnPackage {
private:
    std::string name;
    double dailyFee;

public:
    // Basic-to-Class Type Conversion via Constructor
    AddOnPackage(double flatFee = 0.0) : name("Standard Package"), dailyFee(flatFee) {}

    AddOnPackage(std::string pkgName, double fee) : name(pkgName), dailyFee(fee) {}

    // Binary + Operator Overloading to combine insurance/gear add-ons
    AddOnPackage operator+(const AddOnPackage& other) const {
        return AddOnPackage(this->name + " & " + other.name, this->dailyFee + other.dailyFee);
    }

    // Class-to-Basic Type Conversion (double)
    operator double() const {
        return dailyFee;
    }

    std::string getName() const { return name; }

    // Stream Insertion Operator Overloading
    friend std::ostream& operator<<(std::ostream& os, const AddOnPackage& pkg) {
        os << pkg.name << " (INR " << std::fixed << std::setprecision(2) << pkg.dailyFee << "/day)";
        return os;
    }
};

// ============================================================================
// UNIT 2, 3, 4: OOP CORE, DIAMOND INHERITANCE, POLYMORPHISM
// ============================================================================
class FleetInspector; // Forward declaration for Friend Class

// Abstract Base Class & Virtual Base Class
class Vehicle {
protected:
    int vehicleID;
    std::string model;
    double baseRate;
    bool isAvailable;
    static int totalFleetCount; // Static data member

public:
    // Parameterized constructor with default arguments
    Vehicle(int id = 0, std::string mdl = "Standard", double rate = 50.0, bool avail = true)
        : vehicleID(id), model(mdl), baseRate(rate), isAvailable(avail) {
        totalFleetCount++;
    }

    // Copy Constructor
    Vehicle(const Vehicle& other)
        : vehicleID(other.vehicleID), model(other.model),
          baseRate(other.baseRate), isAvailable(other.isAvailable) {
        totalFleetCount++;
    }

    // Virtual Destructor
    virtual ~Vehicle() {
        totalFleetCount--;
    }

    // Inline Member Function
    inline bool checkAvailability() const {
        return isAvailable;
    }

    // 'this' pointer returning reference for method chaining
    Vehicle& updateRate(double newRate) {
        this->baseRate = newRate;
        return *this;
    }

    // Pass and Return by Reference
    Vehicle& setStatus(const bool& status) {
        this->isAvailable = status;
        return *this;
    }

    int getId() const { return vehicleID; }
    std::string getModel() const { return model; }
    double getBaseRate() const { return baseRate; }

    // Static member function
    static int getTotalFleet() {
        return totalFleetCount;
    }

    // Pure Virtual Functions (Runtime Polymorphism)
    virtual double calculateRent(int days, double mileage = 0.0) const = 0;
    virtual void displayDetails() const = 0;

    // Friend Class Declaration
    friend class FleetInspector;
};

// Initialize Static Member
int Vehicle::totalFleetCount = 0;

// Virtual Base Class Inheritance (Diamond Structure: Branch 1)
class LandVehicle : virtual public Vehicle {
protected:
    double highwayTax;

public:
    LandVehicle(int id, std::string mdl, double rate, double tax, bool avail = true)
        : Vehicle(id, mdl, rate, avail), highwayTax(tax) {}

    double calculateRent(int days, double mileage = 0.0) const override {
        return (baseRate * days) + (highwayTax * days) + (mileage * 0.15);
    }

    void displayDetails() const override {
        std::cout << "[Land Craft]          ID: " << std::left << std::setw(5) << vehicleID
                  << " | " << std::setw(25) << model
                  << " | Base: INR " << std::setw(6) << std::fixed << std::setprecision(2) << baseRate << "/day"
                  << " | Highway Tax: INR " << std::setw(5) << highwayTax
                  << " | Status: " << (isAvailable ? "Available" : "RENTED") << "\n";
    }
};

// Virtual Base Class Inheritance (Diamond Structure: Branch 2)
class WaterVehicle : virtual public Vehicle {
protected:
    double marineDockingFee;

public:
    WaterVehicle(int id, std::string mdl, double rate, double dockFee, bool avail = true)
        : Vehicle(id, mdl, rate, avail), marineDockingFee(dockFee) {}

    double calculateRent(int days, double mileage = 0.0) const override {
        (void)mileage; // Unused for marine craft
        return (baseRate * days) + marineDockingFee;
    }

    void displayDetails() const override {
        std::cout << "[Water Vessel]        ID: " << std::left << std::setw(5) << vehicleID
                  << " | " << std::setw(25) << model
                  << " | Base: INR " << std::setw(6) << std::fixed << std::setprecision(2) << baseRate << "/day"
                  << " | Dock Fee:    INR " << std::setw(5) << marineDockingFee
                  << " | Status: " << (isAvailable ? "Available" : "RENTED") << "\n";
    }
};

// Diamond Inheritance Resolved
class AmphibianCraft : public LandVehicle, public WaterVehicle {
public:
    AmphibianCraft(int id, std::string mdl, double rate, double tax, double dockFee, bool avail = true)
        : Vehicle(id, mdl, rate, avail),
          LandVehicle(id, mdl, rate, tax, avail),
          WaterVehicle(id, mdl, rate, dockFee, avail) {}

    double calculateRent(int days, double mileage = 0.0) const override {
        return (baseRate * days) + (highwayTax * days) + marineDockingFee + (mileage * 0.25);
    }

    void displayDetails() const override {
        std::cout << "[Amphibian Dual-Craft] ID: " << std::left << std::setw(5) << vehicleID
                  << " | " << std::setw(25) << model
                  << " | Base: INR " << std::setw(6) << std::fixed << std::setprecision(2) << baseRate << "/day"
                  << " | Dual Ops Fee: INR " << std::setw(5) << (highwayTax + marineDockingFee)
                  << " | Status: " << (isAvailable ? "Available" : "RENTED") << "\n";
    }
};

// Friend Class Implementation
class FleetInspector {
public:
    void auditVehicle(const Vehicle& v) const {
        std::cout << " [INSPECTOR AUDIT] Vehicle #" << std::left << std::setw(4) << v.vehicleID
                  << " | Model: " << std::setw(25) << v.model
                  << " | Internal Base Rate: INR " << std::fixed << std::setprecision(2) << std::setw(7) << v.baseRate
                  << " | Telemetry Status: " << (v.isAvailable ? "[PASS - READY]" : "[BUSY - LEASED]") << "\n";
    }
};

// ============================================================================
// UNIT 5: FILE MANAGEMENT & BINARY SERIALIZATION
// ============================================================================
#pragma pack(push, 1)
struct VehicleFileRecord {
    int id;
    char model[32];
    double rate;
    bool available;
};
#pragma pack(pop)

class FileStorageManager {
public:
    static void initializeDatabase(const std::string& filename, Vehicle* fleet[], int size) {
        std::ifstream checkFile(filename, std::ios::binary);
        if (checkFile.good()) {
            checkFile.close();
            return; // File already initialized; keep persistence intact across runs
        }

        std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
        if (!outFile) {
            throw std::ios_base::failure("Cannot initialize binary ledger file!");
        }

        for (int i = 0; i < size; ++i) {
            VehicleFileRecord rec;
            rec.id = fleet[i]->getId();
            std::strncpy(rec.model, fleet[i]->getModel().c_str(), sizeof(rec.model) - 1);
            rec.model[sizeof(rec.model) - 1] = '\0';
            rec.rate = fleet[i]->getBaseRate();
            rec.available = fleet[i]->checkAvailability();
            outFile.write(reinterpret_cast<const char*>(&rec), sizeof(VehicleFileRecord));
        }
        outFile.close();
    }

    static void syncFromDatabase(const std::string& filename, Vehicle* fleet[], int size) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) return;

        VehicleFileRecord rec;
        int i = 0;
        while (i < size && inFile.read(reinterpret_cast<char*>(&rec), sizeof(VehicleFileRecord))) {
            fleet[i]->setStatus(rec.available);
            fleet[i]->updateRate(rec.rate);
            i++;
        }
        inFile.close();
    }

    static void updateRecordInPlace(const std::string& filename, int recordIndex, const VehicleFileRecord& updatedRec) {
        std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            throw std::ios_base::failure("Cannot open file for random-access update!");
        }

        // File pointer manipulation: seekp for binary writing
        std::streampos targetPos = recordIndex * sizeof(VehicleFileRecord);
        file.seekp(targetPos, std::ios::beg);
        file.write(reinterpret_cast<const char*>(&updatedRec), sizeof(VehicleFileRecord));
        file.close();
    }

    static void printLedgerContents(const std::string& filename) {
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            throw std::ios_base::failure("Cannot read binary ledger file!");
        }

        VehicleFileRecord rec;
        std::cout << "\n" << std::string(68, '=') << "\n";
        std::cout << "          LIVE BINARY DISK AUDIT (Direct File Inspection)\n";
        std::cout << std::string(68, '=') << "\n";
        std::cout << std::left << std::setw(8)  << "Slot"
                  << std::setw(8)  << "ID"
                  << std::setw(28) << "Model"
                  << std::setw(16) << "Daily Rate"
                  << std::setw(10) << "Status" << "\n";
        std::cout << std::string(68, '-') << "\n";

        int index = 0;
        while (inFile.read(reinterpret_cast<char*>(&rec), sizeof(VehicleFileRecord))) {
            std::cout << std::left << std::setw(8)  << index++
                      << std::setw(8)  << rec.id
                      << std::setw(28) << rec.model
                      << "INR " << std::fixed << std::setprecision(2) << std::setw(12) << rec.rate
                      << std::setw(10) << (rec.available ? "Available" : "RENTED") << "\n";
        }
        inFile.close();
        std::cout << std::string(68, '=') << "\n\n";
    }
};

// ============================================================================
// HELPER FOR SAFE CONSOLE INPUT
// ============================================================================
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ============================================================================
// INTERACTIVE CONSOLE MENU & CONTROLLER
// ============================================================================
int main(int argc, char* argv[]) {
    // Unit 5: Command-line Argument handling
    std::string ledgerFile = "fleet_ledger.bin";
    if (argc > 1) {
        ledgerFile = argv[1];
    }

    // Unit 4: Polymorphic handling via Base Class Pointers
    const int FLEET_SIZE = 3;
    Vehicle* fleet[FLEET_SIZE];

    fleet[0] = new LandVehicle(101, "Mercedes Sprinter Van", 95.0, 15.0);
    fleet[1] = new WaterVehicle(202, "Yamaha FX WaveRunner", 140.0, 45.0);
    fleet[2] = new AmphibianCraft(303, "Gibbs Quadski Amphibian", 230.0, 20.0, 40.0);

    // Initial DB sync or generation
    try {
        FileStorageManager::initializeDatabase(ledgerFile, fleet, FLEET_SIZE);
        FileStorageManager::syncFromDatabase(ledgerFile, fleet, FLEET_SIZE);
    } catch (const std::exception& e) {
        std::cerr << "Initialization Error: " << e.what() << "\n";
    }

    // Unit 6: Class template instantiation
    FleetCatalog<std::string, 5> vehicleRegistry;
    vehicleRegistry.add("VAN-101 (Cargo/Transport)");
    vehicleRegistry.add("SEA-202 (Coastal Patrol)");
    vehicleRegistry.add("AMP-303 (Multi-Terrain Rescue)");

    // Unit 3: Add-on instances
    AddOnPackage gps("Garmin Navigation GPS", 12.50);
    AddOnPackage insurance("All-Terrain Loss Damage Waiver", 28.00);
    FleetInspector auditor;

    int choice = 0;
    while (choice != 7) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "       FLEETDRIVE: VEHICLE MANAGEMENT & LEASING PORTAL\n";
        std::cout << std::string(60, '=') << "\n";
        std::cout << " 1. Display Fleet Catalog (Polymorphic View)\n";
        std::cout << " 2. Rent / Lease a Vehicle (Interactive Booking)\n";
        std::cout << " 3. Return a Vehicle (Update In-Place)\n";
        std::cout << " 4. Run Diagnostic Fleet Audit (Friend Class Access)\n";
        std::cout << " 5. Rate Comparison & Registry Lookup (Templates Demo)\n";
        std::cout << " 6. Inspect Raw Binary Ledger File (File I/O Verification)\n";
        std::cout << " 7. Exit System\n";
        std::cout << std::string(60, '-') << "\n";
        std::cout << "Enter your choice [1-7]: ";

        if (!(std::cin >> choice)) {
            std::cout << "\n[!] Invalid input. Please enter a number between 1 and 7.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1: {
                // Polymorphic display
                std::cout << "\n" << std::string(80, '-') << "\n";
                std::cout << "                      ACTIVE FLEET INVENTORY\n";
                std::cout << std::string(80, '-') << "\n";
                for (int i = 0; i < FLEET_SIZE; ++i) {
                    std::cout << "[" << i << "] ";
                    fleet[i]->displayDetails();
                }
                std::cout << std::string(80, '-') << "\n";
                std::cout << "Total Active Objects (Static Member): " << Vehicle::getTotalFleet() << "\n";
                break;
            }

            case 2: {
                // Interactive booking with Exception Handling
                std::cout << "\n" << std::string(60, '-') << "\n";
                std::cout << "                    VEHICLE LEASE BOOKING\n";
                std::cout << std::string(60, '-') << "\n";
                for (int i = 0; i < FLEET_SIZE; ++i) {
                    std::cout << "  [" << i << "] " << std::left << std::setw(28) << fleet[i]->getModel()
                              << " | Status: " << (fleet[i]->checkAvailability() ? "Available" : "RENTED") << "\n";
                }

                int selIndex = -1;
                std::cout << "\nSelect Vehicle Index to Rent [0-" << FLEET_SIZE - 1 << "]: ";
                std::cin >> selIndex;

                try {
                    if (std::cin.fail() || selIndex < 0 || selIndex >= FLEET_SIZE) {
                        clearInputBuffer();
                        throw std::out_of_range("Invalid vehicle selection index.");
                    }

                    if (!fleet[selIndex]->checkAvailability()) {
                        throw VehicleUnavailableException("This vehicle is already rented out!");
                    }

                    int days = 0;
                    std::cout << "Enter rental duration (Days): ";
                    std::cin >> days;
                    if (std::cin.fail() || days <= 0) {
                        clearInputBuffer();
                        throw InvalidDurationException("Rental duration must be at least 1 full day.");
                    }

                    double miles = 0.0;
                    std::cout << "Estimated travel distance (Miles, enter 0 for watercraft): ";
                    std::cin >> miles;
                    if (std::cin.fail() || miles < 0.0) {
                        clearInputBuffer();
                        miles = 0.0;
                    }

                    char optGps = 'n', optIns = 'n';
                    std::cout << "Include GPS Navigator (INR 12.50/day)? (y/n): ";
                    std::cin >> optGps;
                    std::cout << "Include All-Terrain Insurance Waiver (INR 28.00/day)? (y/n): ";
                    std::cin >> optIns;

                    // Operator Overloading & Type Conversion
                    AddOnPackage selectedAddons(0.0);
                    if (optGps == 'y' || optGps == 'Y') {
                        selectedAddons = selectedAddons + gps;
                    }
                    if (optIns == 'y' || optIns == 'Y') {
                        selectedAddons = selectedAddons + insurance;
                    }

                    // Class-to-basic type conversion
                    double addonDailyRate = selectedAddons;
                    double addonTotal = addonDailyRate * days;

                    // Polymorphic fare computation
                    double vehicleBaseCost = fleet[selIndex]->calculateRent(days, miles);
                    double grandTotal = vehicleBaseCost + addonTotal;

                    // Update memory state
                    fleet[selIndex]->setStatus(false);

                    // Binary File Random Access Update (Unit 5: seekp)
                    VehicleFileRecord rec;
                    rec.id = fleet[selIndex]->getId();
                    std::strncpy(rec.model, fleet[selIndex]->getModel().c_str(), sizeof(rec.model) - 1);
                    rec.model[sizeof(rec.model) - 1] = '\0';
                    rec.rate = fleet[selIndex]->getBaseRate();
                    rec.available = false;

                    FileStorageManager::updateRecordInPlace(ledgerFile, selIndex, rec);

                    // Formatted Invoice Output
                    std::cout << "\n==================================================\n";
                    std::cout << "             OFFICIAL LEASE INVOICE\n";
                    std::cout << "==================================================\n";
                    std::cout << std::left << std::setw(24) << "Vehicle Leased:" << fleet[selIndex]->getModel() << "\n";
                    std::cout << std::left << std::setw(24) << "Duration:" << days << " Day(s)\n";
                    std::cout << std::left << std::setw(24) << "Distance Allowed:" << miles << " Miles\n";
                    std::cout << std::left << std::setw(24) << "Vehicle Base Cost:" << "INR " << std::fixed << std::setprecision(2) << vehicleBaseCost << "\n";
                    std::cout << std::left << std::setw(24) << "Add-ons Selected:" << selectedAddons << "\n";
                    std::cout << std::left << std::setw(24) << "Add-ons Total:" << "INR " << addonTotal << "\n";
                    std::cout << std::string(50, '-') << "\n";
                    std::cout << std::left << std::setw(24) << "NET PAYABLE AMOUNT:" << "INR " << grandTotal << "\n";
                    std::cout << "==================================================\n";
                    std::cout << "[+] Record successfully synchronized with binary ledger!\n";

                } catch (const InvalidDurationException& e) {
                    std::cerr << "\n[LEASE ERROR] Duration Violation: " << e.what() << "\n";
                } catch (const VehicleUnavailableException& e) {
                    std::cerr << "\n[LEASE ERROR] Availability Conflict: " << e.what() << "\n";
                } catch (const std::out_of_range& e) {
                    std::cerr << "\n[SELECTION ERROR] " << e.what() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "\n[SYSTEM ERROR] An unexpected exception occurred: " << e.what() << "\n";
                }
                break;
            }

            case 3: {
                // Return vehicle and mark available again
                std::cout << "\nReturn a Leased Vehicle:\n";
                for (int i = 0; i < FLEET_SIZE; ++i) {
                    std::cout << "  [" << i << "] " << std::left << std::setw(28) << fleet[i]->getModel()
                              << " | Status: " << (fleet[i]->checkAvailability() ? "Available" : "RENTED") << "\n";
                }

                int selIndex = -1;
                std::cout << "Enter Vehicle Index to return [0-" << FLEET_SIZE - 1 << "]: ";
                std::cin >> selIndex;

                if (std::cin.fail() || selIndex < 0 || selIndex >= FLEET_SIZE) {
                    clearInputBuffer();
                    std::cout << "[!] Invalid index selected.\n";
                    break;
                }

                if (fleet[selIndex]->checkAvailability()) {
                    std::cout << "[!] Vehicle #" << fleet[selIndex]->getId() << " is already in the garage.\n";
                } else {
                    fleet[selIndex]->setStatus(true);

                    VehicleFileRecord rec;
                    rec.id = fleet[selIndex]->getId();
                    std::strncpy(rec.model, fleet[selIndex]->getModel().c_str(), sizeof(rec.model) - 1);
                    rec.model[sizeof(rec.model) - 1] = '\0';
                    rec.rate = fleet[selIndex]->getBaseRate();
                    rec.available = true;

                    FileStorageManager::updateRecordInPlace(ledgerFile, selIndex, rec);
                    std::cout << "[+] Vehicle successfully returned and updated in binary database!\n";
                }
                break;
            }

            case 4: {
                // Friend Class Demonstration
                std::cout << "\n--- Running Inspector Telemetry (Friend Class Direct Access) ---\n";
                for (int i = 0; i < FLEET_SIZE; ++i) {
                    auditor.auditVehicle(*fleet[i]);
                }
                break;
            }

            case 5: {
                // Templates Demonstration
                std::cout << "\n--- Generic Template Operations ---\n";
                double cheapest = findCheapest(fleet[0]->getBaseRate(), fleet[1]->getBaseRate(), fleet[2]->getBaseRate());
                std::cout << "Overloaded Function Template -> Lowest Entry Daily Rate: INR "
                          << std::fixed << std::setprecision(2) << cheapest << "\n\n";

                std::cout << "Class Template Registry Entries [FleetCatalog<T, Capacity>]:\n";
                for (int i = 0; i < vehicleRegistry.size(); ++i) {
                    std::cout << "  Index [" << i << "]: " << vehicleRegistry.get(i) << "\n";
                }
                break;
            }

            case 6: {
                // Unit 5: Direct Binary File Audit
                try {
                    FileStorageManager::printLedgerContents(ledgerFile);
                } catch (const std::exception& e) {
                    std::cerr << "File Read Error: " << e.what() << "\n";
                }
                break;
            }

            case 7: {
                std::cout << "\nTerminating FleetDrive session. Releasing dynamic allocations...\n";
                break;
            }

            default: {
                std::cout << "[!] Option not recognized. Please choose between 1 and 7.\n";
                break;
            }
        }
    }

    // Dynamic memory cleanup triggering virtual destructors
    for (int i = 0; i < FLEET_SIZE; ++i) {
        delete fleet[i];
    }

    std::cout << "All objects freed. Remaining active instances: "
              << Vehicle::getTotalFleet() << "\n";

    return 0;
}
