
#include "RSIDWrapper.hpp"

std::vector<RealSenseID::Faceprints>    faceprints_db;
RealSenseID::MatchElement               g_NewMatchElement;
RealSenseID::FaceAuthenticator*         g_FaceAuthenticator;
CFaceprintsAuthClbk*                    g_CFaceprintsAuthClbk;
FaceMatchSate*                          g_FaceMatchSateList;


//. Tread Function
//. Proceed with matching the relevant area of the data base.
void check_user_faceprint(int p_nDBStartPos, int p_nDBEndPos) 
{
    FaceMatchSate       w_FaceMatchSate;

    for (int i = p_nDBStartPos; i < p_nDBEndPos; i++) {
        RealSenseID::Faceprints w_Faceprints = faceprints_db.at(i);

        w_FaceMatchSate = g_CFaceprintsAuthClbk->match_single_user(g_NewMatchElement, w_Faceprints, g_FaceAuthenticator);

        g_FaceMatchSateList[i] = w_FaceMatchSate;

    }
    return;
}

void CFaceprintsAuthClbk::OnHint(const RealSenseID::AuthenticateStatus hint)
{
}

void CFaceprintsAuthClbk::OnFaceDetected(const std::vector<RealSenseID::FaceRect>& faces, const unsigned int ts)
{

}

//. Convert JSON Data to RealSenseID::Faceprints
void CFaceprintsAuthClbk::AddFacePrintsDB(struct FaceData p_JSONFaceData)
{
    RealSenseID::Faceprints     w_Faceprints;

    w_Faceprints.data.version = p_JSONFaceData.features_version;
    w_Faceprints.data.featuresType = p_JSONFaceData.features_type;
    w_Faceprints.data.flags = p_JSONFaceData.features_flags;


    ::memcpy(w_Faceprints.data.adaptiveDescriptorWithoutMask, p_JSONFaceData.features, RSID_FEATURES_VECTOR_ALLOC_SIZE);
    ::memcpy(w_Faceprints.data.enrollmentDescriptor, p_JSONFaceData.features, RSID_FEATURES_VECTOR_ALLOC_SIZE);

    short   w_adaptiveDescriptorWithMask[RSID_FEATURES_VECTOR_ALLOC_SIZE];
    ::memset(w_adaptiveDescriptorWithMask, 0x00, sizeof(short) * RSID_FEATURES_VECTOR_ALLOC_SIZE);
    ::memcpy(w_Faceprints.data.adaptiveDescriptorWithMask, w_adaptiveDescriptorWithMask, RSID_FEATURES_VECTOR_ALLOC_SIZE);

    faceprints_db.push_back(w_Faceprints);
    return;
}

FaceMatchSate       //. Return : Face Match Result 
CFaceprintsAuthClbk::match_single_user(RealSenseID::MatchElement new_prints, RealSenseID::Faceprints user, RealSenseID::FaceAuthenticator* f_FaceAuthenticator)
{
    RealSenseID::ThresholdsConfidenceEnum matcher_confidence_level = RealSenseID::ThresholdsConfidenceEnum::ThresholdsConfidenceLevel_High;
    FaceMatchSate       w_FaceMatchSate;


    try {
        RealSenseID::Faceprints updated_faceprint = RealSenseID::Faceprints();

        auto match = f_FaceAuthenticator->MatchFaceprints(new_prints, user, updated_faceprint, matcher_confidence_level);

        w_FaceMatchSate.m_nCurrentScore = (int)match.score;
        w_FaceMatchSate.m_bSuccess = (bool)match.success;

    }
    catch (const std::exception& err) {
        std::cout << "exception error" << std::endl;
        w_FaceMatchSate.m_bSuccess = false;
        w_FaceMatchSate.m_nCurrentScore = 0;
    }

    return w_FaceMatchSate;
}
int CFaceprintsAuthClbk::match_users_parallel(RealSenseID::MatchElement new_prints, std::vector<RealSenseID::Faceprints> faceprints_db, RealSenseID::FaceAuthenticator* f_FaceAuthenticator)
{
    FaceMatchSate       w_FaceMatchSate;
    int                 w_nDBCnt = (int)faceprints_db.size();
    int                 w_nMaxScore = -100;
    int                 w_SelDB_Pos = -1;

    //. Single Thread
    //for (int i = 0; i < w_nDBCnt; i++) {
    //    RealSenseID::Faceprints w_Faceprints = faceprints_db.at(i);
    //    w_FaceMatchSate = match_single_user(new_prints, w_Faceprints, f_FaceAuthenticator);
    //    if (w_FaceMatchSate.m_bSuccess == true) {
    //        if (w_FaceMatchSate.m_nCurrentScore > w_nMaxScore) {
    //            w_nMaxScore = w_FaceMatchSate.m_nCurrentScore;
    //            w_SelDB_Pos = i;
    //        }
    //    }
    //}

    //. Muti Thread
    if (w_nDBCnt <= 0) {
        return w_SelDB_Pos;
    }

    g_FaceMatchSateList = new FaceMatchSate[w_nDBCnt];
    g_NewMatchElement = new_prints;
    g_FaceAuthenticator = f_FaceAuthenticator;
    g_CFaceprintsAuthClbk = this;

    int         w_nStartDBPos = 0;
    int         w_nEndDBPos = 0;

    // start g_NumThread :
    if (g_NumThread <= 0) {
        g_NumThread = 1;
    }
    int     batch_size = (w_nDBCnt / g_NumThread);
    if (batch_size <= 0) {
        batch_size = 1;
    }
    int     batch_Count = w_nDBCnt / batch_size;

    std::vector<std::thread>    my_threads(batch_Count);
    for (int i = 0; i < batch_Count; i++) {
        w_nStartDBPos = i * batch_size;
        if (i == batch_Count - 1) {
            w_nEndDBPos = w_nDBCnt;
        }
        else {
            w_nEndDBPos = w_nStartDBPos + batch_size;
        }
        my_threads[i] = std::thread(check_user_faceprint, w_nStartDBPos, w_nEndDBPos);
    }
    //. Thread end
    for (auto& th : my_threads) {
        th.join();
    }
    //. Check result
    for (int i = 0; i < w_nDBCnt; i++) {
         w_FaceMatchSate = g_FaceMatchSateList[i];
         if (w_FaceMatchSate.m_bSuccess == true) {
             if (w_FaceMatchSate.m_nCurrentScore > w_nMaxScore) {
                 w_nMaxScore = w_FaceMatchSate.m_nCurrentScore;
                 w_SelDB_Pos = i;
             }
         }
    }

    delete[] g_FaceMatchSateList;
    return w_SelDB_Pos;
}

