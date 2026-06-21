#include <iostream>
#include <memory>
#include <vector>
#include <string>

/**
 * MarketDataFeed: Represents a shared resource that multiple subscribers 
 * need to observe. We use shared_ptr because we don't know who will 
 * be destroyed last.
 */
struct MarketDataFeed {
    std::string exchange_name;
    MarketDataFeed(std::string name) : exchange_name(std::move(name)) {
        std::cout << " [Feed] Connection to " << exchange_name << " established\n";
    }
    ~MarketDataFeed() {
        std::cout << " [Feed] Connection to " << exchange_name << " CLOSED\n";
    }
};

class Subscriber {
public:
    Subscriber(std::string name, std::shared_ptr<MarketDataFeed> feed) 
        : m_name(std::move(name)), m_feed(std::move(feed)) {
        std::cout << " [Sub] " << m_name << " subscribed to " << m_feed->exchange_name << "\n";
    }

private:
    std::string m_name;
    std::shared_ptr<MarketDataFeed> m_feed;
};

int main() {
    std::cout << "--- production_example: Shared Market Data Feed ---\n\n";

    // 1. Create the shared resource
    auto nyse_feed = std::make_shared<MarketDataFeed>("NYSE");
    std::cout << " Feed Use Count: " << nyse_feed.use_count() << "\n\n";

    {
        // 2. Multiple subscribers take ownership
        std::cout << " Creating subscribers...\n";
        Subscriber sub1("Algorithm_A", nyse_feed);
        Subscriber sub2("Risk_Monitor", nyse_feed);
        
        std::cout << " Feed Use Count: " << nyse_feed.use_count() << "\n";
    } // sub1 and sub2 are destroyed, but nyse_feed exists in main

    std::cout << "\n Subscribers out of scope. Feed Use Count: " << nyse_feed.use_count() << "\n";
    
    std::cout << " End of main.\n";
    return 0;
}
