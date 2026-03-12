#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include <algorithm>

// ── Packet representation
//─────────────────────────────────────
struct Packet {
std::string srcIP, dstIP;
int srcPort, dstPort;
std::string proto; // TCP / UDP / ICMP
std::string direction; // INBOUND / OUTBOUND /FORWARD
std::string payload;
bool isSyn = false;
};

// ── Connection tracking (iptables conntrack) ──────────────────
struct ConnKey {
std::string srcIP; int srcPort;
std::string dstIP; int dstPort;
std::string proto;
bool operator<(const ConnKey& o) const {
std::ostringstream a, b;
a << srcIP<<srcPort<<dstIP<<dstPort<<proto;
b << o.srcIP<<o.srcPort<<o.dstIP<<o.dstPort<<o.proto;
return a.str() < b.str();
}
};

enum ConnState { SYN_SENT, ESTABLISHED, CLOSED };
std::map<ConnKey, ConnState> conntrack;

// ── Firewall rule
//─────────────────────────────────────────────
struct Rule {
std::string chain; // INPUT / OUTPUT /FORWARD
std::string srcNet; // CIDR or "ANY"
std::string dstNet;
int dstPort; // -1 = any
std::string proto; // TCP/UDP/ICMP/ANY
std::string target; // ACCEPT / DROP / REJECT / LOG
std::string comment;
bool stateful; // true = also check conntrack
};

// ── Stateful Firewall Engine
//──────────────────────────────────
class Firewall {
std::vector<Rule> rules_;
int accepted_=0, dropped_=0;

bool matchNet(const std::string& ip, const std::string& net) {
if(net=="ANY") return true;
// simplified: check prefix only (demo)
std::string prefix = net.substr(0, net.find('/'));
return ip.find(prefix.substr(0, prefix.rfind('.'))) == 0;
}

// Check conntrack for ESTABLISHED/RELATED
bool isEstablished(const Packet& p) {
ConnKey k{p.srcIP,p.srcPort,p.dstIP,p.dstPort,p.proto};
ConnKey r{p.dstIP,p.dstPort,p.srcIP,p.srcPort,p.proto}; // reverse
return conntrack.count(k) && conntrack[k]==ESTABLISHED
|| conntrack.count(r) && conntrack[r]==ESTABLISHED;
}

void trackConn(const Packet& p) {
ConnKey k{p.srcIP,p.srcPort,p.dstIP,p.dstPort,p.proto};
if(p.isSyn) conntrack[k]=SYN_SENT;
else if(conntrack.count(k)&&conntrack[k]==SYN_SENT)
conntrack[k]=ESTABLISHED;
}

public:
void addRule(const Rule& r) { rules_.push_back(r); }

std::string process(const Packet& p) {
// Conntrack: allow RELATED/ESTABLISHED automatically
if(isEstablished(p)) {
trackConn(p); accepted_++;
return "ACCEPT (ESTABLISHED)";
}

for(auto& r : rules_) {
bool matchProto = r.proto=="ANY" || r.proto==p.proto;
bool matchSrc = matchNet(p.srcIP, r.srcNet);
bool matchDst = (r.dstPort==-1 || r.dstPort==p.dstPort);
bool matchDir = r.chain == p.direction ||
r.chain == "BOTH";

if(matchProto && matchSrc && matchDst && matchDir) {
// Application-layer DPI
if(!p.payload.empty() &&
p.payload.find("DROP_KEYWORD") != std::string::npos) {
dropped_++;
return "DROP (DPI: banned content)";
}
trackConn(p);
if(r.target=="ACCEPT"||r.target=="LOG") {
accepted_++;
return r.target + " [" + r.comment + "]";
}

dropped_++;
return r.target + " [" + r.comment + "]";
}
}
// Default policy: DROP
dropped_++;
return "DROP (default policy)";
}

void stats() {
std::cout << "\n[FW STATS] Accepted=" << accepted_
<< " Dropped=" << dropped_ << "\n";
}
};
int main() {
    Firewall fw;
    fw.addRule({"INPUT","ANY","ANY",80,"TCP","ACCEPT","Allow HTTP",true});
    fw.addRule({"OUTPUT","ANY","ANY",-1,"ANY","DROP","Block all outbound",false});

    Packet p1{"10.0.0.5","192.168.1.10",12345,80,"TCP","INBOUND","Hello",true};
    Packet p2{"192.168.1.10","10.0.0.5",80,12345,"TCP","OUTBOUND","DROP_KEYWORD",false};

    std::cout << fw.process(p1) << "\n";
    std::cout << fw.process(p2) << "\n";
    fw.stats();
}

// ── Zone helper ───────────────────────