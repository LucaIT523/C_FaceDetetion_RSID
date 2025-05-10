#ifndef __RSIDWrapper_HPP__
#define __RSIDWrapper_HPP__

#include "RealSenseID/FaceAuthenticator.h"
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <thread>
#include "FaceJSONDB.hpp"


struct FaceMatchSate {
    bool        m_bSuccess;
    int         m_nCurrentScore;
};

// authenticate with faceprints
class CFaceprintsAuthClbk : public RealSenseID::AuthFaceprintsExtractionCallback
{
public:
    CFaceprintsAuthClbk(RealSenseID::FaceAuthenticator* authenticator) : m_authenticator(authenticator)
    {
    }
    //. 
    int match_users_parallel(RealSenseID::MatchElement new_prints, std::vector<RealSenseID::Faceprints> faceprints_db, RealSenseID::FaceAuthenticator* f_FaceAuthenticator);
    
    //.
    FaceMatchSate match_single_user(RealSenseID::MatchElement new_prints, RealSenseID::Faceprints user, RealSenseID::FaceAuthenticator* f_FaceAuthenticator);

    //
    void OnResult(const RealSenseID::AuthenticateStatus status, const RealSenseID::ExtractedFaceprints* new_prints) override;

    //.
    void OnHint(const RealSenseID::AuthenticateStatus hint) override;
 
    void OnFaceDetected(const std::vector<RealSenseID::FaceRect>& faces, const unsigned int ts) override;

    void AddFacePrintsDB(struct FaceData p_JSONFaceData);

    void ClearFacePrintsDB();

//    void TestingCode();

private:
    RealSenseID::FaceAuthenticator*         m_authenticator;

};

extern CFaceJSONDB		g_CFaceJSONDB;
extern int				g_NumThread;
#endif