#ifndef TRADESIM_MARKET_HPP
#define TRADESIM_MARKET_HPP

namespace tradesim {

class market {
private:
    int m_count;
public:
    market();

    market(const market&) = delete;
    market& operator=(const market&) = delete;

    market(market&&) = delete;
    market& operator=(market&&) = delete;

    void subscribe();
};

}

#endif
