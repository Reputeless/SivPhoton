﻿
# include <Siv3D.hpp> // OpenSiv3D v0.6.3
# include "NetworkSystem.hpp"
# include "ENCRYPTED_PHOTON_APP_ID.SECRET"

class MyNetwork : public SivPhoton
{
public:

	using SivPhoton::SivPhoton;

	void connectReturn(const int32 errorCode, const String& errorString, const String& region, const String& cluster) override
	{
		Print << U"MyNetwork::connectReturn() [サーバへの接続に成功したときに(?) 呼ばれる]";
		Print << U"- error: " << errorString;
		Print << U"- region: " << region;
		Print << U"- cluster [サブ的な region]: " << cluster;

		if (errorCode)
		{
			return;
		}

		if (MessageBoxResult::Yes == System::MessageBoxYesNo(U"新しくルームを作りますか？", MessageBoxStyle::Question))
		{
			const String roomName = m_defaultRoomName;

			this->opCreateRoom(roomName, MaxPlayers);
		}
	}

	void connectionErrorReturn(const int32 errorCode) override
	{
		Print << U"MyNetwork::connectionErrorReturn() [サーバへの接続が失敗したときに呼ばれる]";
		Print << U"errorCode: " << errorCode;
	}

	void joinRandomRoomReturn(const int32 localPlayerID, const int32 errorCode, const String& errorString) override
	{
		Print << U"MyNetwork::joinRandomRoomReturn() [既存のランダムなルームに参加した結果を処理する]";
		Print << U"- localPlayerID [失敗した場合は 0 (?)]: " << localPlayerID;
		Print << U"- errorCode: " << errorCode;
		Print << U"- errorString: " << errorString;

		if (errorCode == NetworkSystem::NoRandomMatchFound) // 100% こうなるはず
		{
			Print << U"[既存のルームが見つからなかった]";

			const String roomName = m_defaultRoomName;

			// 自分でルームを新規作成する
			this->opCreateRoom(roomName, MaxPlayers);
		}
	}

	void joinRoomReturn(const int32 localPlayerID, const int32 errorCode, const String& errorString) override
	{
		Print << U"MyNetwork::joinRoomReturn() [指定したルームに参加した結果を処理する]";
		Print << U"- localPlayerID [失敗した場合は 0 (?)]: " << localPlayerID;
		Print << U"- errorCode: " << errorCode;
		Print << U"- errorString: " << errorString;

		if (errorCode)
		{
			Print << U"[指定したルームに参加できなかった(passwordが間違っています)]";
			return;
		}

		Print << U"[指定したルームに参加できた]";
	}

	/// @brief 受信のコード(サンプル)
	/// @param playerID 送信したプレイヤーのID
	/// @param eventCode イベントコード(番号によって意味を持たせたい場合有用)
	/// @param eventContent 受信した本体
	void customEventAction(const int32 playerID, const int32 eventCode, const Vec2& eventContent) override
	{
		Print << U"MyNetwork::customEventAction(Vec2)";
		Print << U"playerID: " << playerID;
		Print << U"eventCode: " << eventCode;
		Print << U"eventContent: " << eventContent;
	}

	/// @brief 受信のコード(サンプル)
	/// @param playerID 送信したプレイヤーのID
	/// @param eventCode イベントコード(番号によって意味を持たせたい場合有用)
	/// @param eventContent 受信した本体
	void customEventAction(const int32 playerID, const int32 eventCode, const double eventContent) override
	{
		Print << U"MyNetwork::customEventAction(double)";
		Print << U"playerID: " << playerID;
		Print << U"eventCode: " << eventCode;
		Print << U"eventContent: " << eventContent;
	}