// extract faceprints of the user in front of the device
void CFaceprintsAuthClbk::OnResult(const RealSenseID::AuthenticateStatus status, const RealSenseID::ExtractedFaceprints* new_prints)
{
    std::cout << "on_result: status: " << status << std::endl;

    if (status != RealSenseID::AuthenticateStatus::Success)
    {
        std::cout << "ExtractFaceprints failed with status " << status << std::endl;
        return;
    }

    RealSenseID::MatchElement scanned_faceprint;
    scanned_faceprint.data.version = new_prints->data.version;
    scanned_faceprint.data.featuresType = new_prints->data.featuresType;
    int32_t vecFlags = (int32_t)new_prints->data.featuresVector[RSID_INDEX_IN_FEATURES_VECTOR_TO_FLAGS];
    int32_t opFlags = RealSenseID::FaOperationFlagsEnum::OpFlagAuthWithoutMask;
    if (vecFlags == RealSenseID::FaVectorFlagsEnum::VecFlagValidWithMask)
    {
        opFlags = RealSenseID::FaOperationFlagsEnum::OpFlagAuthWithMask;
    }
    scanned_faceprint.data.flags = opFlags;
    //static_assert(sizeof(scanned_faceprint.data.featuresVector) == sizeof(new_prints->data.featuresVector), "faceprints without mask sizes does not match");
    ::memcpy(scanned_faceprint.data.featuresVector, new_prints->data.featuresVector, sizeof(new_prints->data.featuresVector));

    // try to match the resulting faceprint to one of the faceprints stored in the db
    int         selected_user_value = 0;
    selected_user_value = match_users_parallel(scanned_faceprint, faceprints_db, m_authenticator);
    if (selected_user_value >= 0) {

        std::string  w_strFaceID = g_CFaceJSONDB.GetFaceIDInfo(selected_user_value);
        std::cout << "Matched Database Position " << selected_user_value << std::endl;
        std::cout << "Matched Face ID " << w_strFaceID << std::endl;

    }
    else {
        std::cout << "No match found" << std::endl;
    }
}

//. Clear faceprints_db
void CFaceprintsAuthClbk::ClearFacePrintsDB() 
{
    faceprints_db.clear();
}


//void CFaceprintsAuthClbk::TestingCode()
//{
//    RealSenseID::MatchElement scanned_faceprint;
//
//    int         selected_user_value = 0;
//    selected_user_value = match_users_parallel(scanned_faceprint, faceprints_db, m_authenticator);
//    if (selected_user_value >= 0) {
//
//        std::string  w_strFaceID = g_CFaceJSONDB.GetFaceIDInfo(selected_user_value);
//        std::cout << "Matched Database Position " << selected_user_value << std::endl;
//        std::cout << "Matched Face ID " << w_strFaceID << std::endl;
//
//    }
//    else {
//        std::cout << "No match found" << std::endl;
//    }
//
//}




