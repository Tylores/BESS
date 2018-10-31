#ifndef MODBUS_H_
#define MODBUS_H_

#include <string>
#include <vector>

// Modbus Includes
#include <modbus/modbus-tcp.h>

#include "SunSpecModel.h"
#include "tsu.h"

class Modbus {
public:
    Modbus (tsu::string_map& init);
    ~Modbus ();
    void Querry ();
    void ReadRegisters (unsigned int offset,
                        unsigned int length,
                        uint16_t *reg_ptr);
    void WriteRegisters (unsigned int offset,
                         unsigned int length,
                         const uint16_t *reg_ptr);

    void ReadBlock (std::string name);

    void WriteBlock (std::string name);

private:
    modbus_t* context_ptr_;
    std::vector <std::shared_ptr <SunSpecModel>> models_;
};

#endif // MODBUS_H_
