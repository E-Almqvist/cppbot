#define PRINT_PREFIX "[DEBUG] "

#include "sleepy_discord/websocketpp_websocket.h" // discord library
#include <fstream> // used for reading files

using namespace std;

string BOT_ID;      
string BOT_TOKEN;

ifstream fileHandle;

void print( string txt ) { cout << PRINT_PREFIX << txt << endl; }
void error( string txt ) { cerr << txt << endl; }

// get read token and id files functions

string getBotToken() {
	fileHandle.open( "bot.token" );

	if( !fileHandle ) {
		error( "Couldn't read token from file." );
		exit(1);
	}

	while( fileHandle >> BOT_TOKEN ) {

	}
}

string getUserNameID( SleepyDiscord::User user ) {
	return user.username + "#" + user.discriminator;
}

class BotClient : public SleepyDiscord::DiscordClient {
	public:
		using SleepyDiscord::DiscordClient::DiscordClient;

		void onReady() {
			updateStatus( "Test Bot" );
		}

		void onMessage( SleepyDiscord::Message msg ) {
			print( getUserNameID( msg.author ) + " sent: " + msg.content );

			if( msg.isMentioned( BOT_ID ) ) {
				sendMessage( msg.channelID, "Hello" );
			}
		}
};

int main() {
	BotClient client( BOT_TOKEN, 2 );
	client.run();
	
	return 0;
}