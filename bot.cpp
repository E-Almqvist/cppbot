#define PRINT_PREFIX "[DEBUG] "
#define ERROR_PREFIX "[ERROR] "

#define TOKEN_FILE "bot.token"
#define ID_FILE "bot.id"

#define CONFIG_FILE "config.json"

#include "lib/json.hpp" // used for json (obviously)

#include "sleepy_discord/websocketpp_websocket.h" // discord library
#include <fstream> // used for reading files

using json = nlohmann::json;
using namespace std;

string BOT_ID;     		// loaded before the bot is ran
string BOT_TOKEN;		// 

json CONFIG;			// loaded when the bot has loaded

ifstream fileHandle;	// handle for the file reading 

// Debug functions
void print( string txt ) { cout << PRINT_PREFIX << txt << endl; }
void error( string txt ) { cerr << ERROR_PREFIX << txt << endl; }

// Info functions
string readFromFile( string filename ) {
	string cont;

	fileHandle.open( filename );

	if( !fileHandle ) {
		error( "Could not read from file '" + filename + "'. Aborting..." );
		exit(1);
	}

	fileHandle >> cont;
	fileHandle.close();

	return cont;
}

string getBotToken() { return readFromFile( TOKEN_FILE ); }
string getBotID() { return readFromFile( ID_FILE ); }

string getUserNameID( SleepyDiscord::User user ) { return user.username + "#" + user.discriminator; } // get the typical username format for discord like "user#1224"

// Config loading
void readConfigJSON( string filename ) {
	ifstream jsonData( "config.json" );
	jsonData >> CONFIG;
}

class BotClient : public SleepyDiscord::DiscordClient {
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;

		void onReady( std::string* json ) override {
			// load the config json
			readConfigJSON( CONFIG_FILE );
			print("Bot configuration loaded.");
		}

		void onMessage( SleepyDiscord::Message msg ) {
			string username = getUserNameID( msg.author );

			if( msg.startsWith(CONFIG["cmdPrefix"]) ) { // check if the message starts with the command prefix
				// print( getUserNameID( msg.author ) + " issued command: " + msg.content );
				// sendMessage( msg.channelID, "General Kenobi!" );
				print("Starts with prefix!" );
			// } else {
			// 	print( username + " sent: " + msg.content );
			}
			print( username + " sent: " + msg.content );
		}
};

int main() {
	BOT_TOKEN = getBotToken(); // cache it so that we don't have to call these functions all of the time
	BOT_ID = getBotID();

	BotClient client( BOT_TOKEN, 2 );
	client.run();
	
	return 0;
}