#include <iostream>
#include <cassert>
#include <utility>

/**
 * TcpSocket (Exercise 1)
 * A move-only wrapper around a simulated file descriptor.
 */
class TcpSocket {
public:
    TcpSocket(int fd) : m_fd(fd) {
        std::cout << " [Socket] Opened FD: " << m_fd << "\n";
    }

    ~TcpSocket() {
        if (m_fd != -1) {
            std::cout << " [Socket] Closing FD: " << m_fd << "\n";
            m_fd = -1;
        }
    }

    // Move-only
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;

    // Move constructor
    TcpSocket(TcpSocket&& other) noexcept : m_fd(other.m_fd) {
        other.m_fd = -1; // Transfer ownership
    }

    // Move assignment
    TcpSocket& operator=(TcpSocket&& other) noexcept {
        if (this != &other) {
            if (m_fd != -1) {
                std::cout << " [Socket] Closing current FD " << m_fd << " before taking " << other.m_fd << "\n";
            }
            m_fd = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }

    int get_fd() const { return m_fd; }

private:
    int m_fd = -1;
};

int main() {
    std::cout << "--- Exercise: Move-Only TcpSocket ---\n";

    {
        TcpSocket s1(10);
        assert(s1.get_fd() == 10);

        TcpSocket s2 = std::move(s1);
        assert(s1.get_fd() == -1);
        assert(s2.get_fd() == 10);
        std::cout << " Ownership transferred s1 -> s2\n";

        TcpSocket s3(20);
        s3 = std::move(s2);
        assert(s2.get_fd() == -1);
        assert(s3.get_fd() == 10);
        std::cout << " Ownership transferred s2 -> s3 (s3's original FD closed)\n";
    }

    std::cout << "Exercise passed!\n";
    return 0;
}
