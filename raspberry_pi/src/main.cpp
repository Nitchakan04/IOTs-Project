#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

constexpr const char* I2C_DEV = "/dev/i2c-1";
constexpr uint8_t BH1750_ADDR = 0x23;        // 0x23 or 0x5C
constexpr uint8_t CMD_POWER_ON = 0x01;
constexpr uint8_t CMD_CONT_HRES = 0x10;      // Continuous High-Res mode

class I2C {
public:
    I2C(const char* dev, uint8_t addr) {
        fd_ = open(dev, O_RDWR);
        if (fd_ < 0) throw std::runtime_error("Failed to open I2C device");
        if (ioctl(fd_, I2C_SLAVE, addr) < 0) {
            close(fd_);
            throw std::runtime_error("Failed to set I2C address");
        }
    }
    ~I2C() { if (fd_ >= 0) close(fd_); }

    void writeByte(uint8_t b) {
        if (write(fd_, &b, 1) != 1) throw std::runtime_error("I2C write failed");
    }

    void readBytes(uint8_t* buf, size_t len) {
        ssize_t n = read(fd_, buf, len);
        if (n != (ssize_t)len) throw std::runtime_error("I2C read failed");
    }

private:
    int fd_ = -1;
};

int main() {
    try {
        I2C i2c(I2C_DEV, BH1750_ADDR);

        i2c.writeByte(CMD_POWER_ON);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i2c.writeByte(CMD_CONT_HRES);
        std::this_thread::sleep_for(std::chrono::milliseconds(180));

        while (true) {
            uint8_t data[2] = {0,0};
            i2c.readBytes(data, 2);
            uint16_t raw = (uint16_t(data[0]) << 8) | data[1];
            double lux = raw / 1.2;

            std::cout << "Light: " << std::fixed << std::setprecision(2)
                      << lux << " lx" << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << " (" << std::strerror(errno) << ")\n";
        return 1;
    }
    return 0;
}
