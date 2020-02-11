#define PRINT_PREFIX "[DEBUG] "
#define BOTID "675743438117011506"
#include "sleepy_discord/websocketpp_websocket.h"

void print( std::string txt ) {
    std::cout << PRINT_PREFIX << txt << std::endl;
}

class BotClient : public SleepyDiscord::DiscordClient {
    public:
        using SleepyDiscord::DiscordClient::DiscordClient;

        void onReady() {
            updateStatus( "Test Bot" );
        }

        void onMessage( SleepyDiscord::Message msg ) {
            print( msg.author.username + "#" + msg.author.discriminator + " sent: " + msg.content );

            if( msg.isMentioned( BOTID ) ) {
                sendMessage( msg.channelID, "Hello" );
            }
        }
};

int main() {
    BotClient client( "TOKEN", 2 );
    client.run();
    
    return 0;
}