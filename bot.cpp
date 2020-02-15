#define PRINT_PREFIX "[DEBUG] "
#define ERROR_PREFIX "[ERROR] "

#define TOKEN_FILE "bot.token"
#define ID_FILE "bot.id"

#define CONFIG_FILE "config.json"

#include "lib/json.hpp" // used for json (obviously)

#include "sleepy_discord/websocketpp_websocket.h" // discord library
#include <fstream> // used for reading files
#include <iostream>

using json = nlohmann::json;
using namespace std;

string BOT_ID;     		// loaded before the bot is ran
string BOT_TOKEN;		// 

json CONFIG;			// loaded when the bot has loaded

string CFG_PREFIX;					// commands prefix 
bool CFG_PREFIX_SPACE;				// of there is a space after the prefix or not
unsigned int CFG_PREFIX_LEN;		// length of the prefix

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

// get the typical username format for discord like "user#1224"
string getUserNameID( SleepyDiscord::User user ) { 
	return user.username + "#" + user.discriminator; 
} 

// Config loading
void readConfigJSON( string filename ) {
	ifstream jsonData( "config.json" );

	if( !jsonData ) {
		error( "Could not read from JSON config file. Aborting..." );
		exit(1);
	}
	jsonData >> CONFIG;
}

void updateConfig() {
	CFG_PREFIX = CONFIG["cmdPrefix"];
	CFG_PREFIX_SPACE = CONFIG["cmdPrefixSpace"];
	CFG_PREFIX_LEN = CFG_PREFIX.length();

	if( CFG_PREFIX_SPACE == true ) {
		CFG_PREFIX_LEN++;
	}
}

class BotClient : public SleepyDiscord::DiscordClient {
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;

		void onReady( string* json ) override {
			// load the config JSON
			readConfigJSON( CONFIG_FILE );
			updateConfig(); // update the JSON object

			print("Bot configuration loaded.");
		}

		void onMessage( SleepyDiscord::Message msg ) {
			string username = getUserNameID( msg.author );
			print( username + " sent: " + msg.content );
			
			if( msg.startsWith(CFG_PREFIX) ) { // check if the message starts with the command prefix
				// print( getUserNameID( msg.author ) + " issued command: " + msg.content );
				// sendMessage( msg.channelID, "General Kenobi!" );
				print("Starts with prefix!" );

				// get the second argument
			}
		}
};

int main() {
	BOT_TOKEN = getBotToken(); // cache it so that we don't have to call these functions all of the time
	BOT_ID = getBotID();

	BotClient client( BOT_TOKEN, 2 );
	client.run();
	
	return 0;
}