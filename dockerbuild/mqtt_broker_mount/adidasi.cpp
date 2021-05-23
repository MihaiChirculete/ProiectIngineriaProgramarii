#include <algorithm>

#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>

#include <signal.h>

using namespace std;
using namespace Pistache;

//Structura user
//cand by default username ul este N/A
//acest lucru il vom folosi pentru a verifica daca putem folosi anumite functionalitati
//care nu ar fi posibile fara un user definit


struct user{
    string username = "N/A";
    string inaltime = "";
    string masa = "";
};

//functie pentru a verifica daca un string este numeic
//o vom folosi pentru a verifica inputul pentru inaltime si greutate

bool isnumeric(string str){
    for (int i = 0; i < str.length(); i++){
        if(!isdigit(str[i]))
            return false;
    }
    return true;
}
//De la rares
// General advice: pay atetntion to the namespaces that you use in various contexts. Could prevent headaches.
// This is just a helper function to preety-print the Cookies that one of the enpoints shall receive.
void printCookies(const Http::Request& req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto& c: cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

// Some generic namespace, with a simple function we could use to test the creation of the endpoints.
namespace Generic {

    void handleReady(const Rest::Request&, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

}

// Definition of the AdidasiEnpoint class
class AdidasiEndpoint {
public:
    explicit AdidasiEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    // Initialization of the server. Additional options can be provided here
    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(static_cast<int>(thr));
        httpEndpoint->init(opts);
        // Server routes are loaded up
        setupRoutes();
    }

    // Server is started threaded.
    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();
    }

    // When signaled server shuts down
    void stop(){
        httpEndpoint->shutdown();
    }

private:
    void setupRoutes() {
        using namespace Rest;
        // Defining various endpoints
        // Generally say that when http://localhost:9080/ready is called, the handleReady function should be called. 
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/auth", Routes::bind(&AdidasiEndpoint::doAuth, this));
        Routes::Post(router, "/settings/:settingName/:value", Routes::bind(&AdidasiEndpoint::setSetting, this));
        Routes::Get(router, "/settings/:settingName/", Routes::bind(&AdidasiEndpoint::getSetting, this));
        Routes::Post(router, "/setUser/:username/:inaltime/:masa/", Routes::bind(&AdidasiEndpoint::setUser, this));
        Routes::Get(router, "/getUser/", Routes::bind(&AdidasiEndpoint::getUser, this));


    	//Adaugat la 22.05
    	// Routes::Get(router, "/move", Routes::bind(&AdidasiEndpoint::move, this));
 
    }
    	//getter pentru a afisa informatiile user-ului
        void getUser(const Rest::Request& request, Http::ResponseWriter response){
            user user = addidasi.detalii();
            if (user.username != "N/A") {
                response.send(Http::Code::Ok, "Username: " + user.username + " \nInaltime: " + user.inaltime + "\nMasa: " + user.masa + "!");
            }
            else {
                response.send(Http::Code::Not_Found, "Userul nu s-a salvat :( pentru ca inputul nu corespunde cerintelor.");
            }
        }
 
        //functie pentru a seta informatiile pentru user
    void setUser(const Rest::Request& request, Http::ResponseWriter response){
        string username = request.param(":username").as<std::string>();
        string inaltime = request.param(":inaltime").as<std::string>();
        string masa = request.param(":masa").as<std::string>();

        // This is a guard that prevents editing the same value by two concurent threads. 
        Guard guard(adidasiLock);
 

 
        // Setting the adidasi's setting to value
        int setResponse = addidasi.init(username, inaltime, masa);

 
        // Sending some confirmation or error response.
        if (setResponse == 1) {
            response.send(Http::Code::Ok, "Welcome " + username + "!");
        }
        else {
            response.send(Http::Code::Not_Found, "Userul nu s-a salvat :( pentru ca inputul nu corespunde cerintelor.");
        }
    }

    //De la rares
    void doAuth(const Rest::Request& request, Http::ResponseWriter response) {
        // Function that prints cookies
        printCookies(request);
        // In the response object, it adds a cookie regarding the communications language.
        response.cookies()
            .add(Http::Cookie("lang", "en-US"));
        // Send the response
        response.send(Http::Code::No_Content);
    }
 
    // Endpoint to configure one of the Microwave's settings.
    void setSetting(const Rest::Request& request, Http::ResponseWriter response){
        user detaliiUser = addidasi.detalii();
        if (detaliiUser.username == "N/A"){
            response.send(Http::Code::Not_Acceptable, "Userul trebuie definit.");
        }
        // You don't know what the parameter content that you receive is, but you should
        // try to cast it to some data structure. Here, I cast the settingName to string.
        auto settingName = request.param(":settingName").as<std::string>();
 
        // This is a guard that prevents editing the same value by two concurent threads. 
        Guard guard(adidasiLock);
 
 
        string val = "";
        if (request.hasParam(":value")) {
            auto value = request.param(":value");
            val = value.as<string>();
        }
 
        // Setting the microwave's setting to value
        int setResponse = addidasi.set(settingName, val);
 
        // Sending some confirmation or error response.
        if (setResponse == 1) {
            response.send(Http::Code::Ok, settingName + " was set to " + val);
        }
        else {
            response.send(Http::Code::Not_Found, settingName + " was not found and or '" + val + "' was not a valid value ");
        }
 
    }
 
    // Setting to get the settings value of one of the configurations of the Microwave
    void getSetting(const Rest::Request& request, Http::ResponseWriter response){
        user detaliiUser = addidasi.detalii();
        if (detaliiUser.username == "N/A"){
            response.send(Http::Code::Not_Acceptable, "Userul trebuie definit.");
        }
        auto settingName = request.param(":settingName").as<std::string>();
 
        Guard guard(adidasiLock);
 
        string valueSetting = addidasi.get(settingName);
 
        if (valueSetting != "") {
 
            // In this response I also add a couple of headers, describing the server that sent this response, and the way the content is formatted.
            using namespace Http;
            response.headers()
                        .add<Header::Server>("pistache/0.1")
                        .add<Header::ContentType>(MIME(Text, Plain));
 
            response.send(Http::Code::Ok, settingName + " is " + valueSetting);
        }
        else {
            response.send(Http::Code::Not_Found, settingName + " was not found");
        }
    }
 
    // Defining the class of the Microwave. It should model the entire configuration of the Microwave
    class Adidasi {
    public:
        explicit Adidasi(){ }
 
        // Setting the value for one of the settings. Hardcoded for the defrosting option
        //
        //Initializam user-ul cu informatiile introduse
        int init(string username, string inaltime, string masa){
            if (!isnumeric(inaltime) || !isnumeric(masa) )
                return 0;
            detaliiUser.username = username;
            detaliiUser.inaltime = inaltime;
            detaliiUser.masa = masa;
            return 1;
        }

        //funtie pentru a obtine informatiile user-ului
        user detalii(){
            return detaliiUser;
        }

        //setter pentru functionalitati
        //aici practic prelucram datele in functie de input

        int set(std::string name, std::string value){
            if(name == "suntLegati"){
                suntLegati.name = name;
                if(value == "true"){
                    suntLegati.value = true;
                    return 1;
                }
                if(value == "false"){
                    suntLegati.value = false;
                    return 1;
                }
            }
            if(name == "miscare")
            {
            	if(value == "true"){
                    miscare.pasi++;
                    miscare.calorii = 2*miscare.pasi;
                    miscare.value = true;
                    return 1;
            	}
            	if(value == "false"){
            		miscare.value = false;
            		return 1;
            	}
            }
            return 0;
        }
 
        // Getter
        string get(std::string name){
            if (name == "suntLegati"){
                if(suntLegati.value)
                    return "Sunt legati!";
                else return "Nu sunt legati!";
            }
            else if (name == "miscare"){
                string statistica = std::to_string(miscare.pasi) +  " pasi, si a ars " + std::to_string(miscare.calorii) + " calorii";
                if(miscare.value)
                    return "Se misca. A facut " + statistica;
                else return "Nu se misca. A facut " + statistica;
            }
            return " functionalitatea cautata nu s-a implementat inca";

        }
 
    private:
        // Defining and instantiating settings.
        struct boolSetting{
            std::string name;
            bool value = false;
        }suntLegati;

        struct {
            int pasi = 0;
            int calorii = 0;
            bool value = 0;
        }miscare;

        user detaliiUser;

        
        ///Name = stilul in care sunt legati
        //value = true daca sunt legati, false daca nu
    };

    
 
    // Create the lock which prevents concurrent editing of the same variable
    using Lock = std::mutex;
    using Guard = std::lock_guard<Lock>;
    Lock adidasiLock;
 
    // Instance of the microwave model
    Adidasi addidasi;
 
    // Defining the httpEndpoint and a router.
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};
 
