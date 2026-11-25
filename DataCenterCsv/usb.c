#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

struct USBSetup {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

USBSetup parseSetup(const uint8_t raw[8]) {
    USBSetup s;
    s.bmRequestType = raw[0];
    s.bRequest      = raw[1];
    s.wValue        = raw[2] | (raw[3] << 8);
    s.wIndex        = raw[4] | (raw[5] << 8);
    s.wLength       = raw[6] | (raw[7] << 8);
    return s;
}

std::string decodeBmRequestType(uint8_t bm) {
    std::ostringstream os;

    // Direction
    os << ((bm & 0x80) ? "Device→Host" : "Host→Device");

    // Type
    switch ((bm >> 5) & 0x03) {
        case 0: os << ", Standard"; break;
        case 1: os << ", Class";    break;
        case 2: os << ", Vendor";   break;
        default: os << ", Reserved"; break;
    }

    // Recipient
    switch (bm & 0x1F) {
        case 0: os << ", Device";    break;
        case 1: os << ", Interface"; break;
        case 2: os << ", Endpoint";  break;
        case 3: os << ", Other";     break;
        default: os << ", Reserved"; break;
    }

    return os.str();
}

std::string decodeStandardRequest(uint8_t bRequest) {
    switch (bRequest) {
        case 0x00: return "GET_STATUS";
        case 0x01: return "CLEAR_FEATURE";
        case 0x03: return "SET_FEATURE";
        case 0x05: return "SET_ADDRESS";
        case 0x06: return "GET_DESCRIPTOR";
        case 0x07: return "SET_DESCRIPTOR";
        case 0x08: return "GET_CONFIGURATION";
        case 0x09: return "SET_CONFIGURATION";
        case 0x0A: return "GET_INTERFACE";
        case 0x0B: return "SET_INTERFACE";
        default:   return "UNKNOWN";
    }
}

std::string decodeDescriptorType(uint8_t type) {
    switch (type) {
        case 0x01: return "Device";
        case 0x02: return "Configuration";
        case 0x03: return "String";
        case 0x04: return "Interface";
        case 0x05: return "Endpoint";
        case 0x21: return "HID";
        case 0x22: return "HID Report";
        default:   return "Unknown";
    }
}

std::string describeSetup(const USBSetup& s) {
    std::ostringstream os;

    os << "SETUP:\n";
    os << "  bmRequestType: " << decodeBmRequestType(s.bmRequestType) << "\n";

    if ((s.bmRequestType & 0x60) == 0x00) {
        os << "  bRequest     : " << decodeStandardRequest(s.bRequest) << "\n";
    } else {
        os << "  bRequest     : 0x" << std::hex << int(s.bRequest) << "\n";
    }

    os << "  wValue       : 0x" << std::hex << s.wValue << "\n";
    os << "  wIndex       : 0x" << std::hex << s.wIndex << "\n";
    os << "  wLength      : "  << std::dec << s.wLength << "\n";

    if (s.bRequest == 0x06) { // GET_DESCRIPTOR
        uint8_t descType = s.wValue >> 8;
        uint8_t descIdx  = s.wValue & 0xFF;
        os << "    Descriptor : " << decodeDescriptorType(descType)
           << " (index " << int(descIdx) << ")\n";
    }

    return os.str();
}


