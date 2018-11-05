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
    sunspec_key_ = stoul(init["smdx_key"]);
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
    unsigned int id_offset = 0;

    // the true sunspec key is a 32 bit number so I hacked the config to
    // assigned the value to check and if it isn't the actual sunspec key it 
    // will be consided the smdx number.
    if (sunspec_key_ < 100000) {
        SunSpecModel model(sunspec_key_, id_offset);
        models_.push_back (model);
    } else {
        // the sunspec key 
        uint16_t sunspec_id[2];

        // TODO (TS): sunspec states the holding register start can be
        // - 30, 40, or 50,000.
        // - 40000 is the preferece starting location
        unsigned int id_offset = 0;
        ReadRegisters(id_offset, 2, sunspec_id);

        // DEGUG
        std::cout 
            << MODBUS_GET_INT32_FROM_INT16(sunspec_id,id_offset) 
            << std::endl;

        SunSpecModel model(1, 2);
        models_.push_back (model);
    }
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
        Logger("ERROR") << context_ptr_ << "\t" << modbus_strerror(errno);

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
        Logger("ERROR") << context_ptr_ << "\t" << modbus_strerror(errno);
    }
}

// Read Model
// 
tsu::string_map Modbus::ReadModel (const unsigned int did) {
    // search through models looking for the specified did number
    auto model_it = std::find(models_.begin(), models_.end(), did);

    // ensure index is within models
    if (model_it != models_.end()) {
        // get model properties
        unsigned int offset = (*model_it).GetOffset ();
        unsigned int length = (*model_it).GetLength ();

        // read model registers
        uint16_t registers[length];
        Modbus::ReadRegisters(offset, length, registers);

        // store array as vector for easier transport
        std::vector <uint16_t> block (length,0);
        for (unsigned int i = 0; i < length; i++) {
            block[i] = registers[i];
        }

        // convert modbus block to sunspec points 
        return (*model_it).BlockToPoints (block);
    } else {
        std::cout << "[ERROR]: Model not found" << std::endl;
        tsu::string_map blank;
        return blank;
    }
}

void Modbus::WriteModel (const unsigned int did, tsu::string_map model) {
    // search through models looking for the specified did number
    auto model_it = std::find(models_.begin(), models_.end(), did);

    // ensure index is within models
    if (model_it != models_.end()) {
        // get model properties
        unsigned int offset = (*model_it).GetOffset ();
        unsigned int length = (*model_it).GetLength ();

        // convert points to registers
        std::vector <uint16_t> block = (*model_it).PointsToBlock (model);

        // convert block to register array
        uint16_t registers[length];
        for (unsigned int i = 0; i < length; i++) {
            registers[i] = block[i];
            std::cout << block[i] << '\n';
        }

        Modbus::WriteRegisters (offset, length, registers);
    } else {
        std::cout << "[ERROR]: Model not found" << std::endl;
    }

}

