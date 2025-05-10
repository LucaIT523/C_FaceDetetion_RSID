// RSID_C_Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <thread>
#include "FaceJSONDB.hpp"
#include "RSIDWrapper.hpp"


//.
#ifdef _WIN32
RealSenseID::SerialConfig g_PORT{ "COM7" };
#elif LINUX
RealSenseID::SerialConfig g_PORT{ "/dev/ttyACM0" };
#endif
std::string		faceprints_db_path = "data.json";
int				g_NumThread = 3;
CFaceJSONDB		g_CFaceJSONDB;


std::unique_ptr<RealSenseID::FaceAuthenticator> CreateAuthenticator(const RealSenseID::SerialConfig& p_PORT)
{
	auto authenticator = std::make_unique<RealSenseID::FaceAuthenticator>();
	auto connect_status = authenticator->Connect(p_PORT);
	if (connect_status != RealSenseID::Status::Ok)
	{
		std::cout << "Failed connecting to port " << p_PORT.port << " status:" << connect_status << std::endl;
		std::exit(1);
	}
	std::cout << "Connected to device" << std::endl;
	return authenticator;
}



//    Main function for the faceprints authentication program.
int main()
{
	//. Init RSID authenticator
	auto authenticator = CreateAuthenticator(g_PORT);
	CFaceprintsAuthClbk   w_CFaceprintsAuthClbk(authenticator.get());
	int					  w_face_DB_Cnt = 0;

	//. load_faceprints_db()
	g_CFaceJSONDB.LoadJSONFile(faceprints_db_path);
	g_CFaceJSONDB.GetDataCount(w_face_DB_Cnt);
	w_CFaceprintsAuthClbk.ClearFacePrintsDB();
	for (int i = 0; i < w_face_DB_Cnt; i++) {
		FaceData	w_JSONFaceData;
		g_CFaceJSONDB.GetFaceData(i, w_JSONFaceData);
		w_CFaceprintsAuthClbk.AddFacePrintsDB(w_JSONFaceData);
	}

	//. Loop 
	while (true)
	{
		// extract faceprints of the user in front of the device
		auto status = authenticator->ExtractFaceprintsForAuth(w_CFaceprintsAuthClbk);
		if (status != RealSenseID::Status::Ok)
			std::cout << "Status: " << status << std::endl << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));

//		w_CFaceprintsAuthClbk.TestingCode();
	}
	return 0;
}
