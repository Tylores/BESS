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

    tsu::string_map ReadModel (const unsigned int did);

    void WriteModel (const unsigned int did, tsu::string_map model);

private:
    unsigned int sunspec_key_;
    modbus_t* context_ptr_;
    std::vector <SunSpecModel> models_;
};

#endif // MODBUS_H_