	/// @brief 受信のコード(Gridサンプル)
	/// @param playerID 送信したプレイヤーのID
	/// @param eventCode イベントコード(番号によって意味を持たせたい場合有用)
	/// @param eventContent 受信した本体
	void customEventAction(const int32 playerID, const int32 eventCode, const Array<Triangle>& eventContent) override
	{
		Print << U"MyNetwork::customEventAction(Array<Triangle>)";
		Print << U"playerID: " << playerID;
		Print << U"eventCode: " << eventCode;
		Print << U"eventContent: " << eventContent;
	}

	/// @brief 受信のコード(Gridサンプル)
	/// @param playerID 送信したプレイヤーのID
	/// @param eventCode イベントコード(番号によって意味を持たせたい場合有用)
	/// @param eventContent 受信した本体
	void customEventAction(const int32 playerID, const int32 eventCode, const Grid<Vec4>& eventContent) override
	{
		Print << U"MyNetwork::customEventAction(Grid<Vec4>)";
		Print << U"playerID: " << playerID;
		Print << U"eventCode: " << eventCode;
		Print << U"eventContent: " << eventContent;
	}

private:

	static constexpr int32 MaxPlayers = 2;
};

void Main()
{
	Window::Resize(1280, 720);

	const std::string encryptedAppID{ SIV3D_OBFUSCATE(ENCRYPTED_PHOTON_APP_ID) };
	const String appID = Unicode::WidenAscii(encryptedAppID);

	MyNetwork network{ appID, U"1.0" };

	network.connect(U"Siv");

	const Font font{ 18 };

	while (System::Update())
	{
		font(U"getRoomNameList: {}"_fmt(network.getRoomNameList())).draw(520, 270);
		font(U"getName: {}"_fmt(network.getName())).draw(520, 300);
		font(U"getUserID: {}"_fmt(network.getUserID())).draw(520, 330);
		font(U"isInRoom: {}"_fmt(network.isInRoom())).draw(520, 360);
		font(U"getCurrentRoomName: {}"_fmt(network.getCurrentRoomName())).draw(520, 390);

		font(U"getPlayerCountInCurrentRoom: {}"_fmt(network.getPlayerCountInCurrentRoom())).draw(520, 420);
		font(U"getMaxPlayersInCurrentRoom: {}"_fmt(network.getMaxPlayersInCurrentRoom())).draw(520, 450);
		font(U"getIsOpenInCurrentRoom: {}"_fmt(network.getIsOpenInCurrentRoom())).draw(520, 480);

		font(U"localPlayerID: {}"_fmt(network.localPlayerID())).draw(520, 510);
		font(U"isMasterClient: {}"_fmt(network.isMasterClient())).draw(520, 540);

		if (network.isMasterClient())
		{
			Scene::SetBackground(Palette::Skyblue);
		}
		else
		{
			Scene::SetBackground(Palette::DefaultBackground);
		}

		if (SimpleGUI::Button(U"Raise Event Array<Triangle>", Vec2{ 800, 20 }))
		{
			network.opRaiseEvent(33, Array<Triangle>{ { Scene::CenterF(), 100 }, { Scene::CenterF().movedBy(200, -100), 200 }});
		}
		if (SimpleGUI::Button(U"Raise Event Grid<Vec4>", Vec2{ 800, 70 }))
		{
			network.opRaiseEvent(34, Grid<Vec4>{1, 1, Array<Vec4>{ {3.3, 5.5, 7.7, 9.9} }});
		}

		if (not network.isInRoom())
		{
			const Array<String> roomList = network.getRoomNameList();

			for (auto [i, roomName] : Indexed(roomList))
			{
				if (SimpleGUI::Button(roomName, Vec2{ 600, 20 + i * 40 }))
				{
					network.opJoinRoom(roomName);
				}
			}
		}

		if (SimpleGUI::Button(U"Leave Room", Vec2{ 600, 180 }))
		{
			network.opLeaveRoom();
		}

		if (SimpleGUI::Button(U"Disconnect", Vec2{ 600, 220 }))
		{
			network.disconnect();
		}

		network.update();
	}
}
