#ifndef CLIENT_SESSION_ICE
#define CLIENT_SESSION_ICE

#include <Glacier2/Session.ice>
#include <IcePatch2/FileServer.ice>
#include <ClientServerEvents.ice>
#include <ChatInfo.ice>
#include <ClientInterface.ice>

[["java:package:net.lbanet.generated"]]
module LbaNet
{
	interface ClientSession extends Glacier2::Session
	{
		// return the server version - used for client initialization
	 	string GetVersion();
	 	
	 	//client send interface to server
	 	void SetClientInterface(ClientInterface * winterface);
	 	
	 	//client send events to server
	 	void ClientEvents(EventsSeq evts);
	 	
	 	
	 	// client ask server to change world
	 	void ChangeWorld(string WorldName);
	 	
	 	// client ask to get the list of world available
	 	void GetWorldList();
	 	
	 	
	 	
	 	// chat management
	 	ChatRoomParticipant* JoinChatChannel(string room, ChatRoomObserver* view);
	    	void LeaveChatChannel(string room, ChatRoomObserver* view);
	    	
	    	// whisper management
	    	void SetWhisperInterface(ChatRoomObserver * winterface);
		bool Whisper(string To, string Message);    	
	    	
		// set status
		void ChangeStatus(string Status);
		void ChangeNameColor(string Color);  
		void ChangeLocation(string Location);
		
		//get the patcher for a specific world
		IcePatch2::FileServer* GetPatcher(string WorldName);
		
		// called in order to shutdown the world server, update the world data, then restart the server
		string UpdateWorld(string WorldName);
		
		// start server for world if not yet started
		string StartWorld(string WorldName);
		
		// stop server for world if started
		string ShutdownWorld(string WorldName);
		
		// check if server is started or not
		bool GetWorldStatus(string WorldName);		
		
	};

};

#endif

