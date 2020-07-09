#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct HourlyPrices {
  double spot;
  double dd_1h;
  double dd_6h;
  double demand;

  HourlyPrices() : spot{0.0}, dd_1h{0.0}, dd_6h{0.0}, demand{0.0} {}
};

inline std::ostream &operator<<(std::ostream &out, const HourlyPrices &t) {
  out << "{spot: $" << t.spot << "/hr, dd_1h: $" << t.dd_1h
      << "/hr, dd_6h: " << t.dd_6h << "/hr, on_demand: $" << t.demand
      << "/hr,}";

  if (t.demand == 0.0) {
    return out;
  }

  if (t.spot != 0.0) {
    out << "  spot: " << (t.demand / t.spot) << "x";
  }
  if (t.dd_1h != 0.0) {
    out << "  dd_1h: " << (t.demand / t.dd_1h) << "x";
  }
  if (t.dd_6h != 0.0) {
    out << "  dd_3h: " << (t.demand / t.dd_6h) << "x";
  }

  return out;
}

auto parse_price(const std::string &s) -> std::optional<double> {
  if (s.empty() || s[0] != '$') {
    return std::nullopt;
  }
  std::istringstream iss_price{s.substr(1)};
  double price;
  iss_price >> price;
  if (!iss_price.bad()) {
    return {price};
  }
  return std::nullopt;
}

int main() {
  std::map<std::string, HourlyPrices> prices;

  {
    std::string line;
    std::ifstream ifs{"spot.txt"};
    while (ifs.good()) {
      std::getline(ifs, line);

      std::istringstream iss{std::move(line)};

      if (!iss.good()) {
        continue;
      }
      std::string instance_kind;
      iss >> instance_kind;

      if (!iss.good()) {
        continue;
      }
      std::string linux_price_str;
      iss >> linux_price_str;
      if (!iss.bad() && !linux_price_str.empty() && linux_price_str[0] == '$') {
        std::istringstream iss_price{std::move(linux_price_str).substr(1)};
        double price;
        iss_price >> price;
        if (!iss_price.bad()) {
          prices[instance_kind].spot = price;
        }
      }
    }
  }
  {
    std::string line;
    std::ifstream ifs{"dd.txt"};
    while (ifs.good()) {
      std::getline(ifs, line);

      std::istringstream iss{std::move(line)};

      if (!iss.good()) {
        continue;
      }
      std::string instance_kind;
      iss >> instance_kind;

      if (!iss.good()) {
        continue;
      }
      std::string linux_price_str;
      iss >> linux_price_str;
      if (!iss.bad() && linux_price_str[0] == '$') {
        auto price = parse_price(linux_price_str);
        if (price) {
          prices[instance_kind].dd_1h = *price;
        }
      }

      {
        std::string word;
        std::optional<double> price;
        while (iss.good()) {
          iss >> word;
          price = parse_price(word);
          if (price) {
            prices[instance_kind].dd_6h = *price;
            break;
          }
        }
      }
    }
  }
  {
    std::string line;
    std::ifstream ifs{"on_demand.txt"};
    while (ifs.good()) {
      std::getline(ifs, line);

      std::istringstream iss{std::move(line)};

      if (!iss.good()) {
        continue;
      }
      std::string instance_kind;
      iss >> instance_kind;

      std::string word;
      std::optional<double> price;
      while (iss.good()) {
        iss >> word;
        price = parse_price(word);
        if (price) {
          prices[instance_kind].demand = *price;
          break;
        }
      }
    }
  }

  for (const auto &item : prices) {
    std::cout << item.first << " => " << item.second << std::endl;
  }

  return 0;
}