int main(int argc, char *argv[]) {
 
    // This code is needed for gracefull shutdown of the server when no longer needed.
    sigset_t signals;
    if (sigemptyset(&signals) != 0
            || sigaddset(&signals, SIGTERM) != 0
            || sigaddset(&signals, SIGINT) != 0
            || sigaddset(&signals, SIGHUP) != 0
            || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0) {
        perror("install signal handler failed");
        return 1;
    }
 
    // Set a port on which your server to communicate
    Port port(9084);
 
    // Number of threads used by the server
    int thr = 2;
 
    if (argc >= 2) {
        port = static_cast<uint16_t>(std::stol(argv[1]));
 
        if (argc == 3)
            thr = std::stoi(argv[2]);
    }
 
    Address addr(Ipv4::any(), port);
 
    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;
 
    // Instance of the class that defines what the server can do.
    AdidasiEndpoint stats(addr);
 
    // Initialize and start the server
    stats.init(thr);
    stats.start();
 
 
    // Code that waits for the shutdown sinal for the server
    int signal = 0;
    int status = sigwait(&signals, &signal);
    if (status == 0)
    {
        std::cout << "received signal " << signal << std::endl;
    }
    else
    {
        std::cerr << "sigwait returns " << status << std::endl;
    }
 
    stats.stop();
}
