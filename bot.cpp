#define PRINT_PREFIX "[DEBUG] "
#define ERROR_PREFIX "[ERROR] "

#define TOKEN_FILE "bot.token"
#define ID_FILE "bot.id"

#define CONFIG_FILE "config.json"

#include "lib/json.hpp" // used for json (obviously)

#include "sleepy_discord/websocketpp_websocket.h" // discord library
#include <fstream> // used for reading files
#include <iostream>
#include <vector>

using json = nlohmann::json;
using namespace std;

string BOT_ID;     		// loaded before the bot is ran
string BOT_TOKEN;		//
string HELPMSG;

json CONFIG;			// loaded when the bot has loaded

string CFG_PREFIX;		// commands prefix 
bool CFG_PREFIX_SPACE;		// of there is a space after the prefix or not
unsigned int CFG_PREFIX_LEN;	// length of the prefix

bool CFG_HELP_ALLOW = false;	// Allow the help command to display all the commands
string CFG_HELP_CMD = "help";	// Help command

ifstream fileHandle;		// handle for the file reading 

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

// generate a message string for the help command
void generateHelpString( json cfg ) {
	string helpStr = ">>> \n__**Bot Commands**__\n\n";
	string key;
	for (json::iterator it = cfg.begin(); it != cfg.end(); ++it ) {
		key = (string)(it.key());	
		if( cfg[key].find("desc") != cfg[key].end() ) { // if there is no description then dont show the command in the list
			helpStr += "**" + key + "** : `" + (string)cfg[key]["desc"] + "`\n\n";
		}
	}
	HELPMSG = helpStr;
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

	CFG_HELP_ALLOW = CONFIG["allowHelpCmd"];
	CFG_HELP_CMD = CONFIG["helpCmd"];

	if( CFG_PREFIX_SPACE == true ) {
		CFG_PREFIX_LEN++;
	}
}

// String manipulation functions

bool isStartingWithPrefix( string text ) {
	return text.find( CFG_PREFIX ) == 0;
}

bool commandExists( string cmd ) {
	auto cmdFind = CONFIG["cmds"].find( cmd );
	return cmdFind != CONFIG["cmds"].end();
}

string runBotCommand( string text ) {
	if( CFG_PREFIX_SPACE == true ) {
		// split the text
		vector<string> res;
		istringstream iss( text );
		for( string arg; iss >> arg; ) {
			res.push_back( arg );
		}

		if( res[0] == CONFIG["cmdPrefix"] ) {
			if( res.size() == 2 && !res[1].empty() ) { // there are only 1 argument so no need to check if it is greater than or not
			
				if( commandExists(res[1]) ) { // check if the command exists
					return CONFIG["cmds"][res[1]]["returnMsg"];
				} else if (res[1] == CFG_HELP_CMD && CFG_HELP_ALLOW) {
					return HELPMSG;
				}
			}
		}
	} else {
		if( isStartingWithPrefix(text) ) {
			// cut out the prefix
			string cmd = text.substr( CFG_PREFIX_LEN, text.length() );
			if( commandExists(cmd) ) {
				return CONFIG["cmds"][cmd]["returnMsg"];
			} else if (cmd == CFG_HELP_CMD && CFG_HELP_ALLOW) { // help command
				return HELPMSG;
			}
		}
	}
	return "";
}

class BotClient : public SleepyDiscord::DiscordClient {
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;

		void onReady( string* json ) override {
			// load the config JSON
			readConfigJSON( CONFIG_FILE );
			updateConfig(); // update the JSON object

			print("Bot configuration loaded.");
			
			// Update bot status
			updateStatus(CONFIG["statusText"]);
			
			if( CFG_HELP_ALLOW ) {
				// Generate help command
				generateHelpString( CONFIG["cmds"] );
				// print(HELPMSG);
			}
		}

		void onMessage( SleepyDiscord::Message msg ) {
			string username = getUserNameID( msg.author );
			print( username + " sent: " + msg.content );

			if( isStartingWithPrefix( msg.content ) ) {
				string replyTxt = runBotCommand( msg.content );

				if( replyTxt != "" ) {
					sendMessage( msg.channelID, replyTxt );
				}
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
