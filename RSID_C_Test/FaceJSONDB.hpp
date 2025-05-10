#ifndef __FaceJSONDB_HPP__
#define __FaceJSONDB_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <rapidjson/document.h>

struct FaceData {
    std::string faceId;
    int     features_version;
    int     features_flags;
    int     features_type;
    short   features[259]; // RSID_FEATURES_VECTOR_ALLOC_SIZE
};

#define     LD_JS_SUCCESS      0
#define     LD_JS_ERR_PATH    -1
#define     LD_JS_ERR_INIT    -2
#define     LD_JS_ERR_PARAM   -3


#define     LD_JS_ERR_UNKNOWN   -99


// -------------------------------------------------
// Class FaceJSONDB
// -------------------------------------------------
class CFaceJSONDB
{
public:
    //.
    CFaceJSONDB();

    //.
    int LoadJSONFile(std::string p_strJSONPath);

    //.
    int GetFaceData(int p_nPos, struct FaceData& p_stFaceData);

    //.
    std::string GetFaceIDInfo(int p_nPos);

    //.
    int GetDataCount(int& p_nCount);

    //.
    int AddFaceData(struct FaceData p_stFaceData);


private:

    std::vector<FaceData>   m_faceDataDBList;

    int                     m_nItemCount;

    bool                    m_bInit;

};

#endif // __FaceJSONDB_HPP__