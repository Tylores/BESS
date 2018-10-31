#include <iostream>
#include <string>

#include "include/Modbus.h"
#include "include/Logger.h"

Modbus::Modbus (tsu::string_map& init) {
    // convert init map data
    const char* ip = init["ip"].c_str();
    int port = std::stoi(init["port"]);

    // create modbus context pointer and connect to device at
    // given ip address and port number.
    context_ptr_ = modbus_new_tcp(ip, port);
    if (modbus_connect(context_ptr_) == -1) {
        std::cout << "[ERROR] : " << modbus_strerror(errno) << '\n';
        Logger("ERROR") << ip << "/" << port << "\t" << modbus_strerror(errno);
    }

    // read modbus registers for sunspec blocks
    Modbus::Querry ();
}

Modbus::~Modbus () {
    models_.clear ();
    modbus_close(context_ptr_);
    modbus_free(context_ptr_);
}

// Querry
// - Read all available registers to find sunspec compliant blocks.
// - The first step is to read the first register and see if the modbus
// - device is a sunspec complient device.
// - Then traverse register array and find sunspec DID values and create a
// - sunspec model.
void Modbus::Querry () {
    unsigned int sunspec_key_ = 1850954613;
    uint16_t sunspec_id[2];

    // TODO (TS): sunspec states the holding register start can be
    // - 30, 40, or 50,000.
    // - 40000 is the preferece starting location
    unsigned int id_offset = 0;
    ReadRegisters(id_offset, 2, sunspec_id);
    std::cout << MODBUS_GET_INT32_FROM_INT16(sunspec_id,0) << std::endl;
    std::shared_ptr <SunSpecModel> model (new SunSpecModel (1, 2));
    models_.push_back (std::move (model));
}

// Read Registers
// - the register array is passed to the function as a pointer so the
// - modbus method call can operate on them.
void Modbus::ReadRegisters (unsigned int offset,
                    unsigned int length,
                    uint16_t *reg_ptr) {
    int status = modbus_read_registers (context_ptr_,
                                        offset,
                                        length,
                                        reg_ptr);
    if (status == -1) {
        std::cout << "[ERROR] : " << modbus_strerror(errno) << '\n';
        Logger("ERROR") << modbus_strerror(errno);
    }
}

// Write Registers
// - the registers to write are passed by reference to reduce memory
void Modbus::WriteRegisters (unsigned int offset,
                     unsigned int length,
                     const uint16_t *reg_ptr) {
    int status = modbus_write_registers (context_ptr_,
                                         offset,
                                         length,
                                         reg_ptr);
    if (status == -1) {
        std::cout << "[ERROR] : " << modbus_strerror(errno) << '\n';
        Logger("ERROR") << modbus_strerror(errno);
    }
}

void Modbus::ReadBlock (std::string name) {
}

void Modbus::WriteBlock (std::string name) {
}

