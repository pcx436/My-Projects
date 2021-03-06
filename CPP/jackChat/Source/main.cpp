#include <encodedMessage.h>
#include <networkManager.h>

using namespace std;

struct thread_stuffs
{
    int thread_id;
    networkManager * perroSocks;
};

// Used in the multi-threading of the sending and receiving threads
void *listenThread(void *threadContent)
{
    pthread_detach(pthread_self());
    struct thread_stuffs *somestuff;
    somestuff = (struct thread_stuffs *) threadContent;
    somestuff->perroSocks->listenr();
    pthread_exit(NULL);
}

void *sendThread(void *threadContent)
{
    struct thread_stuffs *somestuff;
    somestuff = (struct thread_stuffs *) threadContent;
    somestuff->perroSocks->sender();

    pthread_exit(NULL);
}

// The function that opens thens the example IP file and prints out as many examples as requested.
int listIPs(int numOfIP)
{
    if(numOfIP <= 0 || numOfIP > 3000)
        return -1;

    ifstream reader("./validIPs.txt", ios::in);
    if(reader.fail())
        return -2;

    string line;
    for(int i = 0; i < numOfIP && getline(reader, line); i++)
        cout << line << endl;

    reader.close();
    return 0;
}

int main(void)
{
    // The debug boolean variable should be used for debugging purposes only.
    // It will assume that the IP address you're hosting on or connecting to is 127.0.0.1
    bool debug = false, valIP = false;
    string info[3];

    cout << "Welcome to jackChat, a simple TCP IP based chatting system." << endl;
    do
    {
        cout << "Do you want to host?\n1) Yes\n2) No" << endl;
        getline(cin, info[0]);

        if(info[0] != "1" && info[0] != "2")
            cout << "You didn't enter a valid choice. Please enter 1 or 2." << endl;

    } while(info[0] != "1" && info[0] != "2");

    networkManager checker;// used to check entered IP addresses
    while(!valIP && !debug)
    {
        if(stoi(info[0]) == 1)
            cout << "What's your IP address?" << endl;
        else
            cout << "What IP do you want to connect to?" << endl;

        cout << "Enter \"!Ex\" (without quotes) to see some examples of valid IP addresses." << endl;
        cout << "If you don't enter anything, 127.0.0.1 will be assumed." << endl;

        getline(cin, info[1]);
        for(int i = 0; i < info[1].length(); i++)
            info[1][i] = tolower(info[1][i]);


        if(info[1] == "!ex")
        {
            cout << "How many examples would you like to see?\nEnter a number [1, 3000]: ";
            string num;

            stringstream ss;
            getline(cin, num);
            int d;
            ss << num;
            if(!(ss >> d))
            {
                cout << "Uh oh, that's an invalid number!" << endl;
                continue;
            }
            if(d < 1 || d > 3000)
            {
                cout << "Uh oh, that number isn't within the correct range!" << endl;
                continue;
            }

            int ret = listIPs(d);
            if(ret != 0)
            {
                //Something went wrong in listing the IPs
                cout << "Uh oh, something wen't wrong with getting the examples. Error code:" << ret << endl;
                continue;
            }

        }
        else if(info[1].length() > 0)
        {
            string check = checker.validIP(info[1]);
            if(check == info[1])
                valIP = true;
            else
                cout << check << endl;// bad IP address.
        }
        else
        {
            info[1] = "127.0.0.1";
            valIP = true;
        }

    }

    // Used in case you want to get around a lot of what's happening and just test core functionality.
    if(debug)
    {
        if(info[0] == "1")
            info[2] = "Host";
        else
            info[2] = "Other";

        info[1] = "127.0.0.1";
    }

    //Getting username.
    do
    {
        cout << "What's your user-name going to be?" << endl;
        getline(cin, info[2]);

        if(info[2].empty())
            cout << "You must have a user name." << endl;
        else if(info[2].length() > 1500)
            cout << "Your user name is too long (1500 characters max)." << endl;
    } while(info[2].empty() || info[2].length() > 1500);

    // Thread information creation
    pthread_t forListenThread;
    pthread_t forSendThread;
    struct thread_stuffs threadInfo;
    threadInfo.thread_id = 4;
    threadInfo.perroSocks = new networkManager(info);

    // Thread creation
    pthread_create(&forListenThread, NULL, listenThread, (void *)&threadInfo);
    pthread_create(&forSendThread, NULL, sendThread, (void *)&threadInfo);

    // Thread joining
    pthread_join(forSendThread, NULL);

    // Program ending maitnence
    delete threadInfo.perroSocks;// Prevents memory leak

    threadInfo.perroSocks = nullptr;
    return 0;
}
